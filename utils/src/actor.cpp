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
        while (1){
            if (rwlock_.try_write_lock()){
                if (running_){ // running on other thread
                    put_task(task);
                    rwlock_.write_unlock();
                }else{ // running on current thread
                    running_ = true;
                    rwlock_.write_unlock();
                    task(shared_from_this());
                    apply_accumulated();
                }

                break;
            }else{
                if (add_task(task)) {
                    break;
                }
            }
        }
    }

    bool Actor::add_task(Actor::ActorTask task)
    {
        ReadLockGuard rlg(rwlock_);

        bool added = false;
        if (running_){
            std::lock_guard<std::mutex> lck(mtx_);
            task_queue_.push(task);
            added = true;
        }

        return added;
    }

    Actor::ActorTask Actor::get_task()
    {
        std::lock_guard<std::mutex> lck(mtx_);
        Actor::ActorTask task;
        if (!task_queue_.empty()){
            task = task_queue_.front();
            task_queue_.pop();
        }
        return task;
    }

    void Actor::put_task(Actor::ActorTask task)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        task_queue_.push(task);
    }

    void Actor::apply_accumulated()
    {
        while (1){
            rwlock_.write_lock();
            auto task = get_task();
            if (task) {
                rwlock_.write_unlock();
                task(shared_from_this());
                continue;
            }
            else{
                running_ = false;
                rwlock_.write_lock();
                break;
            }
        }
    }

} // namespace utils
} // namespace soce
