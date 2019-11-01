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

#ifndef _SOCE_CRPC_PROXY_ENTRY_PROCESSOR_s41TLfgeVA_H_
#define _SOCE_CRPC_PROXY_ENTRY_PROCESSOR_s41TLfgeVA_H_

#include "crpc/base-processor.h"
#include "utils/dispatch-queue.hpp"
#include "dispatcher.h"

namespace soce {
namespace crpc {

    class ProxyEntryProcessor : public BaseProcessor
    {
    public:
        ProxyEntryProcessor(size_t thread_num,
                            soce::transport::TransportIf* transport);
        virtual void process(uint64_t conn_id, std::string&& data);
        virtual void on_error(uint64_t conn_id);

        std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyRequest>> get_req_queue();
        std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyResponse>> get_resp_queue();

    private:
        void handle_resp(int fd);

    private:
        size_t shared_thread_num_ = 0;
        std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyRequest>> req_queue_;
        std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyResponse>> resp_queue_;
        int count_ = 0;
    };

} // namespace crpc
} // namespace soce
#endif
