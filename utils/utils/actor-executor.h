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

#ifndef _SOCE_UTILS_ACTOR_EXECUTOR_lTn9RLjLA6_H_
#define _SOCE_UTILS_ACTOR_EXECUTOR_lTn9RLjLA6_H_

#include <functional>
#include <memory>
#include "executor.h"
#include "actor.h"

namespace soce {
namespace utils {

    class ActorExecutor
    {
    public:
        using Status = enum{
            kOk,
            kTimeout
        };

        ActorExecutor(size_t min_core,
                      size_t max_core,
                      size_t idle_sec,
                      size_t max_queue_size,
                      soce::transport::TransportIf* transport);

        Executor::Status submit(Actor::ActorPtr actor, Actor::ActorTask task);
        Executor::Status submit(Actor::ActorPtr actor, Actor::ActorTask task, size_t tmout_ms);

    private:
        void exec(Actor::ActorPtr actor, Actor::ActorTask task);

    private:
        std::shared_ptr<Executor> executor_;
    };

} // namespace utils
} // namespace soce

#endif
