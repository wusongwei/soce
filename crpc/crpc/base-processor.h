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

#ifndef _SOCE_CRPC_BASE_PROCESSOR_EbuQgWxPTw_H_
#define _SOCE_CRPC_BASE_PROCESSOR_EbuQgWxPTw_H_

#include "transport/processor-if.h"

namespace soce {
namespace crpc {

    class BaseProcessor : public soce::transport::ProcessorIf
    {
    public:
        virtual size_t get_max_header_size();
        virtual size_t get_full_pkt_size(const char* data, size_t len);

    protected:
        int send(uint64_t conn_id, const char* data, size_t len);
        void close(uint64_t conn_id);
    };

} // namespace crpc
} // namespace soce
#endif
