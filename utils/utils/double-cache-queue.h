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

#ifndef _SOCE_UTILS_DOUBLE_CACHE_QUEUE_wBmc25UfQv_H_
#define _SOCE_UTILS_DOUBLE_CACHE_QUEUE_wBmc25UfQv_H_

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string.h>
#include <functional>
#include "rwlock.hpp"
#include "evtfd-mail.h"

namespace soce {
namespace utils {

    class DoubleCacheQueue
    {
    public:
        using WaitStrategy = enum {
            kStrategyBlocking,
            kStrategyYielding
        };

        using Adder = std::function<void(size_t buff_index, size_t cursor)>;
        using Consumer = std::function<void(size_t buff_index, size_t cursor)>;

    public:
        DoubleCacheQueue();
        DoubleCacheQueue(DoubleCacheQueue::WaitStrategy strategy,
                         size_t buff_size,
                         size_t water_mark);
        void add(size_t elem_size, Adder adder);
        void consume(Consumer consumer);
        void try_consume(Consumer consumer);
        void try_consume_for(Consumer consumer, uint64_t timeout_ms);
        inline int get_notify_fd() {
            return evtfd_mail_.get_fd();
        }
        inline bool good() {
            return evtfd_mail_.good();
        }

    protected:
        void wait();
        void wakeup();
        void try_consume_for(Consumer consumer, struct timeval* tv);

    protected:
        WaitStrategy strategy_ = kStrategyYielding;
        size_t buff_index_ = 0;
        size_t elem_size_ = 10240;
        size_t water_mark_ = 0;
        std::atomic<size_t> cursor_{0};
        RWLock lock_;
        std::mutex wait_mtx_;
        std::condition_variable wait_cv_;
        EvtfdMail evtfd_mail_;
    };

} // namespace utisl
} // namespace soce

#endif
