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

#if defined(SOCE_USE_COROUTINE)
#include "cort-trigger.h"
#include "utils/time-helper.h"

using namespace std;
using namespace soce::cortengine;
using namespace soce::utils;

namespace soce{
namespace redis{
    CortTrigger::CortState CortTrigger::yield(int fd)
    {
        CortEngine::CortId id = SCortEngine.get_cur_cort_id();
        CortInfo* cort_info = NULL;

        if (corts_.get(id, &cort_info)){
            CortInfo cort_info(id, fd, TimeHelper::get_time_ms());
            corts_.push_back(id, cort_info);
        }
        else{
            cort_info->fd = fd;
        }

        if (timeout_ == 0){
            SCortEngine.yield();
        }
        else{
            if (!cort_info){
                corts_.get(id, &cort_info);
            }
            uint64_t cur_tm = TimeHelper::get_time_ms();
            if (cur_tm > cort_info->crt_tm + timeout_){
                return kCortTimeout;
            }
            else{
                SCortEngine.yield(cort_info->crt_tm + timeout_ - cur_tm);
            }
        }

        if (SCortEngine.is_timeout()){
            return kCortTimeout;
        }

        return state_;
    }

    int CortTrigger::get_wait_fd()
    {
        return corts_.empty() ? 0 : corts_.begin()->second.fd;
    }

    void CortTrigger::wakeup_top()
    {
        state_ = kCortOk;
        CortEngine::CortId id = corts_.begin()->second.id;
        SCortEngine.resume(id);
    }

    void CortTrigger::pop()
    {
        if (corts_.empty()){
            return;
        }

        corts_.pop_front();
    }

    void CortTrigger::reset()
    {
        for (auto iter=corts_.begin(); iter!=corts_.end(); ++iter){
            state_ = kCortReset;
            SCortEngine.resume(iter->second.id);
        }
    }

}} // end namespace
#endif
