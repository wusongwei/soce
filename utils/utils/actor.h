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

#ifndef _SOCE_UTILS_ACTOR_mfHAMM098W_H_
#define _SOCE_UTILS_ACTOR_mfHAMM098W_H_

#include <functional>
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include "transport/transport-if.h"
#include "utils/rwlock.hpp"

namespace soce {
namespace utils {

    class Actor : public std::enable_shared_from_this<Actor>
    {
    public:
        using ActorPtr = std::shared_ptr<Actor>;
        using ActorTask = std::function<void(ActorPtr)>;

    public:
        void apply_task(ActorTask task);

    private:
        bool add_task(ActorTask task);
        ActorTask get_task();
        void put_task(ActorTask task);
        void apply_accumulated();

    protected:
        std::queue<ActorTask> task_queue_;
        std::mutex mtx_;
        RWLock rwlock_;
        volatile bool running_ = false;
    };

} // namespace utils
} // namespace soce

#endif
