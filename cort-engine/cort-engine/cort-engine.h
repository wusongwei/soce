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

#ifndef _CORT_ENGINE_18F397K6SB_H_
#define _CORT_ENGINE_18F397K6SB_H_

#include <ucontext.h>
#include <stddef.h>
#include <unistd.h>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <map>
#include <stdint.h>
#include "utils/singleton.hpp"
#include "utils/nocopyable.hpp"
#include "utils/index-list.hpp"
#include "utils/factory.hpp"
#include "utils/snowflake.h"

#define SCortEngine soce::utils::SoceSingletonThreadLocal<soce::cortengine::CortEngine>::get_instance()

namespace soce{
namespace cortengine{

    class CortEngine : public soce::utils::NoCopyable
    {
    public:
        using Errno = enum{
            OK = 0,

            /* for CortEngine */
            kCortNotExist = -1,
            kResumeInOtherSharedCort = -2,
            kResumeSelf = -3,
            kResumeFinishedCort = -4,

            /* for channel */
            kEmptyMailbox = -10,
            kMailboxOverflow = -11
        };

    public:
        using CortFunc = std::function<void()>;
        using CortId = int64_t;
        using HookCallback = std::function<void(int)>;

    public:
        CortEngine();

        /*
         * Snowflake is used to generate coroutine id, it's thread-safe.
         * So one snowflake may be used in multi-thread.
         *
         * @param snowflake The Snowflake used to create coroutine id.
         */
        void set_snowflake(std::shared_ptr<soce::utils::Snowflake> snowflake);

        /*
         * Create a coroutine, just create.
         * It does not start before call resume().
         *
         * @param func The entry of a coroutine
         *
         * @return CortId The coroutine id. see resume().
         */
        CortId create(CortFunc func);

        /*
         * Create a coroutine, and enable_hook with timeout.
         * It does not start before call resume().
         *
         * @param func The entry of a coroutine
         * @param timeout Timeout after be hooked and scheduled out.
         *
         * @return CortId The coroutine id. see resume().
         */
        CortId create(CortFunc func, uint32_t timeout);

        /*
         * Start / Restart a coroutine.
         * A coroutine may be scheduled out by yield().
         *
         * @param id The coroutine id.
         *
         * @return OK Success
         *         kCortNotExist Coroutine not exist
         *         kResumeInOtherSharedcort Resume coroutine in another coroutine with shared stack
         *         kResumeSelf Resume self
         *         kResumeFinishedCort Resume a finished coroutine
         */
        int resume(CortId id);

        /*
         * Yield a coroutine.
         * The coroutine will never be scheduled unless call resume().
         *
         * @return OK success
         *         kCortNotExist Coroutine not exists.
         */
        int yield();

        /*
         * Yield a coroutine with timeout.
         * The function check_timeout() will check whether a timed-coroutine
         * is timeout, and resume() it if it is.
         * So call is_timeout() first after the timed-coroutine be rescheduled.
         *
         * @param timeout timeout in microseconds
         *
         * @return OK success
         *         kCortNotExist Coroutine not exists.
         */
        int yield(uint32_t timeout);

        /*
         * Check whether the coroutine is timeout
         *
         * @return true   Timeout
         *         false  Not timeout or not in coroutine
         */
        bool is_timeout();

        /*
         * Get the current coroutine id.
         *
         * @return 0 not in coroutine.
         *         other The cort id.
         */
        CortId get_cur_cort_id() const;

        /*
         * Call it repeatly to check whether the yielded coroutines are timeout.
         */
        void check_timeout();

        /*
         * Enable hook for current coroutine.
         * If enabled, the coroutine will be scheduled out
         * when read() and other hooked function be called.
         * You can call wakeup() to wakeup a coroutine which
         * is waiting for data on that fd.
         *
         * @param timeout timeout for yield
         * @return OK Success
         *         kCortNotExist Not in coroutine.
         */
        int enable_hook(uint32_t timeout);
        int disable_hook();
        bool is_enable_hook() const;
        void set_hook_callback(HookCallback hook_cb);

