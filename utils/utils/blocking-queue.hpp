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

#ifndef _SOCE_UTILS_BLOCKING_QUEUE_urdrqOTFDf_H_
#define _SOCE_UTILS_BLOCKING_QUEUE_urdrqOTFDf_H_

#include <mutex>
#include <queue>
#include <condition_variable>
#include "time-helper.h"

namespace soce{
namespace utils{

    template <typename T>
    class BlockingQueue {
    public:
        BlockingQueue()
            {
            }

        BlockingQueue(size_t max_size)
            :max_size_(max_size)
            {
            }

        BlockingQueue(const BlockingQueue &) = delete;
        BlockingQueue& operator=(const BlockingQueue &) = delete;

        void push(const T& val)
            {
                {
                    std::unique_lock<std::mutex> lck(mtx_);
                    while (queue_.size() >= max_size_){
                        cv_producer_.wait(lck);
                    }
                    queue_.push(val);
                }

                cv_consumer_.notify_one();
            }

        int push(const T& val, size_t tmout_ms)
            {
                {
                    std::unique_lock<std::mutex> lck(mtx_);
                    uint64_t start_time = TimeHelper::get_time_ms();

                    while (queue_.size() >= max_size_){
                        uint64_t cur_time = TimeHelper::get_time_ms();
                        if (start_time + tmout_ms < cur_time){
                            return -1;
                        }
                        uint64_t time_left = tmout_ms + start_time - cur_time;
                        cv_producer_.wait_for(lck, std::chrono::milliseconds(time_left));
                    }
                    queue_.push(val);
                }

                cv_consumer_.notify_one();
                return 0;
            }

        void pop(T& val)
            {
                {
                    std::unique_lock<std::mutex> lck(mtx_);
                    while (queue_.empty()) {
                        cv_consumer_.wait(lck, [this]{ return !queue_.empty(); });
                    }

                    val = std::move(queue_.front());
                    queue_.pop();
                }

                cv_producer_.notify_one();
            }

        int pop(T& val, size_t tmout_ms)
            {
                {
                    std::unique_lock<std::mutex> lck(mtx_);
                    uint64_t start_time = TimeHelper::get_time_ms();

                    while (queue_.empty()) {
                        uint64_t cur_time = TimeHelper::get_time_ms();
                        if (start_time + tmout_ms < cur_time){
                            return -1;
                        }
                        uint64_t time_left = tmout_ms + start_time - cur_time;
                        cv_consumer_.wait_for(lck,
                                              std::chrono::milliseconds(time_left),
                                              [this]{ return !queue_.empty(); });
                    }

                    val = std::move(queue_.front());
                    queue_.pop();
                }

                cv_producer_.notify_one();
                return 0;
            }

        size_t size() const
            {
                std::unique_lock<std::mutex> lck(mtx_);
                return queue_.size();
            }

    private:
        mutable std::mutex mtx_;
        std::condition_variable cv_consumer_;
        std::condition_variable cv_producer_;
        std::queue<T> queue_;
        size_t max_size_ = -1;
    };

} // namespace utils
} // namespace soce
#endif
