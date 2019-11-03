/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "dbserver.h"
#include "proto/binary-proto.h"
#include "transport/transport-libevent.h"
#include "transport/bypass-processor.h"
#include "utils/dispatch-queue.hpp"

using soce::transport::BypassProcessor;
using soce::transport::TransportLibevent;
using soce::proto::BinaryProto;

#include <iostream>
using namespace std;

namespace soce{
namespace fadsdb{

    FadsDbService::FadsDbService(size_t cores)
        : cores_(cores)
    {
        dbcore_.init(cores_);
    }

    void FadsDbService::start()
    {
        int fd = dbcore_.get_resp_queue()->get_consumer_fd(0);
        transport_->watch(fd,
                          std::shared_ptr<BypassProcessor>(new BypassProcessor(
                                                               std::bind(&FadsDbService::handle_resp,
                                                                         this,
                                                                         std::placeholders::_1))));
    }

    string FadsDbService::exec(const int64_t& id, const string& req)
    {
        dbcore_.do_sql(id, req.c_str(), req.size());
        req_map_[id] = SCortEngine.get_cur_cort_id();
        SCortEngine.yield();
        return std::move(response_);
    }

    void FadsDbService::handle_resp(int fd)
    {
        (void) fd;

        soce::utils::FQVector<DbCore::RespData> resp;
        dbcore_.get_resp_queue()->try_consume(0, resp);

        for (auto& i : resp){
            auto iter = req_map_.find(i.req_id_);
            if (iter != req_map_.end()) {
                auto cid = iter->second;
                FadsDbResponse resp;
                resp.set_id(i.req_id_);
                resp.set_status(i.result_);
                resp.set_response(std::move(i.data_));
                soce::proto::BinaryProto bp;
                resp.serialize(&bp);
                response_ = std::move(std::string(bp.data(), bp.size()));
                req_map_.erase(iter);
                SCortEngine.resume(cid);
            }
        }
    }

    FadsDbServiceFactory::FadsDbServiceFactory(size_t cores)
        : cores_(cores)
    {
    }

    std::shared_ptr<ServiceIf> FadsDbServiceFactory::create()
    {
        return std::shared_ptr<ServiceIf>(new FadsDbService(cores_));
    }

    std::shared_ptr<TransportIf> LibeventTransportFactory::create()
    {
        TransportLibevent* transport = new TransportLibevent;
        transport->init();
        return std::shared_ptr<TransportIf>(transport);
    }

    ProcessorFactory::ProcessorFactory(size_t cores)
        : cores_(cores)
    {
    }

    std::shared_ptr<Processor> ProcessorFactory::create()
    {
        std::shared_ptr<Processor> processor(new Processor);

        processor->set_shared_thread_num(1);
        processor->add_service_factory(std::shared_ptr<ServiceFactoryIf>(new FadsDbServiceFactory(cores_)), true);
        return processor;
    }

    std::shared_ptr<NameServerIf> NameServerFactory::create()
    {
        return std::shared_ptr<NameServerIf>(new FadsDbNameServer);
    }

    int FadsDbServer::start(const std::string& server_addr, size_t cores)
    {
        Server server(server_addr,
                      std::shared_ptr<TransportFactoryIf>(new LibeventTransportFactory),
                      std::shared_ptr<ProcessorFactoryIf>(new ProcessorFactory(cores)),
                      std::shared_ptr<NameServerFactoryIf>(new NameServerFactory));


        server.run();
        return 0;
    }

} //namespace fadsdb
} // namespace soce
