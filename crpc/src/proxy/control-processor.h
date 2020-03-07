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

#ifndef _SOCE_CRPC_PROXY_CONTROL_PROCESSOR_MhmaCM2Gcb_H_
#define _SOCE_CRPC_PROXY_CONTROL_PROCESSOR_MhmaCM2Gcb_H_

#include <vector>
#include <thread>
#include "crpc/base-processor.h"
#include "crpc/nameserver/name-server.h"
#include "transport/transport-libevent.h"
#include "local-req-processor.h"
#include "remote-req-processor.h"


namespace soce{
namespace crpc{

    class ProxyControlProcessor : public BaseProcessor
    {
    public:
        ProxyControlProcessor(const std::string& remote_req_addr_,
                              std::shared_ptr<NameServerIf> nameserver);
        ~ProxyControlProcessor();
        virtual void process(uint64_t conn_id, std::string&& data);
        virtual void on_connected(uint64_t conn_id);
        virtual void on_error(uint64_t conn_id);

    private:
        void handle_server_info_msg(char* data, size_t size);
        void handle_register_msg(char* data, size_t size);
        void handle_watch_service_msg(char* data, size_t size);
        void handle_remote_req(const std::string& addr);

    private:
        std::string remote_req_addr_;
        uint64_t local_server_fd_ = 0;
        std::shared_ptr<NameServerIf> nameserver_;
        std::vector<uint32_t> type_index_{0};
        std::thread remote_req_thread_;
        soce::transport::TransportLibevent transport_;
        bool run_ = false;
        std::shared_ptr<LocalReqProcessor> local_req_processor_;
        std::shared_ptr<RemoteReqProcessor> remote_req_processor_;

    };

} // namespace crpc
} // namespace soce


#endif
