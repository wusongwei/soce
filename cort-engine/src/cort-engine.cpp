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

#include <string.h>
#include <sys/time.h>
#include <cassert>
#include <dlfcn.h>
#include "cort-engine/cort-engine.h"
#include "utils/time-helper.h"

using namespace soce::utils;
using namespace std;

typedef ssize_t (*read_hook_t)(int fd, void *buf, size_t count);
typedef int (*close_hook_t)(int fd);

static read_hook_t g_read_hook = (read_hook_t)dlsym(RTLD_NEXT, "read");
static close_hook_t g_close_hook = (close_hook_t)dlsym(RTLD_NEXT, "close");

ssize_t read(int fd, void *buf, size_t count)
{
    if (!SCortEngine.is_enable_hook()){
        return g_read_hook(fd, buf, count);
    }

    SCortEngine.wait(fd);
    return g_read_hook(fd, buf, count);
}

int close(int fd)
{
    if (!SCortEngine.is_enable_hook()){
        return g_close_hook(fd);
    }

    SCortEngine.free(fd);
    return g_close_hook(fd);
}

namespace soce{
namespace cortengine{

    size_t CortEngine::stack_size = 8 * 1024;
    size_t CortEngine::shared_stack_size = 8 * 1024 * 1024;

#if defined (SOCE_USE_SHARED_STACK)
    static char* g_shared_stack = new char[CortEngine::shared_stack_size];
#endif

    void comain(CortEngine::CortId id)
    {
        auto iter = SCortEngine.corts_.find(id);
        if (iter == SCortEngine.corts_.end()){
            return;
        }

        CortEngine::CortPtr& ct = iter->second;
        ct->do_func();
        SCortEngine.finish(ct);
    }

    CortEngine::CortEngine()
    {
        snowflake_.reset(new soce::utils::Snowflake);
        hook_fd_.reset(new HookFd);
    }

    void CortEngine::set_snowflake(std::shared_ptr<soce::utils::Snowflake> snowflake)
    {
        assert(snowflake);
        snowflake_ = snowflake;
    }

    CortEngine::CortId CortEngine::create(CortEngine::CortFunc func)
    {
        CortPtr rt = SoceFactory<Cort>::create(snowflake_->gen_id(), func);
        corts_[rt->id_] = rt;
        return rt->id_;
    }

    CortEngine::CortId CortEngine::create(CortEngine::CortFunc func, uint32_t timeout)
    {
        CortPtr rt = SoceFactory<Cort>::create(snowflake_->gen_id(), func);
        corts_[rt->id_] = rt;
        enable_hook(timeout);
        return rt->id_;
    }

    int CortEngine::resume(CortEngine::CortId id)
    {
#if defined(SOCE_USE_SHARED_STACK)
        if (cur_cort_){
            return kResumeInOtherSharedCort;
        }
#endif
        auto iter = corts_.find(id);
        if (iter == corts_.end()){
            return kCortNotExist;
        }

        CortPtr ct = iter->second;
        switch (ct->status_)
        {
        case Cort::kStatusEpoch:
            makecontext(&ct->ctx_, (void (*)(void))&comain, 1, ct->id_);
            break;

        case Cort::kStatusRun:
            return kResumeSelf;

        case Cort::kStatusYield:
            yield_cts_[ct->expire_].erase(ct->id_);
            break;

        case Cort::kStatusDone:
            return kResumeFinishedCort;

        default:
            break;
        }

        swap_ctx(ct);
        return OK;
    }

    int CortEngine::yield()
    {
        if (!cur_cort_){
            return kCortNotExist;
        }

        CortPtr ct = cur_cort_;
        ct->status_ = Cort::kStatusYield;
        ct->timeout_ = false;
        ct->save_stack();
        cur_cort_.reset();

        if (ct->caller_ && ct->caller_->status_ == Cort::kStatusSuspend){
            cur_cort_ = ct->caller_;
            cur_cort_->restore_stack();
            swapcontext(&ct->ctx_, &ct->caller_->ctx_);
        }
        else{
            swapcontext(&ct->ctx_, &main_ctx_);
        }

        return OK;
    }

    int CortEngine::yield(uint32_t timeout)
    {
        if (!cur_cort_){
            return kCortNotExist;
        }

        if (timeout != 0){
            cur_cort_->expire_ = TimeHelper::get_time_ms() + timeout;
            yield_cts_[cur_cort_->expire_].push_back(cur_cort_->id_, cur_cort_);
        }

        return yield();
    }

    bool CortEngine::is_timeout()
    {
        if (!cur_cort_){
            return false;
        }

        return cur_cort_->timeout_;
    }

    CortEngine::CortId CortEngine::get_cur_cort_id() const
    {
        if (!cur_cort_){
            return 0;
        }

        return cur_cort_->id_;
    }

    void CortEngine::check_timeout()
    {
        uint64_t cur_tm = TimeHelper::get_time_ms();
        for (auto iter = yield_cts_.begin(); iter != yield_cts_.end();) {
            if (iter->first > cur_tm){
                break;
            }

            while (!iter->second.empty()){
                CortPtr cort = iter->second.front().second;
                cort->timeout_ = true;
                iter->second.erase(iter->second.front().first);
                resume(cort->id_);
            }

            iter = yield_cts_.erase(iter);
        }
    }

    int CortEngine::enable_hook(uint32_t timeout)
    {
        if (!cur_cort_){
            return kCortNotExist;
        }

        cur_cort_->enable_hook(timeout);
        return OK;
    }

