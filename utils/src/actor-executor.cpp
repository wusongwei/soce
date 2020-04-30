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

#include "utils/actor.h"
#include "utils/actor-executor.h"

namespace soce {
namespace utils {

    ActorExecutor::ActorExecutor(size_t min_core,
                                 size_t max_core,
                                 size_t idle_sec,
                                 size_t max_queue_size,
                                 soce::transport::TransportIf* transport)
    {
        executor_ = std::make_shared<Executor>(min_core, max_core, idle_sec, max_queue_size, transport);
    }

    Executor::Status ActorExecutor::submit(Actor::ActorPtr actor, Actor::ActorTask task)
    {
        Executor::ExecFunc func = std::bind(&ActorExecutor::exec, this, actor, task);
        return executor_->submit(func);
    }

    Executor::Status ActorExecutor::submit(Actor::ActorPtr actor, Actor::ActorTask task, size_t tmout_ms)
    {
        Executor::ExecFunc func = std::bind(&ActorExecutor::exec, this, actor, task);
        return executor_->submit(func, tmout_ms);
    }

    void ActorExecutor::exec(Actor::ActorPtr actor, Actor::ActorTask task)
    {
        actor->apply_task(task);
    }

} // namespace utils
} // namespace soce
