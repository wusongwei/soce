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

namespace soce {
namespace utils {

    void Actor::apply_task(Actor::ActorTask task)
    {
        mtx_.lock();
        if (running_) {
            task_queue_.push(task);
            mtx_.unlock();
        }else{
            running_ = true;
            mtx_.unlock();
            task(shared_from_this());
            apply_accumulated();
        }
    }

    void Actor::apply_accumulated()
    {
        while (1){
            mtx_.lock();
            auto task = get_task();
            if (task) {
                mtx_.unlock();
                task(shared_from_this());
                continue;
            }
            else{
                running_ = false;
                mtx_.unlock();
                break;
            }
        }
    }

    Actor::ActorTask Actor::get_task()
    {
        Actor::ActorTask task;
        if (!task_queue_.empty()){
            task = task_queue_.front();
            task_queue_.pop();
        }
        return task;
    }

} // namespace utils
} // namespace soce
