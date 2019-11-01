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

#include "crpc/client/base-client.h"
#include "proto/binary-proto.h"

namespace soce {
namespace crpc {

    std::string BaseClient::BROADCASTOK = "BroadcastOk";
    std::string BaseClient::BROADCASTERROR = "BroadcastError";

    void BaseClient::set_timeout(size_t timeout_ms)
    {
        timeout_ = timeout_ms;
    }

    void BaseClient::set_target_server(const std::string& addr)
    {
        target_server_ = addr;
    }

    void BaseClient::init_req_header(soce::crpc::CrpcReqHeader& header,
                                     const std::string& service,
                                     const std::string& method,
                                     soce::crpc::RpcType type,
                                     int64_t req_id)
    {
        header.set_type(type);
        header.set_service(service);
        header.set_method(method);
        header.set_req_id((char*)&req_id);
    }

    int BaseClient::do_before_itcptor(soce::crpc::CrpcReqHeader& header,
                                      const std::string& service,
                                      const std::string& method,
                                      soce::fads::FadsMessage* resp_msg,
                                      RpcStatus& status)
    {
        std::string resp = SInterceptor.do_before_itcptor(header, service, method, true);
        if (!resp.empty()) {
            if (resp.size() < 4){
                status = kRpcStatusAOPFailed;
                return -1;
            }

            soce::proto::BinaryProto bp;
            bp.init(const_cast<char*>(resp.c_str() + 4), resp.size() - 4);
            if (resp_msg->deserialize(&bp)){
                status = kRpcStatusAOPFailed;
                return -1;
            }else{
                status = kRpcStatusOK;
                return 1;
            }
        }

        return 0;
    }

    uint32_t BaseClient::get_timeout(const std::string& method)
    {
        uint32_t timeout = timeout_;
        auto iter = method_timeout_.find(method);
        if (iter != method_timeout_.end()){
            timeout = iter->second;
        }

        return timeout;
    }

    RpcStatus BaseClient::deserialize_resp(soce::fads::FadsMessage* resp_msg,
                                           const char* data,
                                           size_t len)
    {
        if (len == BROADCASTOK.size()
            && 0 == strncmp(data, BROADCASTOK.c_str(), len)){
            return kRpcStatusBroadcastOk;
        }

        if (len == BROADCASTERROR.size()
            && 0 == strncmp(data, BROADCASTERROR.c_str(), len)){
            return kRpcStatusBroadcastError;
        }

        soce::proto::BinaryProto bp;
        if (len < 4
            || bp.init(const_cast<char*>(data + 4), len - 4)
            || 0 == resp_msg->deserialize(&bp)){
            return kRpcStatusProtoError;
        }

        return kRpcStatusOK;
    }

    RpcStatus BaseClient::do_after_itcptor(soce::crpc::CrpcRespHeader& header,
                                           const std::string& service,
                                           const std::string& method)
    {
        SInterceptor.do_after_itcptor(header, service, method, true);
        return header.get_status();
    }

} // namespace crpc
} // namespace soce
