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

#ifndef _SOCE_PROCESSOR_weIlLIA1Xy_H_
#define _SOCE_PROCESSOR_weIlLIA1Xy_H_

#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include "base-processor.h"
#include "service-if.h"
#include "nameserver/name-server.h"
#include "utils/snowflake.h"
#include "factory.h"

namespace soce{
namespace crpc{

    class RequestIn;
    class RequestOut;
    class ResponseOut;
    class WorkerThread;

    class Processor : public BaseProcessor
    {
    public:
        Processor();
        ~Processor() = default;

        virtual void process(uint64_t conn_id, std::string&& data);
        virtual void on_connected(uint64_t conn_id);
        virtual void on_error(uint64_t conn_id);

        inline void set_transport_factory(std::shared_ptr<TransportFactoryIf> factory)
        {
            transport_factory_ = factory;
        }
        inline void set_shared_thread_num(size_t num) {shared_threads_ = num;}
        inline void set_nameserver(std::shared_ptr<NameServerIf> ns) {nameserver_ = ns;}
        inline void set_snowflake(std::shared_ptr<soce::utils::Snowflake> snowflake) {snowflake_ = snowflake;}
        inline std::shared_ptr<soce::utils::Snowflake> get_snowflake() {return snowflake_;}
        int add_service_factory(std::shared_ptr<ServiceFactoryIf> factory, bool share_thread);
        std::string get_service_info();
        int run();

    protected:
        void watch_request_out();
        void watch_response_out();
        void handle_request_out(int fd);
        void handle_response_out(int fd);
        void append_null_resp(uint64_t conn_id, int64_t req_id, int32_t tid);
        void register_to_ns();
        int run_thread_shared_service(size_t cores, int& tid);
        int run_thread_per_service(int& tid);

    protected:
        size_t shared_threads_ = 0;
        std::shared_ptr<TransportFactoryIf> transport_factory_;
        std::vector<std::shared_ptr<ServiceFactoryIf>> service_factories_;
        std::unordered_map<std::string, std::shared_ptr<ServiceIf>> thread_shared_services_;
        std::unordered_map<std::string, std::shared_ptr<ServiceIf>> thread_per_services_;
        std::shared_ptr<NameServerIf> nameserver_;
        std::unordered_map<std::string, std::shared_ptr<RequestIn>> req_in_;
        std::shared_ptr<RequestOut> req_out_;
        std::shared_ptr<ResponseOut> resp_out_;
        std::shared_ptr<soce::utils::Snowflake> snowflake_;
        std::unordered_map<int, std::shared_ptr<WorkerThread>> dispatch_threads_;

        /* map<conn_id, map<tid, set<reqid>>> */
        std::unordered_map<uint64_t, std::unordered_map<int, std::unordered_set<int64_t>>> conn_id_map_;
        /* map<reqid, pair<conn_id, tid>> */
        std::unordered_map<int64_t, std::pair<uint64_t, int>> reqid_map_;

        /* in CrpcReqHeader */
        std::vector<uint32_t> service_index_{0, 1};

        /* in CrpcRespHeader */
        std::vector<uint32_t> type_index_{0, 0};
        std::vector<uint32_t> reqid_index_{0, 1};
    };

} // namespace crpc
} // namespace soce

#endif
