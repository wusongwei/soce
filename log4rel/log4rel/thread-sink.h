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

#ifndef _THREAD_SINK_z4fIgrnGIR_H_
#define _THREAD_SINK_z4fIgrnGIR_H_

#include <mutex>
#include <list>
#include <unordered_set>
#include <string>
#include <utility>
#include <memory>
#include <thread>
#include <atomic>
#include "log4rel/sink-if.h"
#include "log4rel/type.h"

namespace soce{
namespace log4rel{

    typedef struct LogDetail
    {
    LogDetail(struct timeval _time, LogLevel _level, const std::string& _msg)
        : time(_time), level(_level), msg(_msg)
            {
            }
        bool operator< (const LogDetail& other) {
                if (this->time.tv_sec < other.time.tv_sec){
                    return true;
                }
                else if (this->time.tv_sec == other.time.tv_sec){
                    return this->time.tv_usec < other.time.tv_usec;
                }

                return false;
            }

        struct timeval time;
        LogLevel level;
        std::string msg;
    }LogDetail;

    class Buffer
    {
    public:
        inline void lock() {mtx_.lock();}
        inline void unlock() {mtx_.unlock();}
        inline void append(const struct timeval& tv, LogLevel level, const std::string& msg) {
            std::lock_guard<std::mutex> lck(mtx_);
            msgs_.emplace_back(tv, level, msg);
        }

        inline void flush(std::list<LogDetail>& out) {
            std::lock_guard<std::mutex> lck(mtx_);
            if (!msgs_.empty()) {
                out.splice(out.end(), msgs_, msgs_.begin(), msgs_.end());
            }
        }

        inline uint32_t get_log_size() {
            std::lock_guard<std::mutex> lck(mtx_);
            return msgs_.size();
        }

    private:
        std::list<LogDetail> msgs_;
        std::mutex mtx_;
    };

    class ThreadSink : public ISink
    {
    public:
        ThreadSink();
        virtual void log(const struct timeval& tv, LogLevel level, const std::string& msg) override;
        void notify();
        void flush(std::list<LogDetail>& out);

        inline void set_watermark(uint32_t watermark) {watermark_ = watermark;}
        inline int get_evtfd() const {return evtfd_;}

    private:
        std::shared_ptr<Buffer> append_buffer_;
        uint32_t watermark_ = 512;
        int evtfd_ = 0;
        std::mutex mtx_;
        bool notified_ = false;
    };

    class MultiThreadSink
    {
    public:
        MultiThreadSink();
        ~MultiThreadSink();
        void set_sink(std::shared_ptr<ISink> sink)
        {
            sink_ = sink;
        }

        std::shared_ptr<ThreadSink> create_thread_sink();
        void erase_sink(std::shared_ptr<ThreadSink> sink);
        void enable_sort() {isSort_ = true;}
        void disable_sort() {isSort_ = false;}
        void server();
        void thread_entry();

    private:
        void flush(std::list<LogDetail>& out);

    private:
        std::shared_ptr<ISink> sink_;
        std::unordered_set<std::shared_ptr<ThreadSink>> sinks_;
        bool isSort_ = true;
        std::thread thread_;
        std::mutex mtx_;
        int epoll_fd_ = 0;
        const uint32_t epoll_size_ = 256;
        std::atomic<bool> run_{false};
        uint32_t timeout_ = 1000;
    };

} // namespace log4rel
} // namespace soce
#endif
