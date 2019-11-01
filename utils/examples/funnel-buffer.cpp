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

#include <thread>
#include "utils/funnel-buffer.h"
#include "log4rel/logger.h"

using namespace soce::utils;

class ExampleFunnelBuffer
{
public:
    void start()
        {
            producer_ = std::thread(&ExampleFunnelBuffer::produce, this);
            consumer_ = std::thread(&ExampleFunnelBuffer::consume, this);

            producer_.join();
            consumer_.join();
        }

    void produce()
        {
            char c = 'a';
            for (size_t i=0; i<msg_size_; ++i) {
                fb_.put(&c, 1);
            }
        }

    void consume()
        {
            size_t total = 0;
            while (1) {
                fb_.flush([this, &total](const char* data, size_t len){
                        SOCE_DEBUG << _S("msg", std::string(data, len));
                        total += len;
                    }, 500);

                if (total >= msg_size_) {
                    break;
                }
            }
        }

private:
    FunnelBuffer fb_;
    std::thread producer_;
    std::thread consumer_;
    size_t msg_size_ = 10;
};

void funnel_buffer()
{
    ExampleFunnelBuffer efb;
    efb.start();
}
