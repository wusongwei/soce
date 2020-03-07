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

#ifndef _SOCE_CRPC_PROXY_DISPATCHER_HtG4foKKZO_H_
#define _SOCE_CRPC_PROXY_DISPATCHER_HtG4foKKZO_H_

#include <memory>
#include <string>
#include <vector>
#include "transport/transport-if.h"
#include "utils/dispatch-queue.hpp"
#include "gen-cpp/crpc-header.h"

namespace soce {
namespace crpc {

    class ProxyDispatcher
    {
    public:
        using ProxyRequest = struct ProxyRequest {
            ProxyRequest() {}
        ProxyRequest(uint64_t id, std::string&& req)
        :conn_id(id), request(std::move(req)){}

            uint64_t conn_id;
            std::string request;
        };

        using ProxyResponse = struct ProxyResponse{
            ProxyResponse(){}
        ProxyResponse(uint64_t id, bool err, std::string&& resp)
        :conn_id(id), error(err), response(std::move(resp)){}

            uint64_t conn_id;
            bool error = false;
            std::string response;
        };

        int init(int thread_index,
                 soce::transport::TransportIf* transport,
                 std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyRequest>> req_queue,
                 std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyResponse>> resp_queue);

    private:
        void recv_queue_message(int fd);
        void handle_queue_message(uint64_t conn_id, std::string&& request);
        void append_resp(uint64_t conn_id, bool err, soce::crpc::RpcStatus status);

    private:
        int thread_index_ = 0;
        std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyRequest>> req_queue_;
        std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyResponse>> resp_queue_;
        std::vector<uint32_t> reqid_index_{0, 3};
        std::vector<uint32_t> respid_index_{0, 1};
    };

} // namespace crpc
} // namespace soce

#endif
