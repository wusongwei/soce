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

#ifndef _SOCE_FADSDB_DBSERVER_12vEfEkJRQ_H_
#define _SOCE_FADSDB_DBSERVER_12vEfEkJRQ_H_

#include <unordered_map>
#include "dbrpc.h"
#include "dbrpc-service.h"
#include "crpc/server.h"
#include "dbcore.h"
#include "cort-engine/cort-engine.h"

using namespace soce::crpc;
using namespace soce::transport;
using soce::cortengine::CortEngine;

namespace soce{
namespace fadsdb{

    class FadsDbService : public FadsDbServiceIf
    {
    public:
        FadsDbService(size_t cores);
        virtual void start();
        virtual string exec(const int64_t& id, const string& req);

    private:
        void handle_resp(int fd);

    private:
        size_t cores_ = 1;
        DbCore dbcore_;
        std::string response_;
        std::unordered_map<int64_t, CortEngine::CortId> req_map_;
    };

    class FadsDbServiceFactory : public ServiceFactoryIf
    {
    public:
        FadsDbServiceFactory(size_t cores);
        virtual std::shared_ptr<ServiceIf> create();

    private:
        size_t cores_ = 1;
    };

    class LibeventTransportFactory : public TransportFactoryIf
    {
    public:
        virtual std::shared_ptr<TransportIf> create();
    };

    class ProcessorFactory : public ProcessorFactoryIf
    {
    public:
        ProcessorFactory(size_t cores);
        virtual std::shared_ptr<Processor> create();

    private:
        size_t cores_ = 1;
    };

    class NameServerFactory : public NameServerFactoryIf
    {
    public:
        virtual std::shared_ptr<NameServerIf> create();
    };

    class FadsDbNameServer : public NameServerIf
    {
    public:
        void set_processor(std::shared_ptr<soce::transport::ProcessorIf> processor)
        {
            processor_ = processor;
            transport_ = processor_->get_transport();
        }

        void watch_service(const std::string& service) {}
        virtual void watch_all(){}
        virtual void on_connected(uint64_t conn_id) {}
        virtual int add_server(const std::string& service,
                               const std::string& value){return 0;}
        virtual void del_server(uint64_t conn_id) {}
        virtual uint64_t get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request){return 0;}
        virtual uint64_t get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    const std::string& target_addr){return 0;}
        virtual int get_cond_servers(const std::string& service,
                                     const std::string& method,
                                     const std::string& request,
                                     std::set<uint64_t>& conn_ids){return 0;};
        virtual int get_all_servers(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    std::set<uint64_t>& conn_ids){return 0;}
        virtual void do_watch_service(const std::string& service){}
        virtual void run(){}
    };

    class FadsDbServer
    {
    public:
        int start(const std::string& server_addr, size_t cores);
    };

} //namespace fadsdb
} // namespace soce

#endif
