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

#include <sys/eventfd.h>
#include <sys/select.h>
#include <unistd.h>
#include "utils/funnel-buffer.h"

namespace soce {
namespace utils {

    FunnelBuffer::FunnelBuffer()
        : DoubleCacheQueue(kStrategyYielding, 1024*1024, 1024)
    {
        init();
    }

    FunnelBuffer::FunnelBuffer(DoubleCacheQueue::WaitStrategy strategy,
                               size_t buff_size,
                               size_t water_mark)
        : DoubleCacheQueue(strategy, buff_size, water_mark)
    {
        init();
    }

    FunnelBuffer::~FunnelBuffer()
    {
        for (int i=0; i<2; ++i){
            if (buffers_[i] != NULL) {
                delete [] buffers_[i];
            }
        }
    }

    int FunnelBuffer::init()
    {
        for (int i=0; i<2; ++i){
            if (buffers_[i] != NULL) {
                delete [] buffers_[i];
            }

            buffers_[i] = new char[elem_size_];
        }

        if (0 == water_mark_) {
            water_mark_ = elem_size_ * 0.5;
        }

        return evtfd_mail_.good() ? 0 : -1;
    }

     void FunnelBuffer::put(const char* data, size_t size)
     {
         add(size, [this, &data, &size](size_t buff_index, size_t cursor){
                 memcpy(buffers_[buff_index] + cursor, data, size);
             });
     }

    void FunnelBuffer::flush(std::function<void(const char*, size_t)> handler, size_t interval)
    {
        try_consume_for([this, &handler](size_t buff_index, size_t cursor){
                handler(buffers_[buff_index], cursor);
            }, interval);
    }

} // namespace utisl
} // namespace soce
