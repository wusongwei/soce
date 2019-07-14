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

 #ifndef _SOCE_RWLOCK_ORThOtauNi_HPP_
#define _SOCE_RWLOCK_ORThOtauNi_HPP_

#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

#include <iostream>
using namespace std;

namespace soce{
namespace utils{

    class RWLock{
    public:
        void read_lock()
            {
                while (1){
                    if (writing_){
                        std::this_thread::yield();
                        continue;
                    }

                    int readers = readers_;
                    if (readers_.compare_exchange_weak(readers, readers + 1)){
                        if (!writing_){
                            break;
                        }

                        read_unlock();
                    }
                }
            }

        template <class Rep, class Period>
        bool try_read_lock_for(const std::chrono::duration<Rep,Period>& rel_time)
            {
                auto end = std::chrono::system_clock::now() + rel_time;

                while (1){
                    auto now = std::chrono::system_clock::now();
                    if (now > end){
                        return false;
                    }

                    if (writing_){
                        std::this_thread::yield();
                        continue;
                    }

                    int readers = readers_;
                    if (readers_.compare_exchange_weak(readers, readers + 1)){
                        if (!writing_){
                            break;
                        }

                        read_unlock();
                    }
                }

                return true;
            }

        void read_unlock()
            {
                while (1){
                    int readers = readers_;
                    if (readers_.compare_exchange_weak(readers, readers - 1)){
                        break;
                    }
                }
            }

        void write_lock()
            {
                mtx_.lock();
                ++writing_;
                while (1){
                    int readers = 0;
                    if (readers_.compare_exchange_weak(readers, 0)){
                        break;
                    }
                }
            }

        template <class Rep, class Period>
        bool write_lock(const std::chrono::duration<Rep,Period>& rel_time)
            {
                auto end = std::chrono::system_clock::now() + rel_time;
                if (mtx_.try_lock_for(rel_time)){
                    return false;
                }

                ++writing_;
                while (1){
                    int readers = 0;
                    if (readers_.compare_exchange_weak(readers, 0)){
                        break;
                    }

                    auto now = std::chrono::system_clock::now();
                    if (now > end){
                        return false;
                    }
                }

                return true;
            }

        void write_unlock()
            {
                --writing_;
                mtx_.unlock();
            }

    private:
        std::atomic<int> readers_{0};
        std::atomic<int> writing_{0};
        std::timed_mutex mtx_;
    };

} // namespace utils
} // namespace soce

#endif
