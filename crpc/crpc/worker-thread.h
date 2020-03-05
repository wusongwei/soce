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

#ifndef _SOCE_WORKER_THREAD_04K7pILYte_H_
#define _SOCE_WORKER_THREAD_04K7pILYte_H_

#include <memory>
#include <thread>
#include <vector>
#include <set>
#include <unordered_map>
#include <mutex>
#include "service-if.h"
#include "nameserver/name-server.h"
#include "transport/transport-if.h"
#include "cort-engine/cort-engine.h"
#include "utils/snowflake.h"
#include "utils/group-list.hpp"
#include "gen-cpp/crpc-header.h"

namespace soce{
namespace crpc{

    class RequestIn;
    class RequestOut;
    class ResponseIn;
    class ResponseOut;

    class WorkerThread
    {
    public:
        using RequestStub = struct {
            std::set<uint64_t> conn_ids;
            bool is_broadcast = false;
            soce::cortengine::CortEngine::CortId cid;
        };

    public:
        WorkerThread(int tid,
                     size_t consumer_id,
                     std::shared_ptr<RequestIn> req_in,
                     std::shared_ptr<RequestOut> req_out,
                     std::shared_ptr<ResponseIn> resp_in,
                     std::shared_ptr<ResponseOut> resp_out,
                     std::shared_ptr<NameServerIf> nameserver,
                     std::shared_ptr<soce::transport::TransportIf> transport,
                     std::shared_ptr<soce::utils::Snowflake> snowflake);
        ~WorkerThread();

        inline void add_service(std::shared_ptr<ServiceIf> service) {
            services_[service->get_name()] = service;
        }
        inline void add_request(int64_t req_id, soce::cortengine::CortEngine::CortId cid){
            request_stubs_[req_id].cid = cid;
        }
        inline void del_request(int64_t req_id){
            request_stubs_.erase(req_id);
        }
        inline void run(){
            thread_ = std::thread(&WorkerThread::thread_entry, this);
        }
        inline std::shared_ptr<soce::transport::TransportIf> get_transport() {return transport_;}
        inline int64_t get_reqid() {return snowflake_->gen_id();}
        inline std::string* get_last_resp() {return last_resp_;}

        void append_req(const CrpcReqHeader& header, std::string&& req);
        void append_resp(uint64_t conn_id, int64_t req_id, std::string&& resp);
        void stop();
        void check_cort_timeout();
        void steal_jobs();

    public:
        static thread_local WorkerThread* s_self;

    private:
        void thread_entry();
        void handle_reqs(int fd);
        void handle_req(const std::string& service,
                        const std::string& data,
                        uint64_t conn_id);
        void handle_resp(int fd);

    private:
        int tid_ = 0;
        size_t consumer_id_ = 0;
        const uint32_t check_cort_gap_ = 500;
        const uint32_t steal_gap_ = 10;
        soce::transport::TransportIf::tmid_t steal_tmid = 0;
        std::thread thread_;
        std::shared_ptr<RequestIn> req_in_;
        std::shared_ptr<RequestOut> req_out_;
        std::shared_ptr<ResponseIn> resp_in_;
        std::shared_ptr<ResponseOut> resp_out_;
        std::shared_ptr<NameServerIf> nameserver_;
        std::shared_ptr<soce::transport::TransportIf> transport_;
        std::shared_ptr<soce::utils::Snowflake> snowflake_;
        std::unordered_map<std::string, std::shared_ptr<ServiceIf>> services_;
        std::unordered_map<int64_t, RequestStub> request_stubs_;
        std::string* last_resp_ = nullptr;
    };

} // namespace crpc
} // namespace soce

#endif
