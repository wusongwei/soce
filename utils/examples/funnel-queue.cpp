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
#include <unistd.h>
#include "utils/funnel-queue.hpp"
#include "log4rel/logger.h"

using namespace soce::utils;

#define N 2

class ExampleFunnelQueue
{
public:
    void start()
        {
            SOCE_DEBUG << _D("--------------- sample_funnel_queue ---------------");

            for (int i=0; i<N; ++i){
                producer_[i] = std::thread(&ExampleFunnelQueue::produce, this);
            }

            consumer_ = std::thread(&ExampleFunnelQueue::consume, this);

            for (int i=0; i<N; ++i){
                producer_[i].join();
            }
            consumer_.join();
        }

    void produce()
        {
            for (size_t i=0; i<msg_size_; ++i) {
                fq_.produce(i);
            }
        }

    void consume()
        {
            size_t total = 0;
            int fd = fq_.get_notify_fd();
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 10000;

            while (1) {
                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(fd, &readfds);
                select(fd + 1, &readfds, NULL, NULL, &tv);

                FQVector<int> msg = fq_.try_consume();
                total += msg.size();

                usleep(10000);

                for (auto& i : msg) {
                    SOCE_DEBUG << _S("msg", i);
                }

                if (total >= N * msg_size_) {
                    break;
                }
            }
        }

private:
    FunnelQueue<int> fq_;
    std::thread producer_[N];
    std::thread consumer_;
    size_t msg_size_ = 5;
};

void funnel_queue()
{
    ExampleFunnelQueue efq;
    efq.start();
}
