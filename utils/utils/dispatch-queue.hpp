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

#ifndef _SOCE_DISPATCH_QUEUE_hczymcOWI3_H_
#define _SOCE_DISPATCH_QUEUE_hczymcOWI3_H_

#include <functional>
#include "funnel-queue.hpp"

namespace soce{
namespace utils{

    template <class T>
    class DispatchQueue
    {
    public:
        using Selector = std::function<int(const T&, size_t)>;

    public:
        DispatchQueue()
            : DispatchQueue(1, Selector())
            {
            }

        DispatchQueue(Selector selector)
            : DispatchQueue(1, selector)
            {
            }

        DispatchQueue(size_t consumers)
            : DispatchQueue(consumers, Selector())
            {
            }

        DispatchQueue(size_t consumers, Selector selector)
            {
                (void)init(consumers, selector);
            }

        bool good()
            {
                if (consumers_.empty()){
                    return false;
                }

                for (auto& i : consumers_){
                    if (!i->good()){
                        return false;
                    }
                }
                return true;
            }

        void produce(const T& e)
            {
                int cid = get_cid(e);
                consumers_[cid]->produce(e);
            }

        void produce(T&& val)
            {
                int cid = get_cid(val);
                consumers_[cid]->produce(std::move(val));
            }

        template <class... Args>
        void produce(Args&&... args)
            {
                T e(std::forward<Args>(args)...);
                int cid = get_cid(e);
                consumers_[cid]->produce(std::move(e));
            }

        int consume(size_t cid, FQVector<T>& out)
            {
                if (cid > consumers_.size()){
                    return -1;
                }

                out = consumers_[cid]->consume();
                return 0;
            }

        int try_consume(size_t cid, FQVector<T>& out)
            {
                if (cid > consumers_.size()){
                    return -1;
                }

                out = consumers_[cid]->try_consume();
                return 0;
            }

        int try_consume_for(size_t cid, FQVector<T>& out, uint64_t timeout_ms)
            {
                if (cid > consumers_.size()){
                    return -1;
                }

                out = consumers_[cid]->try_consume_for(timeout_ms);
                return 0;
            }

        int get_consumer_fd(size_t cid)
            {
                if (cid > consumers_.size()){
                    return -1;
                }

                return consumers_[cid]->get_notify_fd();
            }

    private:
        int get_cid(const T& e)
            {
                auto size = consumers_.size();
                auto cid = consumer_selector_(e, size);

                if (cid < 0 || (size_t)cid >= size){
                    size_t index = 0;
                    while (1){
                        index = consumer_index_;
                        if (consumer_index_.compare_exchange_weak(index, index + 1)){
                            break;
                        }
                    }

                    cid = index % size;
                }

                return cid;
            }

        int init(size_t consumers, Selector selector)
            {
                consumers_.resize(consumers);
                for (size_t i=0; i<consumers; ++i) {
                    consumers_[i].reset(new FunnelQueue<T>());
                }

                if (selector) {
                    consumer_selector_ = selector;
                }

                return 0;
            }

    private:
        std::atomic<size_t> consumer_index_{0};
        Selector consumer_selector_ = [](const T&, size_t){return -1;};
        std::vector<std::shared_ptr<FunnelQueue<T>>> consumers_;
    };

} // namespace utils
} // namespace soce

#endif
