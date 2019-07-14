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

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/types.h>
#include <chrono>
#include "log4rel/thread-sink.h"

using std::lock_guard;
using std::mutex;
using std::pair;
using std::list;
using std::string;
using std::thread;

namespace soce{
namespace log4rel{

    ThreadSink::ThreadSink()
    {
        evtfd_ = eventfd(0, 0); // failed ?
        append_buffer_.reset(new Buffer);
    }

    void ThreadSink::log(const struct timeval& tv, LogLevel level, const string& msg)
    {
        append_buffer_->append(tv, level, msg);
        if (append_buffer_->get_log_size() > watermark_){
            notify();
        }
    }

    void ThreadSink::notify()
    {
        std::lock_guard<std::mutex> lck(mtx_);

        uint64_t u = 1;
        if (!notified_){
            (void)write(evtfd_, &u, sizeof(uint64_t));
            notified_ = true;
        }
    }

    void ThreadSink::flush(list<LogDetail>& out)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        if (notified_){
            uint64_t u = 0;
            size_t size = read(evtfd_, &u, sizeof(uint64_t));
            if (size != sizeof(uint64_t)){
                return;
            }
            notified_ = false;
        }

        append_buffer_->flush(out);
    }

    MultiThreadSink::MultiThreadSink()
    {
        epoll_fd_ = epoll_create(epoll_size_);
    }

    MultiThreadSink::~MultiThreadSink()
    {
        run_.store(false);

        if (!sinks_.empty()){
            (*sinks_.begin())->notify();
        }

        if (thread_.joinable()){
            thread_.join();
        }
    }

    std::shared_ptr<ThreadSink> MultiThreadSink::create_thread_sink()
    {
        std::shared_ptr<ThreadSink> sink;
        ThreadSink* psink = new ThreadSink;
        if (psink->get_evtfd() == -1){
            return sink;
        }

        sink.reset(psink);
        lock_guard<mutex> lck(mtx_);
        sinks_.insert(sink);
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sink->get_evtfd(), &ev);
        return sink;
    }

    void MultiThreadSink::erase_sink(std::shared_ptr<ThreadSink> sink)
    {
        lock_guard<mutex> lck(mtx_);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, sink->get_evtfd(), &ev);
        sinks_.erase(sink);
    }

    void MultiThreadSink::server()
    {
        thread_ = thread(&MultiThreadSink::thread_entry, this);
        while (!run_);
    }

    void MultiThreadSink::thread_entry()
    {
        run_.store(true);

        struct epoll_event events[epoll_size_];
        list<LogDetail> out;

        while (run_){
            int nfds = epoll_wait(epoll_fd_, events, epoll_size_, timeout_);
            if (nfds >= 0){
                out.clear();
                for (auto& i : sinks_){
                    mtx_.lock();
                    i->flush(out);
                    mtx_.unlock();
                }

                flush(out);
            }
        }

        out.clear();
        for (auto& i : sinks_){
            mtx_.lock();
            i->flush(out);
            mtx_.unlock();
        }

        flush(out);
    }

    void MultiThreadSink::flush(list<LogDetail>& logs)
    {
        static size_t i  = 0;
        i += logs.size();
        if (isSort_){
            logs.sort();
        }

        for (auto& i : logs){
            sink_->log((const struct timeval&)i.time, i.level, i.msg);
        }
    }

} // namespace soce
} //namespace log4rel
