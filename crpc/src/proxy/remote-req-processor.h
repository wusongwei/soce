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

#ifndef _SOCE_CRPC_PROXY_REMOTE_REQ_PROCESSOR_aCzagELP1n_H_
#define _SOCE_CRPC_PROXY_REMOTE_REQ_PROCESSOR_aCzagELP1n_H_

#include <unordered_map>
#include <vector>
#include "crpc/base-processor.h"

namespace soce{
namespace crpc{

    class RemoteReqProcessor : public BaseProcessor
    {
    public:
        void set_local_server_fd(uint64_t local_server_fd);
        virtual void process(uint64_t conn_id, std::string&& data);
        virtual void on_connected(uint64_t conn_id);
        virtual void on_error(uint64_t conn_id);
        void reply(std::string&& request);

    private:
        int send_msg(const char* data, size_t size);

    private:
        using RequestInfo = struct {
            char req_id[8];
            uint64_t conn_id;
        };

        uint64_t local_server_fd_ = 0;
        int64_t req_id_ = 1;
        std::vector<uint32_t> reqid_index_{0, 3};
        std::vector<uint32_t> respid_index_{0, 1};
        std::unordered_map<int64_t, RequestInfo> request_infos_;
    };

} // namespace crpc
} // namespace soce

#endif
