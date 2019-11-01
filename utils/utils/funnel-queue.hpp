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

#ifndef _SOCE_UTILS_FUNNEL_QUEUE_iSLR852hnK_H_
#define _SOCE_UTILS_FUNNEL_QUEUE_iSLR852hnK_H_

#include <vector>
#include "double-cache-queue.h"

namespace soce {
namespace utils {

    template<class T>
    class FQVectorIterator : public std::iterator<std::forward_iterator_tag, T>
    {
    public:
        FQVectorIterator(std::shared_ptr<std::vector<T>> elems, size_t pos)
            : elems_(elems), pos_(pos)
            {
            }

        FQVectorIterator(const FQVectorIterator& rhs)
            {
                elems_ = rhs.elems_;
                pos_ = rhs.pos_;
            }

        FQVectorIterator& operator=(const FQVectorIterator& rhs)
            {
                if (this == &rhs){
                    return *this;
                }

                elems_ = rhs.elems_;
                pos_ = rhs.pos_;
                return *this;
            }

        FQVectorIterator& operator++()
            {
                ++pos_;
                return *this;
            }

        FQVectorIterator operator++(int)
            {
                FQVectorIterator tmp(elems_, pos_);
                ++pos_;
                return tmp;
            }

        bool operator==(const FQVectorIterator& rhs)
            {
                return (elems_ == rhs.elems_ && pos_ == rhs.pos_) ? true : false;
            }

        bool operator!=(const FQVectorIterator& rhs)
            {
                return !operator==(rhs);
            }

        T& operator*()
            {
                return (*elems_)[pos_];
            }

    protected:
        std::shared_ptr<std::vector<T>> elems_;
        size_t pos_ = 0;
    };

    template <class T>
    class FQVector
    {
    public:
        typedef FQVectorIterator<T> iterator;

    public:
        FQVector(){}
        FQVector(std::shared_ptr<std::vector<T>> elems, size_t size)
            : elems_(elems), size_(size)
            {
            }
        ~FQVector() = default;


        inline bool empty() {return size_ == 0 ? true : false;}
        inline size_t size() {return size_;}
        iterator begin()
            {
                return FQVectorIterator<T>(elems_, 0);
            }

        iterator end()
            {
                return FQVectorIterator<T>(elems_, size_);
            }

    private:
        std::shared_ptr<std::vector<T>> elems_;
        size_t size_ = 0;
    };

    template <class T>
    class FunnelQueue : public DoubleCacheQueue
    {
    public:
        FunnelQueue()
            : DoubleCacheQueue(kStrategyYielding, 10000, 0)
            {
                init();
            }

        FunnelQueue(DoubleCacheQueue::WaitStrategy strategy, size_t elem_size)
            : DoubleCacheQueue(strategy, elem_size, 0)
            {
                init();
            }

        ~FunnelQueue()
            {
            }

        int init()
            {
                for (int i=0; i<2; ++i){
                    buffers_[i] = std::make_shared<std::vector<T>>(elem_size_);
                }

                return evtfd_mail_.good() ? 0 : -1;
            }

        void produce(const T& e)
            {
                add(1, [this, &e](size_t buff_index, size_t cursor){
                        (*buffers_[buff_index])[cursor] = e;
                    });
            }

        void produce(T&& e)
            {
                add(1, [this, &e](size_t buff_index, size_t cursor){
                        (*buffers_[buff_index])[cursor] = std::move(e);
                    });
            }

        template <class... Args>
        void produce(Args&&... args)
            {
                T e(std::forward<Args>(args)...);
                add(1, [this, &e](size_t buff_index, size_t cursor){
                        (*buffers_[buff_index])[cursor] = std::move(e);
                    });
            }

        FQVector<T> consume()
            {
                FQVector<T> fqv;
                DoubleCacheQueue::consume([this, &fqv](size_t buff_index, size_t cursor){
                        fqv = FQVector<T>(buffers_[buff_index], cursor);
                    });
                return fqv;
            }

        FQVector<T> try_consume()
            {
                FQVector<T> fqv;
                DoubleCacheQueue::try_consume([this, &fqv](size_t buff_index, size_t cursor){
                        fqv = FQVector<T>(buffers_[buff_index], cursor);
                    });
                return fqv;
            }

        FQVector<T> try_consume_for(size_t timeout_ms)
            {
                FQVector<T> fqv;
                DoubleCacheQueue::try_consume_for([this, &fqv](size_t buff_index, size_t cursor){
                        fqv = FQVector<T>(buffers_[buff_index], cursor);
                    }, timeout_ms);
                return fqv;
            }

    private:
        using ElemBuffer = std::shared_ptr<std::vector<T>>;

    private:
        ElemBuffer buffers_[2];
    };

} // namespace utils
} // namespace soce
#endif
