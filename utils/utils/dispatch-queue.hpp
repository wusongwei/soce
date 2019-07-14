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

#include <mutex>
#include <sys/eventfd.h>
#include <unistd.h>
#include <vector>
#include <list>
#include <memory>
#include <atomic>
#include <functional>

namespace soce{
namespace utils{

    class EvtfdMail
    {
    public:
        EvtfdMail()
            {
                evtfd_ = eventfd(0, 0);
            }

        ~EvtfdMail()
            {
                if (evtfd_ > 0){
                    close(evtfd_);
                }
            }

        EvtfdMail(const EvtfdMail&) = delete;
        EvtfdMail& operator=(const EvtfdMail&) = delete;

        int get_fd()
            {
                return evtfd_;
            }

        bool good()
            {
                return (evtfd_ > 0) ? true : false;
            }

        int notify()
            {
                bool n = false;
                if (!notified_.compare_exchange_strong(n, true)){
                    return 0;
                }

                uint64_t val = 1;
                if (write(evtfd_, &val, sizeof(uint64_t)) != sizeof(uint64_t)){
                    return -1;
                }

                return 0;
            }

        int clear()
            {
                if (!notified_){
                    return 0;
                }

                uint64_t val = 0;
                if (read(evtfd_, (void*)&val, sizeof(uint64_t)) != sizeof(uint64_t)){
                    return -1;
                }

                notified_ = false;
                return 0;
            }

    private:
        int evtfd_ = -1;
        std::atomic<bool> notified_{false};
    };

    template<class T>
    class DQVectorIterator : public std::iterator<std::forward_iterator_tag, T>
    {
    public:
        DQVectorIterator(typename std::vector<std::vector<T>>::iterator outer_iter,
                         typename std::vector<std::vector<T>>::iterator outer_end,
                         typename std::vector<T>::iterator inner_iter,
                         bool begin)
            : outer_iter_(outer_iter),
              outer_end_(outer_end),
              inner_iter_(inner_iter)
            {
                if (outer_iter_ != outer_end_){
                    if (begin){
                        inner_iter_ = outer_iter_->begin();
                    }
                    else{
                        outer_iter_ = outer_end_;
                        auto end = outer_end_;
                        --end;
                        inner_iter_ = end->end();
                    }
                }
            }

        DQVectorIterator(const DQVectorIterator& rhs)
            {
                outer_iter_ = rhs.outer_iter_;
                outer_end_ = rhs.outer_end_;
                inner_iter_ = rhs.inner_iter_;
            }

        DQVectorIterator& operator=(const DQVectorIterator& rhs)
            {
                if (this == &rhs){
                    return *this;
                }

                outer_iter_ = rhs.outer_iter_;
                outer_end_ = rhs.outer_end_;
                inner_iter_ = rhs.inner_iter_;
                return *this;
            }

        DQVectorIterator& operator++()
            {
                if (inner_iter_ == outer_iter_->end()){
                    return *this;
                }

                ++inner_iter_;
                if (inner_iter_ == outer_iter_->end()){
                    if (outer_iter_ != outer_end_){
                        ++outer_iter_;
                        if (outer_iter_ != outer_end_){
                            inner_iter_ = outer_iter_->begin();
                        }
                    }
                }

                return *this;
            }

        DQVectorIterator operator++(int)
            {
                DQVectorIterator tmp(outer_iter_, outer_end_, inner_iter_);
                operator++();
                return tmp;
            }

        bool operator==(const DQVectorIterator& rhs)
            {
                return (outer_iter_ == rhs.outer_iter_
                        && outer_end_ == rhs.outer_end_
                        && inner_iter_ == rhs.inner_iter_) ? true : false;
            }

        bool operator!=(const DQVectorIterator& rhs)
            {
                return !operator==(rhs);
            }

        T& operator*()
            {
                return *inner_iter_;
            }

    protected:
        typename std::vector<std::vector<T>>::iterator outer_iter_;
        typename std::vector<std::vector<T>>::iterator outer_end_;
        typename std::vector<T>::iterator inner_iter_;
    };

    template <class T>
    class DQVector
    {
    public:
        typedef DQVectorIterator<T> iterator;

    public:
        DQVector() = default;
        ~DQVector() = default;

