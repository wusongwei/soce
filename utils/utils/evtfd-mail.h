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

#ifndef _SOCE_UTILS_EVTFD_MAIL_gEkN0ZyeST_H_
#define _SOCE_UTILS_EVTFD_MAIL_gEkN0ZyeST_H_

#include <sys/eventfd.h>
#include <atomic>
#include "rwlock.hpp"

namespace soce{
namespace utils{

    class EvtfdMail
    {
    public:
        EvtfdMail();
        ~EvtfdMail();
        EvtfdMail(const EvtfdMail&) = delete;
        EvtfdMail& operator=(const EvtfdMail&) = delete;

        inline int get_fd() {return evtfd_;}
        inline bool good() {
                return (evtfd_ > 0) ? true : false;
            }
        inline bool is_notified(){return notified_;}
        int notify();
        int clear();

    private:
        int evtfd_ = -1;
        std::atomic<bool> notified_{false};
    };

    class EvtfdMailMt
    {
    public:
        int get_fd();
        bool good();
        bool is_notified();
        int notify();
        int clear();

    private:
        EvtfdMail email_;
        RWLock lock_;
    };

} // namespace utils
} // namespace soce

#endif
