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

#include "remote-req-processor.h"
#include "crpc/log.h"
#include "proto/dynamic-getter.h"
#include "proto/dynamic-setter.h"
#include "transport/transport-if.h"

namespace soce{
namespace crpc{

    void RemoteReqProcessor::set_local_server_fd(uint64_t local_server_fd)
    {
        local_server_fd_ = local_server_fd;
    }

    void RemoteReqProcessor::process(uint64_t conn_id, std::string&& request)
    {
        char* data = const_cast<char*>(request.data() + 4);
        size_t size = request.size() - 4;

        char old_req_id[8];
        soce::proto::DynamicGetter dg;
        dg.init(data, size);
        if (dg.get_bytes(reqid_index_, old_req_id, 8)) {
            // todo:
            return;
        }

        soce::proto::DynamicSetter ds;
        ds.init(data, size);
        if (ds.set_bytes(reqid_index_, (char*)&req_id_, 8)){
            return;
        }

        if (send_msg(data, size) == 0) {
            memcpy(request_infos_[req_id_].req_id, old_req_id, 8);
            request_infos_[req_id_].conn_id = conn_id;
            ++req_id_;
        }
    }

    void RemoteReqProcessor::on_connected(uint64_t conn_id)
    {
    }

    void RemoteReqProcessor::on_error(uint64_t conn_id)
    {
    }

    void RemoteReqProcessor::reply(std::string&& resp)
    {
        char* data = const_cast<char*>(resp.data() + 4);
        size_t size = resp.size() - 4;

        char req_id_buff[8];
        soce::proto::DynamicGetter dg;
        dg.init(data, size);
        if (dg.get_bytes(respid_index_, req_id_buff, 8)) {
            SOCE_ERROR << _D("Failed to get req_id");
            return;
        }

        int64_t req_id = *(int64_t*)(req_id_buff);
        auto iter = request_infos_.find(req_id);
        if (iter == request_infos_.end()) {
            SOCE_ERROR << _S("NoCacheForReqId", req_id);
            return;
        }

        soce::proto::DynamicSetter ds;
        ds.init(data, size);
        if (ds.set_bytes(respid_index_, iter->second.req_id, 8)){
            request_infos_.erase(iter);
            return;
        }

        CRPC_DEBUG << _S("SendRespTo", iter->second.conn_id) << _S("RespSize", resp.size());

        transport_->send(iter->second.conn_id, resp.data(), resp.size());
        request_infos_.erase(iter);
    }

    int RemoteReqProcessor::send_msg(const char* data, size_t size)
    {
        CRPC_DEBUG << _S("SendTo", local_server_fd_) << _S("MsgSize", size);

        char buff[4];
        for (size_t i =0; i<4; ++i){
            buff[i] = (size >> (3-i)*8) & 0xff;
        }

        if (transport_->send(local_server_fd_, buff, 4)
            || transport_->send(local_server_fd_, data, size)){
            return -1;
        }

        return 0;
    }

} // namespace crpc
} // namespace soce