    int CortEngine::disable_hook()
    {
        if (!cur_cort_){
            return kCortNotExist;
        }

        cur_cort_->disable_hook();
        return OK;
    }

    bool CortEngine::is_enable_hook() const
    {
        if (!cur_cort_){
            return false;
        }

        return cur_cort_->is_enable_hook();
    }

    void CortEngine::set_hook_callback(HookCallback hook_cb)
    {
        hook_cb_ = hook_cb;
    }

    void CortEngine::wait(int fd)
    {
        return wait(fd, hook_cb_);
    }

    void CortEngine::wait(int fd, HookCallback hook_cb)
    {
        if (!cur_cort_){
            return;
        }

        if (hook_cb){
            hook_cb(fd);
        }
        hook_fd_->wait(fd, cur_cort_->hook_timeout_);
    }

    void CortEngine::wakeup(int fd)
    {
        hook_fd_->wakeup(fd);
    }

    void CortEngine::free(int fd)
    {
        hook_fd_->free(fd);
    }

    void CortEngine::swap_ctx(CortPtr ct)
    {
        ct->status_ = Cort::kStatusRun;

        if (!cur_cort_){
            cur_cort_ = ct;
            ct->restore_stack();
            ct->ctx_.uc_link = &main_ctx_;
            swapcontext(&main_ctx_, &ct->ctx_);
        }
        else{
            cur_cort_->status_ = Cort::kStatusSuspend;
            ct->ctx_.uc_link = &cur_cort_->ctx_;
            CortPtr caller = cur_cort_;
            if (ct->caller_){
                if (ct->caller_->status_ == Cort::kStatusDone){
                    ct->caller_.reset();
                }
                else{
                    cur_cort_->caller_ = ct->caller_;
                }
            }
            ct->caller_ = cur_cort_;
            caller = cur_cort_;
            cur_cort_ = ct;
            caller->save_stack();
            ct->restore_stack();
            swapcontext(&caller->ctx_, &ct->ctx_);
        }
    }

    void CortEngine::finish(CortPtr ct)
    {
        ct->status_ = Cort::kStatusDone;
        CortPtr caller = ct->caller_;
        corts_.erase(ct->id_);

        ucontext_t ctx;

        if (!caller || caller->status_ == Cort::kStatusDone){
            if (caller){
                ct->caller_.reset();
            }
            cur_cort_.reset();
            swapcontext(&ctx, &main_ctx_);
        }
        else{
            cur_cort_ = caller;
            cur_cort_->restore_stack();
            swapcontext(&ctx, &caller->ctx_);
        }
    }

    CortEngine::Cort::Cort(CortId id, CortEngine::CortFunc func)
    {
        id_ = id;
        func_ = func;
        stack_buff_.reset(new char[CortEngine::stack_size]);
        getcontext(&ctx_);

#if defined (SOCE_USE_SHARED_STACK)
        cap_ = CortEngine::stack_size;
        size_ = 0;
        ctx_.uc_stack.ss_sp = g_shared_stack;
        ctx_.uc_stack.ss_size = CortEngine::shared_stack_size;
#else
        ctx_.uc_stack.ss_sp = stack_buff_.get();
        ctx_.uc_stack.ss_size = CortEngine::stack_size;
#endif
        ctx_.uc_stack.ss_flags = 0;
    }

    void CortEngine::Cort::save_stack()
    {
#if defined (SOCE_USE_SHARED_STACK)
        char top = 0;
        char* ptop = g_shared_stack + CortEngine::shared_stack_size;
        ptrdiff_t cur_stack_size = ptop - &top;

        if (cur_stack_size > cap_){
            ptrdiff_t lack = cur_stack_size - CortEngine::stack_size;
            stack_.reset(new char[lack]);
            cap_ = cur_stack_size;
        }

        if (cur_stack_size <= CortEngine::stack_size){
            memcpy(stack_buff_.get(), &top, cur_stack_size);
        }
        else{
            memcpy(stack_buff_.get(), &top, CortEngine::stack_size);
            memcpy(stack_.get(), (char*)&top + CortEngine::stack_size, cur_stack_size - CortEngine::stack_size);
        }
        size_ = cur_stack_size;
#endif
    }

    void CortEngine::Cort::restore_stack()
    {

#if defined (SOCE_USE_SHARED_STACK)
        if (!size_){
            return;
        }

        if (size_ < CortEngine::stack_size){
            memcpy(g_shared_stack + CortEngine::shared_stack_size - size_,
                   stack_buff_.get(),
                   size_);
        }
        else{
            size_t lack = size_ - CortEngine::stack_size;
            memcpy(g_shared_stack + CortEngine::shared_stack_size - size_,
                   stack_buff_.get(),
                   CortEngine::stack_size);
            memcpy(g_shared_stack + CortEngine::shared_stack_size - lack,
                   stack_.get(),
                   lack);
        }
#endif
    }

    void CortEngine::HookFd::wait(int fd, uint32_t timeout)
    {
        char holder;
        auto iter = channel_.find(fd);
        if (iter == channel_.end())
        {
            channel_[fd] = Channel<char>(timeout, timeout);
        }
        channel_[fd].pop(holder);
    }

    void CortEngine::HookFd::wakeup(int fd)
    {
        channel_[fd].push(fd);
    }

    void CortEngine::HookFd::free(int fd)
    {
        channel_.erase(fd);
    }

}} // end namespace
