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

#include <unistd.h>
#include "utils/evtfd-mail.h"

namespace soce{
namespace utils{

    EvtfdMail::EvtfdMail()
    {
        evtfd_ = eventfd(0, 0);
    }

    EvtfdMail::~EvtfdMail()
    {
        if (evtfd_ > 0){
            close(evtfd_);
        }
    }

    int EvtfdMail::notify()
    {
        int rc = 0;
        if (!notified_) {
            uint64_t val = 1;
            if (write(evtfd_, &val, sizeof(uint64_t)) != sizeof(uint64_t)){
                rc = -1;
            }else{
                notified_ = true;
            }
        }
        return rc;
    }

    int EvtfdMail::clear()
    {
        int rc = 0;
        if (notified_) {
            uint64_t val;
            if (read(evtfd_, (void*)&val, sizeof(uint64_t)) != sizeof(uint64_t)){
                rc = -1;
            }else{
                notified_ = false;
            }
        }
        return rc;
    }

    int EvtfdMailMt::get_fd()
    {
        ReadLockGuard lck(lock_);
        return email_.get_fd();
    }

    bool EvtfdMailMt::good()
    {
        ReadLockGuard lck(lock_);
        return email_.good();
    }

    bool EvtfdMailMt::is_notified()
    {
        ReadLockGuard lck(lock_);
        return email_.is_notified();
    }

    int EvtfdMailMt::notify()
    {
        ReadLockGuard lck(lock_);
        return email_.notify();
    }

    int EvtfdMailMt::clear()
    {
        WriteLockGuard lck(lock_);
        return email_.clear();
    }

} // namespace utils
} // namespace soce
