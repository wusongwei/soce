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

#ifndef _SOCE_UTILS_FUNNEL_BUFFER_FVQoDIO9I4_H_
#define _SOCE_UTILS_FUNNEL_BUFFER_FVQoDIO9I4_H_

#include "double-cache-queue.h"

namespace soce {
namespace utils {

    class FunnelBuffer : public DoubleCacheQueue
    {
    public:
        FunnelBuffer();
        FunnelBuffer(DoubleCacheQueue::WaitStrategy strategy,
                     size_t buff_size,
                     size_t water_mark);
        ~FunnelBuffer();

        int init();
        void put(const char* data, size_t size);
        void flush(std::function<void(const char*, size_t)> handler, size_t interval);

    private:
        char* buffers_[2] = {NULL};
    };

} // namespace utisl
} // namespace soce

#endif
