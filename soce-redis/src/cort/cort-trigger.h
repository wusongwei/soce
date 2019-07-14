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

#ifndef _SOCE_CORT_TRIGGER_A45E5KF1Ay_H_
#define _SOCE_CORT_TRIGGER_A45E5KF1Ay_H_

#if defined(SOCE_USE_COROUTINE)

#include <unordered_set>
#include <list>
#include "cort-engine/cort-engine.h"
#include "utils/index-list.hpp"

using namespace soce::cortengine;

namespace soce{
namespace redis{

    class CortTrigger
    {
    public:
        using CortState = enum{
            kCortOk,
            kCortReset,
            kCortTimeout
        };

    public:
        CortState yield(int fd);
        int get_wait_fd();
        void wakeup_top();
        void pop();
        void reset();

    private:
        using CortInfo = struct CortInfo{
        CortInfo(CortEngine::CortId _id, int _fd, uint64_t _crt_tm)
        :id(_id), fd(_fd), crt_tm(_crt_tm){}
            CortEngine::CortId id;
            int fd;
            uint64_t crt_tm;
        };
        soce::utils::IndexList<soce::cortengine::CortEngine::CortId, CortInfo> corts_;

        CortState state_ = kCortOk;
        uint64_t timeout_ = 0;
    };

}} // end namespace
#endif
#endif
