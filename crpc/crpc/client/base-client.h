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

#ifndef _SOCE_CRPC_BASE_CLIENT_R4HkmDQfPA_H_
#define _SOCE_CRPC_BASE_CLIENT_R4HkmDQfPA_H_

#include <unordered_map>
#include "gen-cpp/crpc-header.h"
#include "proto/fads-message.h"
#include "crpc/interceptor.h"

namespace soce {
namespace crpc {

    class BaseClient
    {
    public:
        void set_timeout(size_t timeout_ms);
        void set_target_server(const std::string& addr);

    protected:
        void init_req_header(soce::crpc::CrpcReqHeader& header,
                             const std::string& service,
                             const std::string& method,
                             soce::crpc::RpcType type,
                             int64_t req_id);
        int do_before_itcptor(soce::crpc::CrpcReqHeader& header,
                              const std::string& service,
                              const std::string& method,
                              soce::fads::FadsMessage* resp_msg,
                              RpcStatus& status);
        uint32_t get_timeout(const std::string& method);
        RpcStatus deserialize_resp(soce::fads::FadsMessage* msg,
                                   const char* data,
                                   size_t len);
        RpcStatus do_after_itcptor(soce::crpc::CrpcRespHeader& header,
                                   const std::string& service,
                                   const std::string& method);

    protected:
        size_t timeout_ = 0;
        std::unordered_map<std::string, size_t> method_timeout_;
        std::string target_server_;

    public:
        static std::string BROADCASTOK;
        static std::string BROADCASTERROR;
    };

} // namespace crpc
} // namespace soce

#endif