        /*
         * Wait on a fd manualy.
         */
        void wait(int fd);
        void wait(int fd, HookCallback hook_cb);
        void wakeup(int fd);
        void free(int fd);

    private:
        class Cort;
        using CortPtr = std::shared_ptr<soce::cortengine::CortEngine::Cort>;

        void swap_ctx(CortPtr ct);
        void finish(CortPtr ct);

    private:
        std::unordered_map<CortId, CortPtr> corts_;
        /* map will sort thre yield_cts by yiled time */
        std::map<uint64_t, soce::utils::IndexList<CortId, CortPtr>> yield_cts_;
        CortPtr cur_cort_;
        ucontext_t main_ctx_;
        std::shared_ptr<soce::utils::Snowflake> snowflake_;

        class HookFd;
        std::shared_ptr<HookFd> hook_fd_;
        HookCallback hook_cb_;

        friend void comain(CortId id);

    public:
        static void set_stack_size(size_t size){stack_size = size;}
        static void set_shared_stack_size(size_t size){shared_stack_size = size;}
        static size_t get_stack_size(){return stack_size;}
        static size_t get_shared_stack_size(){return shared_stack_size;}

    private:
        static size_t stack_size;
        static size_t shared_stack_size;
    };

    class CortEngine::Cort : public soce::utils::NoCopyable
    {
    public:
        Cort(CortId id, CortEngine::CortFunc func);

        inline void set_func(CortFunc func){func_ = func;}
        inline void do_func(){func_();}
        inline CortPtr get_caller() const {return caller_;}
        inline bool is_timeout() const {return timeout_;}
        inline bool is_enable_hook() const {return enable_hook_;}
        inline void enable_hook(uint32_t timeout)
        {
            enable_hook_ = true;
            hook_timeout_ = timeout;
        }

        void disable_hook()
        {
            enable_hook_ = false;
            hook_timeout_ = 0;
        }

        void save_stack();
        void restore_stack();

    private:
        using Status = enum{
            kStatusEpoch,
            kStatusRun,
            kStatusSuspend,
            kStatusYield,
            kStatusDone
        };
        CortId id_;
        CortFunc func_;
        std::shared_ptr<char> stack_buff_;
        ucontext_t ctx_;
        Status status_ = kStatusEpoch;
        CortPtr caller_;
        uint64_t expire_ = 0;
        bool timeout_ = false;
        bool enable_hook_ = false;
        uint32_t hook_timeout_ = 0;

#if defined(SOCE_USE_SHARED_STACK)
        std::shared_ptr<char> stack_;
        ptrdiff_t cap_ = 0;
        ptrdiff_t size_ = 0;
#endif
        friend class CortEngine;
    };

    template<typename T>
        class Channel
    {
    public:
        Channel() = default;
        Channel(uint32_t push_timeout, uint32_t pop_timeout, uint32_t cap = -1)
        : push_timeout_(push_timeout),
            pop_timeout_(pop_timeout),
            cap_(cap){}

        ~Channel()
        {
            while (!block_read_cts_.empty()){
                auto id = block_read_cts_.front();
                block_read_cts_.pop();
                SCortEngine.resume(id);
            }

            while (!block_write_cts_.empty()){
                auto id = block_write_cts_.front();
                block_write_cts_.pop();
                SCortEngine.resume(id);
            }
        }

        /*
         * Push data and resume a pending pop cort if any.
         *
         * @param mail The input data.
         *
         * @return OK Success
         *         kCortNotExist Mailbox is full, and currently not in coroutine.
         *         kMailboxOverflow Mailbox is still full after coroutine resumed.
         */
        int push(const T& mail)
        {
            return push([&mail, this]{
                    mailbox_.push(mail);
                });
        }

