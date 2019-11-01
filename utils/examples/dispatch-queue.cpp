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
#include <functional>
#include <atomic>
#include <unistd.h>
#include "log4rel/logger.h"
#include "utils/dispatch-queue.hpp"

using namespace soce::utils;

#define N 2
class ExampleDispatchQueue
{
public:
    ExampleDispatchQueue()
        {
            dq_.reset(new DispatchQueue<int>(N, std::bind(&ExampleDispatchQueue::consumer_selector,
                                                             this,
                                                             std::placeholders::_1,
                                                             std::placeholders::_2)));

            dq_.reset(new DispatchQueue<int>(N));
        }

    void start()
        {
            SOCE_DEBUG << _D("--------------- sample_dispatch_queue ---------------");

            for (int i=0; i<N; ++i){
                producers_[i] = std::thread(&ExampleDispatchQueue::producer_thread, this, i);
                consumers_[i] = std::thread(&ExampleDispatchQueue::consumer_thread, this, i);
            }

            for (int i=0; i<N; ++i){
                producers_[i].join();
                consumers_[i].join();
            }
        }

private:
    int consumer_selector(const int& val, size_t consumer_size)
        {
            (void) consumer_size;

            /*
             * even number to consumer 0
             * odd number to consumer 1
             */
            return (val % 2) ? 0 : 1;
        }

    void producer_thread(int consumer_id)
        {
            for (size_t i=0; i<data_size_; ++i){
                dq_->produce(2 * i + consumer_id);
            }
        }

    void consumer_thread(int consumer_id)
        {
            size_t total = 0;
            int fd = dq_->get_consumer_fd(consumer_id);
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 10000;

            while (1){
                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(fd, &readfds);
                select(fd + 1, &readfds, NULL, NULL, &tv);

                FQVector<int> data;
                if (!dq_->try_consume(consumer_id, data)){
                    total += data.size();
                    count_ += data.size();
                }
                usleep(10000);
                if (count_ >= data_size_ * N){
                    break;
                }
            }

            SOCE_DEBUG << _S("consumer_id", consumer_id) << _S("data size", total);
        }

private:
    std::shared_ptr<DispatchQueue<int>> dq_;
    std::thread producers_[N];
    std::thread consumers_[N];
    const size_t data_size_ = 1000;
    std::atomic<size_t> count_{0};
};

void dispatch_queue()
{
    ExampleDispatchQueue edq;
    edq.start();
}
