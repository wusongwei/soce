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
#include "log4rel/logger.h"
#include "utils/dispatch-queue.hpp"

using namespace soce::utils;

class ExampleDispatchQueue
{
public:
    ExampleDispatchQueue()
        {
            // 2 producers and 2 consumers
            dq_.reset(new DispatchQueue<int>(2, 2, std::bind(&ExampleDispatchQueue::consumer_selector,
                                                             this,
                                                             std::placeholders::_1,
                                                             std::placeholders::_2)));
        }

    void start()
        {
            SOCE_DEBUG << _D("--------------- sample_dispatch_queue ---------------");

            // try_consume_for
            SOCE_DEBUG << _D("before try_consumer_for, wait for at most 1sec");
            DQVector<int> data;
            dq_->try_consume_for(0, data, 1000);
            SOCE_DEBUG << _D("after try_consumer_for");

            // try_consume
            t1_ = std::thread(&ExampleDispatchQueue::consumer_thread, this, 0);
            t2_ = std::thread(&ExampleDispatchQueue::consumer_thread, this, 1);

            for (size_t i=0; i<data_size_; ++i){
                dq_->produce(0, i);
                dq_->produce(1, i + 10);
            }

            t1_.join();
            t2_.join();
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

    void consumer_thread(int consumer_id)
        {
            size_t total = 0;
            while (1){
                DQVector<int> data;
                if (!dq_->try_consume(consumer_id, data)){
                    total += data.size();
                    count_ += data.size();
                }

                if (count_ >= data_size_ * 2){
                    break;
                }
            }

            SOCE_DEBUG << _S("consumer_id", consumer_id) << _S("data size", total);
        }

private:
    std::shared_ptr<DispatchQueue<int>> dq_;
    std::thread t1_;
    std::thread t2_;
    const size_t data_size_ = 5;
    std::atomic<size_t> count_{0};
};

void dispatch_queue()
{
    ExampleDispatchQueue sdq;
    sdq.start();
}
