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

#include <unistd.h>
#include <sys/eventfd.h>
#include "crpc/worker-thread.h"
#include "crpc/log.h"
#include "dispatch-queue.h"
#include "transport/bypass-processor.h"
#include "proto/dynamic-getter.h"
#include "cort-engine/cort-engine.h"

#include <sys/syscall.h>
#define gettid() syscall(__NR_gettid)

using std::string;
using std::vector;
using soce::transport::BypassProcessor;

namespace soce{
namespace crpc{

    thread_local WorkerThread* WorkerThread::s_self = nullptr;

    WorkerThread::WorkerThread(int tid,
                               size_t consumer_id,
                               std::shared_ptr<RequestIn> req_in,
                               std::shared_ptr<RequestOut> req_out,
                               std::shared_ptr<ResponseIn> resp_in,
                               std::shared_ptr<ResponseOut> resp_out,
                               std::shared_ptr<NameServerIf> nameserver,
                               std::shared_ptr<soce::transport::TransportIf> transport,
                               std::shared_ptr<soce::utils::Snowflake> snowflake)
        : tid_(tid),
          consumer_id_(consumer_id),
          req_in_(req_in),
          req_out_(req_out),
          resp_in_(resp_in),
          resp_out_(resp_out),
          nameserver_(nameserver),
          transport_(transport),
          snowflake_(snowflake)
    {
        transport_->add_timer(check_cort_gap_,
                              soce::transport::TransportIf::kTimerModePresist,
                              std::bind(&WorkerThread::check_cort_timeout, this));
        steal_tmid = transport_->add_timer(steal_gap_,
                                           soce::transport::TransportIf::kTimerModeOnceAndRetain,
                                           std::bind(&WorkerThread::steal_jobs, this));
    }

    WorkerThread::~WorkerThread()
    {
        stop();
    }

    void WorkerThread::handle_reqs(int fd)
    {
        (void) fd;

        soce::utils::DQVector<RequestIn::QueueData> reqs;
        if (req_in_->try_consume(consumer_id_, reqs)){
            return;
        }

        for (auto& i : reqs){
            handle_req(i.service_, i.data_, i.conn_id_);
        }

        steal_jobs();
    }

    void WorkerThread::append_req(const CrpcReqHeader& header, std::string&& request)
    {
        RpcType rpc_type = header.get_type();
        const string& service = header.get_service();
        const string& method = header.get_method();
        int64_t req_id = header.get_req_id();
        std::vector<uint64_t> conn_ids;

        if (rpc_type == kRpcTypeReq){
            uint64_t conn_id = 0;
            if (header.has_target_addr() && !header.get_target_addr().empty()){
                conn_id = nameserver_->get_server(service,
                                                  method,
                                                  request,
                                                  header.get_target_addr());
            }
            else{
                conn_id = nameserver_->get_server(service, method, request);
            }

            conn_ids.push_back(conn_id);
            req_out_->produce(tid_, conn_ids, tid_, req_id, std::move(request), true);
        }
        else{
            vector<uint64_t> conn_ids;
            int rc = 0;
            if(rpc_type == kRpcTypeCondCast){
                rc = nameserver_->get_cond_servers(service, method, request, conn_ids);
            }
            else if (rpc_type == kRpcTypeUncondCast){
                rc = nameserver_->get_all_servers(service, method, request, conn_ids);
            }

            if (!rc){
                req_out_->produce(tid_, conn_ids, tid_, req_id, std::move(request), false);
            }
        }
    }

    void WorkerThread::append_resp(int64_t req_id, std::string&& resp)
    {
        resp_in_->produce(consumer_id_, req_id, std::move(resp));
    }

    void WorkerThread::stop()
    {
        transport_->stop();

        if (thread_.joinable()){
            thread_.join();
        }
    }

    void WorkerThread::check_cort_timeout()
    {
        SCortEngine.check_timeout();
    }

    void WorkerThread::steal_jobs()
    {
        if (!req_in_->empty(consumer_id_)){
            return;
        }

        req_in_->rebalance(consumer_id_);

        if (req_in_->empty(consumer_id_)){
            transport_->restart_timer(steal_tmid, steal_gap_);
        }
    }

    void WorkerThread::thread_entry()
    {
        SAsyncLogger.init_thread_logger();
        WorkerThread::s_self = this;

        transport_->watch(req_in_->get_consumer_fd(consumer_id_),
                          std::shared_ptr<BypassProcessor>(new BypassProcessor(
                                                               std::bind(&WorkerThread::handle_reqs,
                                                                         this,
                                                                         std::placeholders::_1))));

        transport_->watch(resp_in_->get_consumer_fd(consumer_id_),
                          std::shared_ptr<BypassProcessor>(new BypassProcessor(
                                                               std::bind(&WorkerThread::handle_resp,
                                                                         this,
                                                                         std::placeholders::_1))));
        for (auto& i : services_){
            i.second->start();
        }

        transport_->run();

        for (auto& i : services_){
            i.second->end();
        }
    }

    void WorkerThread::handle_req(const std::string& service,
                                  const std::string& data,
                                  uint64_t conn_id)
    {
        CRPC_DEBUG << _S("consumer_id", consumer_id_) << _S("service", service);

        auto& svr = services_[service];
        if (svr->get_svr_proc_type() == ServiceIf::kSvrProcTypeSync){
            string resp = services_[service]->process(data);
            if (!resp.empty()){
                resp_out_->produce(tid_, conn_id, std::move(resp));
            }
        }else if (svr->get_svr_proc_type() == ServiceIf::kSvrProcTypeCort){
            auto id = SCortEngine.create([&]{
                    string resp = services_[service]->process(data);
                    if (!resp.empty()){
                        resp_out_->produce(tid_, conn_id, std::move(resp));
                    }
                });
            SCortEngine.resume(id);
        }else{
            CRPC_DEBUG << _S("service", service) << _S("NotSupportSvrProcType", svr->get_svr_proc_type());
        }
    }

    void WorkerThread::handle_resp(int fd)
    {
        (void) fd;

        soce::utils::DQVector<ResponseIn::QueueData> resps;
        if (resp_in_->try_consume(consumer_id_, resps)){
            return;
        }

        for (auto& i : resps){
            CRPC_DEBUG << _S("ConsumerId", consumer_id_) << _S("reqid", i.reqid_);

            auto iter = requests_.find(i.reqid_);
            if (iter == requests_.end()){
                continue;
            }

            last_resp_ = &i.data_;
            SCortEngine.resume(iter->second);
            requests_.erase(iter->first);
        }
    }

} // namespace crpc
} // namespace soce