        int push(T&& mail)
        {
            return push([&mail, this]{
                    mailbox_.push(std::move(mail));
                });
        }
        /*
         * Get data and resume a pending push cort if any.
         *
         * @param mail The output.
         *
         * @return OK Success
         *         kCortNotExist Mailbox is empty, and currently not in coroutine.
         *         kEmptyMailbox Mailbox is still empty after coroutine resumed.
         */
        int pop(T& mail)
        {
            if (mailbox_.empty()){
                CortEngine::CortId id = SCortEngine.get_cur_cort_id();
                if (id == 0){
                    return CortEngine::kCortNotExist;
                }

                block_read_cts_.push(id);

                if (pop_timeout_){
                    SCortEngine.yield(pop_timeout_);
                }
                else{
                    SCortEngine.yield();
                }

                if (mailbox_.empty()){
                    return CortEngine::kEmptyMailbox;
                }
            }

            mail = std::move(mailbox_.front());
            mailbox_.pop();

            while (!block_write_cts_.empty() && mailbox_.size() < cap_){
                CortEngine::CortId id = block_write_cts_.front();
                block_write_cts_.pop();
                SCortEngine.resume(id);
            }

            return CortEngine::OK;
        }

        size_t size()
        {
            return mailbox_.size();
        }

        void reset()
        {
            while (!block_read_cts_.empty()){
                CortEngine::CortId id = block_read_cts_.front();
                block_read_cts_.pop();
                SCortEngine.resume(id);
            }

            while (!block_write_cts_.empty()){
                CortEngine::CortId id = block_write_cts_.front();
                block_write_cts_.pop();
                SCortEngine.resume(id);
            }

            mailbox_.clear();
        }

    private:
        int push(std::function<void()> push_func)
        {
            if (mailbox_.size() >= cap_){
                CortEngine::CortId id = SCortEngine.get_cur_cort_id();
                if (id == 0){
                    return CortEngine::kCortNotExist;
                }
                block_write_cts_.push(id);
                if (push_timeout_){
                    SCortEngine.yield(push_timeout_);
                }
                else{
                    SCortEngine.yield();
                }

                if (mailbox_.size() >= cap_){
                    return CortEngine::kMailboxOverflow;
                }
            }

            push_func();

            while (!block_read_cts_.empty() && !mailbox_.empty()){
                CortEngine::CortId id = block_read_cts_.front();
                block_read_cts_.pop();
                SCortEngine.resume(id);
            }

            return CortEngine::OK;
        }

    private:
        std::queue<T> mailbox_;
        std::queue<CortEngine::CortId> block_read_cts_;
        std::queue<CortEngine::CortId> block_write_cts_;
        uint32_t push_timeout_ = 0;
        uint32_t pop_timeout_ = 0;
        uint32_t cap_ = -1;
    };

    class CortEngine::HookFd
    {
    public:
        void wait(int fd, uint32_t timeout);
        void wakeup(int fd);
        void free(int fd);

    private:
        std::unordered_map<int, Channel<char>> channel_;
    };

    template <class T>
        class CortConnPool
    {
    public:
    CortConnPool(uint32_t timeout_ms, size_t cap, std::function<T()> creator)
        : cap_(cap), creator_(creator)
        {
            chann_ = std::shared_ptr<Channel<T>>(new Channel<T>(0,
                                                                timeout_ms,
                                                                cap));
        }

        int get(T& out)
        {
            int rc = 0;
            if (chann_->size()){
                rc = chann_->pop(out);
            }else if (created_ < cap_){
                out = creator_();
                ++created_;
            }else{
                rc = chann_->pop(out);
            }

            return rc;
        }

        int put(T& in)
        {
            if (chann_->size() >= cap_){
                return -1;
            }

            chann_->push(in);
            return 0;
        }

        void discard(T& in)
        {
            (void) in;

            if (created_){
                --created_;
            }
        }

    private:
        size_t cap_ = 100;
        size_t created_ = 0;
        uint32_t pop_timeout = 0;
        std::function<T()> creator_;
        std::shared_ptr<Channel<T>> chann_;
    };

} // namespace cortengine
} // namespace soce

#endif
