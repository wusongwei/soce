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

#include "transport/transport-if.h"
#include "crpc/base-processor.h"
#include "proto/binary-proto.h"
#include "gen-cpp/crpc-header.h"

namespace soce {
namespace crpc {

    size_t BaseProcessor::get_max_header_size()
    {
        return 4;
    }

    size_t BaseProcessor::get_full_pkt_size(const char* data, size_t len)
    {
        uint32_t size = 0;
        if (len == 4){
            for (size_t i = 0; i < 4; ++i){
                size = (size << 8) | (*(data + i) & 0xFF);
            }
        }

        return size + 4;
    }

    int BaseProcessor::send(uint64_t conn_id, const char* data, size_t len)
    {
        char buff[4];
        for (size_t i =0; i<4; ++i){
            buff[i] = (len >> (3-i)*8) & 0xff;
        }

        if (transport_->send(conn_id, buff, 4)
            || transport_->send(conn_id, data, len)){
            return -1;
        }

        return 0;
    }

    void BaseProcessor::close(uint64_t conn_id)
    {
        transport_->close(conn_id);
    }

} // namespace crpc
} // namespace soce
