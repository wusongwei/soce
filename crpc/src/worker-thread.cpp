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
    }

    WorkerThread::~WorkerThread()
    {
        stop();
    }

    void WorkerThread::handle_reqs(int fd)
    {
        (void) fd;

        soce::utils::FQVector<RequestIn::QueueData> reqs;
        if (req_in_->try_consume(consumer_id_, reqs)){
            return;
        }

        for (auto& i : reqs){
            handle_req(i.service_, i.data_, i.conn_id_);
        }
    }

    void WorkerThread::append_req(const CrpcReqHeader& header, std::string&& request)
    {
        RpcType rpc_type = header.get_type();
        const string& service = header.get_service();
        const string& method = header.get_method();
        int64_t req_id = *(int64_t*)const_cast<char*>(header.get_req_id());
        std::set<uint64_t> conn_ids;

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
            conn_ids.insert(conn_id);
            request_stubs_[req_id].is_broadcast = false;
        }
        else{
            int rc = 0;
            if(rpc_type == kRpcTypeCondCast){
                rc = nameserver_->get_cond_servers(service, method, request, conn_ids);
            }
            else if (rpc_type == kRpcTypeUncondCast){
                rc = nameserver_->get_all_servers(service, method, request, conn_ids);
            }

            if (rc){
                conn_ids.clear();
            }
            request_stubs_[req_id].is_broadcast = true;
        }

        request_stubs_[req_id].conn_ids = conn_ids;
        req_out_->produce(conn_ids, tid_, req_id, std::move(request));
    }

    void WorkerThread::append_resp(uint64_t conn_id, int64_t req_id, std::string&& resp)
    {
        resp_in_->produce(consumer_id_, conn_id, req_id, std::move(resp));
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

    void WorkerThread::thread_entry()
    {
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
                resp_out_->produce(conn_id, std::move(resp));
            }
        }else if (svr->get_svr_proc_type() == ServiceIf::kSvrProcTypeCort){
            auto id = SCortEngine.create([=, &data]{
                    string resp = services_[service]->process(data);
                    if (!resp.empty()){
                        resp_out_->produce(conn_id, std::move(resp));
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

        soce::utils::FQVector<ResponseIn::QueueData> resps;
        if (resp_in_->try_consume(consumer_id_, resps)){
            return;
        }

        for (auto& i : resps){
            auto iter = request_stubs_.find(i.reqid_);
            if (iter == request_stubs_.end()){
                continue;
            }

            if (iter->second.is_broadcast) {
                static string ok = "BroadcastOk";
                static string error = "BroadcastError";

                size_t conn_size = iter->second.conn_ids.size();
                bool do_resp = true;
                if (conn_size == 0) {
                    last_resp_ = &ok;
                }else if (i.data_.empty()) {
                    last_resp_ = &error;
                }else if(conn_size == 1){
                    last_resp_ = &ok;
                }else{
                    iter->second.conn_ids.erase(i.conn_id_);
                    do_resp = false;
                }

                if (do_resp) {
                    SCortEngine.resume(iter->second.cid);
                    request_stubs_.erase(iter);
                }
            }else{
                last_resp_ = &i.data_;
                SCortEngine.resume(iter->second.cid);
                request_stubs_.erase(iter);
            }
        }
    }

} // namespace crpc
} // namespace soce
