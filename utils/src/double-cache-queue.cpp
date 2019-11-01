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
#include "utils/double-cache-queue.h"

namespace soce {
namespace utils {

    DoubleCacheQueue::DoubleCacheQueue()
    {
    }

    DoubleCacheQueue::DoubleCacheQueue(DoubleCacheQueue::WaitStrategy strategy,
                                       size_t elem_size,
                                       size_t water_mark)
        :strategy_(strategy), elem_size_(elem_size), water_mark_(water_mark)
    {
    }

    void DoubleCacheQueue::add(size_t elem_size, Adder adder)
    {
        bool succ = false;
        while (1) {
            lock_.read_lock();

            size_t cursor = cursor_;
            size_t next = cursor + elem_size;

            if (next >= elem_size_) {
                wait();
                continue;
            }else {
                succ = cursor_.compare_exchange_weak(cursor, next);
                if (succ){
                    adder(buff_index_, cursor);
                }
            }

            if (succ) {
                if (next > water_mark_) {
                    evtfd_mail_.notify();
                }
                lock_.read_unlock();
                return;
            }
            lock_.read_unlock();
        }
    }

    void DoubleCacheQueue::consume(Consumer consumer)
    {
        struct timeval* ptv = NULL;
        try_consume_for(consumer, ptv);
    }

    void DoubleCacheQueue::try_consume(Consumer consumer)
    {
        lock_.write_lock();
        size_t buff_index = buff_index_;
        size_t cursor = cursor_;
        buff_index_ = (buff_index_ + 1) % 2;
        cursor_ = 0;
        wakeup();
        lock_.write_unlock();

        consumer(buff_index, cursor);
    }

    void DoubleCacheQueue::try_consume_for(Consumer consumer, uint64_t timeout_ms)
    {
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        try_consume_for(consumer, &tv);
    }

    void DoubleCacheQueue::wait()
    {
        if (strategy_ == kStrategyYielding) {
            lock_.read_unlock();
            std::this_thread::yield();
        }
        else if (strategy_ == kStrategyBlocking) {
            std::unique_lock<std::mutex> lck(wait_mtx_);
            lock_.read_unlock();
            wait_cv_.wait(lck);
        }
    }

    void DoubleCacheQueue::wakeup()
    {
        evtfd_mail_.clear();

        if (strategy_ == kStrategyBlocking) {
            std::unique_lock<std::mutex> lck(wait_mtx_);
            wait_cv_.notify_all();
        }
    }

    void DoubleCacheQueue::try_consume_for(Consumer consumer, struct timeval* tv)
    {
        if (!evtfd_mail_.is_notified()){
            int fd = evtfd_mail_.get_fd();
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(fd, &readfds);
            select(fd + 1, &readfds, NULL, NULL, tv);
        }
        try_consume(consumer);
    }

} // namespace utisl
} // namespace soce
