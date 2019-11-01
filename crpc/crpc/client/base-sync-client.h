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

#ifndef _SOCE_CRPC_BASE_SYNC_CLIENT_bjLt2pmXyh_H_
#define _SOCE_CRPC_BASE_SYNC_CLIENT_bjLt2pmXyh_H_

#include "base-client.h"

namespace soce {
namespace crpc {

    class BaseSyncClient : public BaseClient
    {
    public:
        BaseSyncClient();
        ~BaseSyncClient();
        int init(const std::string& host, int port);
        inline char* data(){return data_;}
        inline size_t size(){return size_;}

    protected:
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
        RpcStatus send_req(const char* data, size_t len);
        RpcStatus send_data(const char* data, size_t len);
        void reset();
        RpcStatus recv_resp(uint32_t timeout);
        RpcStatus do_connect();
        RpcStatus do_recv_resp();
        int do_recv(size_t total);
        size_t get_msg_len();
        bool is_completed();

    private:
        using State = enum{
            kStateRecvHeader,
            kStateRecvBody
        };

    private:
        int fd_ = 0;
        std::string host_;
        int port_ = 0;
        char* data_ = NULL;
        size_t cap_ = 1024;
        size_t size_ = 0;
        size_t remain_len_ = 0;
        State state_ = kStateRecvHeader;

    };

} // namespace crpc
} // namespace soce

#endif