        inline bool empty() {return data_.empty();}
        inline size_t size() {return size_;}
        inline void clear()
            {
                data_.clear();
                size_ = 0;
                end_ = DQVectorIterator<T>(data_.begin(), data_.end(), dummy_, true);
            }

        void swap(std::vector<T>& rhs)
            {
                if (!rhs.empty()){
                    size_ += rhs.size();
                    data_.push_back(std::vector<T>());
                    data_.back().swap(rhs);

                    end_ = DQVectorIterator<T>(data_.begin(), data_.end(), dummy_, false);
                }
            }

        iterator begin()
            {
                return DQVectorIterator<T>(data_.begin(), data_.end(), dummy_, true);
            }

        iterator end()
            {
                return end_;
            }

    private:
        typename std::vector<std::vector<T>> data_;
        size_t size_ = 0;
        typename std::vector<T>::iterator dummy_;
        iterator end_{data_.begin(), data_.end(), dummy_, true};
    };

    template <class T>
    class DispatchQueue
    {
    public:
        template <class X>
        struct ConsumerBuffer{
            std::mutex mtx;
            std::vector<X> buffer;
        };

        template <class Y>
        struct Consumer{
            std::atomic<size_t> prod_index{0};
            std::atomic<bool> empty{true};
            EvtfdMail mail;
            std::vector<std::shared_ptr<ConsumerBuffer<Y>>> csum_buffer;
        };

        using Selector = std::function<int(const T&, size_t)>;

    public:
        DispatchQueue(Selector selector = [](const T&, size_t){return -1;})
            {
                (void)init(1, 1);
                consumer_selector_ = selector;
            }

        DispatchQueue(size_t producers,
                   size_t consumers,
                   Selector selector = [](const T&, size_t){return -1;})
            {
                (void)init(producers, consumers);
                consumer_selector_ = selector;
            }

        bool good()
            {
                if (consumers_.empty()){
                    return false;
                }

                for (auto& i : consumers_){
                    if (!i->mail.good()){
                        return false;
                    }
                }
                return true;
            }

        bool empty(size_t cid)
            {
                return (cid >= consumers_.size()) ? true : consumers_[cid]->empty.load();
            }

        void produce(size_t pid, const T& val)
            {
                auto size = consumers_.size();
                auto cid = consumer_selector_(val, size);

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
                std::shared_ptr<Consumer<T>>& consumer = consumers_[cid];

                auto csum_buff_size = consumer->csum_buffer.size();
                if (pid >= csum_buff_size){
                    size_t pindex = 0;
                    while (1){
                        pindex = consumer->prod_index;
                        if (consumer->prod_index.compare_exchange_weak(pindex, pindex + 1)){
                            break;
                        }
                    }

                    pid = pindex % csum_buff_size;
                }

                std::lock_guard<std::mutex> lck(consumer->csum_buffer[pid]->mtx);
                consumer->csum_buffer[pid]->buffer.push_back(val);
                consumer->empty.store(false);
                consumer->mail.notify();
            }

        void produce(size_t pid, T&& val)
            {
                auto size = consumers_.size();
                auto cid = consumer_selector_(val, size);

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
                std::shared_ptr<Consumer<T>>& consumer = consumers_[cid];

                auto csum_buff_size = consumer->csum_buffer.size();
                if (pid >= csum_buff_size){
                    size_t pindex = 0;
                    while (1){
                        pindex = consumer->prod_index;
                        if (consumer->prod_index.compare_exchange_weak(pindex, pindex + 1)){
                            break;
                        }
                    }

                    pid = pindex % csum_buff_size;
                }

                std::lock_guard<std::mutex> lck(consumer->csum_buffer[pid]->mtx);
                consumer->csum_buffer[pid]->buffer.push_back(std::move(val));
                consumer->empty.store(false);
                consumer->mail.notify();
            }

