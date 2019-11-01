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

#ifndef _SOCE_CRPC_BASE_CORT_CLIENT_VYw6Eq4ivx_H_
#define _SOCE_CRPC_BASE_CORT_CLIENT_VYw6Eq4ivx_H_

#include "gen-cpp/crpc-header.h"
#include "base-client.h"

namespace soce {
namespace crpc {

    class BaseCortClient : public BaseClient
    {
    public:
        RpcStatus do_req(const std::string& service,
                         const std::string& method,
                         soce::crpc::RpcType type,
                         int64_t req_id,
                         soce::crpc::CrpcReqHeader& req_header,
                         soce::fads::FadsMessage* req_msg,
                         soce::crpc::CrpcRespHeader& resp_header,
                         soce::fads::FadsMessage* resp_msg);

        /* for null request */
        RpcStatus do_req(const std::string& service,
                         const std::string& method,
                         soce::crpc::RpcType type,
                         int64_t req_id,
                         soce::crpc::CrpcReqHeader& req_header,
                         soce::fads::FadsMessage* req_msg);
    };

} // namespace crpc
} // namespace soce


#endif