        template <class... Args>
        void produce(size_t pid, Args&&... args)
            {
                T val(std::forward<Args>(args)...);
                auto size = consumers_.size();
                auto cid = consumer_selector_(val, size);

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
                std::shared_ptr<Consumer<T>>& consumer = consumers_[cid];

                auto csum_buff_size = consumer->csum_buffer.size();
                if (pid >= csum_buff_size){
                    size_t pindex = 0;
                    while (1){
                        pindex = consumer->prod_index;
                        if (consumer->prod_index.compare_exchange_weak(pindex, pindex + 1)){
                            break;
                        }
                    }

                    pid = pindex % csum_buff_size;
                }

                std::lock_guard<std::mutex> lck(consumer->csum_buffer[pid]->mtx);
                consumer->csum_buffer[pid]->buffer.push_back(std::move(val));
                consumer->empty.store(false);
                consumer->mail.notify();
            }

        int consume(size_t cid, DQVector<T>& out)
            {
                if (cid > consumers_.size()){
                    return -1;
                }
                int fd = consumers_[cid]->mail.get_fd();

                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(fd, &readfds);
                int rc = select(fd + 1, &readfds, NULL, NULL, NULL);
                if (rc > 0) {
                    out.clear();
                    consumers_[cid]->empty.store(true);
                    consumers_[cid]->mail.clear();

                    for (auto& i : consumers_[cid]->csum_buffer){
                        std::lock_guard<std::mutex> lck(i->mtx);
                        out.swap(i->buffer);
                    }
                    rc = 0;
                }
                else{
                    rc = -1;
                }

                return rc;
            }

        int try_consume_for(size_t cid, DQVector<T>& out, uint64_t timeout_ms)
            {
                if (cid > consumers_.size()){
                    return -1;
                }

                struct timeval tv;
                tv.tv_sec = timeout_ms / 1000;
                tv.tv_usec = (timeout_ms % 1000) * 1000;

                int fd = consumers_[cid]->mail.get_fd();
                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(fd, &readfds);
                int rc = select(fd + 1, &readfds, NULL, NULL, &tv);
                if (rc > 0) {
                    out.clear();
                    consumers_[cid]->empty.store(true);
                    consumers_[cid]->mail.clear();

                    for (auto& i : consumers_[cid]->csum_buffer){
                        std::lock_guard<std::mutex> lck(i->mtx);
                        out.swap(i->buffer);
                    }
                    rc = 0;
                }
                else if(rc == 0){
                    rc = 1;
                }
                else{
                    rc = -1;
                }

                return rc;
            }

        int try_consume(size_t cid, DQVector<T>& out)
            {
                return try_consume_for(cid, out, 0);
            }

        int get_consumer_fd(size_t cid)
            {
                if (cid > consumers_.size()){
                    return -1;
                }

                return consumers_[cid]->mail.get_fd();
            }

        void rebalance(size_t cid)
            {
                if (cid > consumers_.size()){
                    return;
                }

                static size_t start = 0;
                ++start;

                size_t num = consumers_.size();
                for (size_t j=0; j<num; ++j){
                    size_t i = (j + start) % num;
                    if (i == cid || consumers_[i]->empty){
                        continue;
                    }

                    for (auto& buff_out : consumers_[i]->csum_buffer){
                        std::lock_guard<std::mutex> lck(buff_out->mtx);
                        if (!buff_out->buffer.empty()){
                            auto& buff_in = consumers_[cid]->csum_buffer[0];
                            buff_in->mtx.lock();
                            buff_in->buffer.push_back(std::move(buff_out->buffer.back()));
                            buff_in->mtx.unlock();
                            consumers_[cid]->empty.store(false);
                            consumers_[cid]->mail.notify();
                            buff_out->buffer.pop_back();
                            break;
                        }
                    }
                    break;
                }
            }

    private:
        int init(size_t producers, size_t consumers)
            {
                consumers_.clear();

                for (size_t i = 0; i<consumers; ++i){
                    std::shared_ptr<Consumer<T>> c(new Consumer<T>());
                    if (!c->mail.good()){
                        return -1;
                    }

                    for (size_t i=0; i<producers; ++i){
                        std::shared_ptr<ConsumerBuffer<T>> b(new ConsumerBuffer<T>());
                        c->csum_buffer.push_back(b);
                    }

                    consumers_.push_back(c);
                }

                return 0;
            }

    private:
        std::atomic<size_t> consumer_index_{0};
        Selector consumer_selector_;
        std::vector<std::shared_ptr<Consumer<T>>> consumers_;
    };

    template <class T>
    class DispatchQueue_11
    {
    public:
        DispatchQueue_11()
            {
                dispatch_queue_.reset(new DispatchQueue<T>());
            }

        bool good()
            {
                return dispatch_queue_->good();
            }

        void produce(const T& val)
            {
                dispatch_queue_->produce(0, val);
            }

        void produce(T&& val)
            {
                dispatch_queue_->produce(0, std::move(val));
            }

        template <class... Args>
        void produce(Args&&... args)
            {
                dispatch_queue_->produce(0, std::forward<Args>(args)...);
            }

        int consume(DQVector<T>& out)
            {
                return dispatch_queue_->consume(0, out);
            }

        int try_consume_for(DQVector<T>& out, uint64_t timeout_ms)
            {
                return dispatch_queue_->try_consume_for(0, out, timeout_ms);
            }

        int try_consume(DQVector<T>& out)
            {
                return dispatch_queue_->try_consume(0, out);
            }

        int get_consumer_fd()
            {
                return dispatch_queue_->get_consumer_fd(0);
            }

        bool empty()
            {
                return dispatch_queue_->empty(0);
            }

    private:
        std::shared_ptr<DispatchQueue<T>> dispatch_queue_;
    };

    template <class T>
    class DispatchQueue_1n
    {
    public:
        using Selector = std::function<int(const T&, size_t)>;

    public:
        DispatchQueue_1n(size_t consumers, Selector selector = [](const T&, size_t){return -1;})
            {
                dispatch_queue_.reset(new DispatchQueue<T>(1, consumers, selector));
            }

        bool good()
            {
                return dispatch_queue_->good();
            }

        void produce(const T& val)
            {
                dispatch_queue_->produce(0, val);
            }

        void produce(T&& val)
            {
                dispatch_queue_->produce(0, std::move(val));
            }

        template <class... Args>
        void produce(Args&&... args)
            {
                dispatch_queue_->produce(0, std::forward<Args>(args)...);
            }
        int consume(size_t cid, DQVector<T>& out)
            {
                return dispatch_queue_->consume(cid, out);
            }

        int try_consume_for(size_t cid, DQVector<T>& out, uint64_t timeout_ms)
            {
                return dispatch_queue_->try_consume_for(cid, out, timeout_ms);
            }

        int try_consume(size_t cid, DQVector<T>& out)
            {
                return dispatch_queue_->try_consume(cid, out);
            }

        int get_consumer_fd(size_t cid)
            {
                return dispatch_queue_->get_consumer_fd(cid);
            }

        bool empty(size_t cid)
            {
                return dispatch_queue_->empty(cid);
            }

        void rebalance(size_t cid)
            {
                dispatch_queue_->rebalance(cid);
            }

    private:
        std::shared_ptr<DispatchQueue<T>> dispatch_queue_;
    };

    template <class T>
    class DispatchQueue_n1
    {
    public:
        DispatchQueue_n1(size_t producers)
            {
                dispatch_queue_.reset(new DispatchQueue<T>(producers, 1));
            }

        bool good()
            {
                return dispatch_queue_->good();
            }

        void produce(size_t pid, const T& val)
            {
                dispatch_queue_->produce(pid, val);
            }

        void produce(size_t pid, T&& val)
            {
                dispatch_queue_->produce(pid, std::move(val));
            }

        template <class... Args>
        void produce(size_t pid, Args&&... args)
            {
                dispatch_queue_->produce(pid, std::forward<Args>(args)...);
            }

        int consume(DQVector<T>& out)
            {
                return dispatch_queue_->consume(0, out);
            }

        int try_consume_for(DQVector<T>& out, uint64_t timeout_ms)
            {
                return dispatch_queue_->try_consume_for(0, out, timeout_ms);
            }

        int try_consume(DQVector<T>& out)
            {
                return dispatch_queue_->try_consume(0, out);
            }

        int get_consumer_fd()
            {
                return dispatch_queue_->get_consumer_fd(0);
            }

        bool empty()
            {
                return dispatch_queue_->empty(0);
            }

    private:
        std::shared_ptr<DispatchQueue<T>> dispatch_queue_;
    };

} // namespace utils
} // namespace soce
#endif
