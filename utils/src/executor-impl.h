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

#ifndef _SOCE_UTILS_EXECUTOR_IMPL_JbeslwKBDx_H_
#define _SOCE_UTILS_EXECUTOR_IMPL_JbeslwKBDx_H_

#include <thread>
#include "cort-engine/cort-engine.h"
#include "utils/executor.h"
#include "utils/blocking-queue.hpp"
#include "utils/dispatch-queue.hpp"

using soce::cortengine::CortEngine;

namespace soce {
namespace utils {

    class ExecutorImpl
    {
    public:
        ExecutorImpl(size_t min_core,
                     size_t max_core,
                     size_t idle_sec,
                     size_t max_queue_size,
                     soce::transport::TransportIf* transport);
        Executor::Status submit(std::function<void()> func);
        Executor::Status submit(std::function<void()> func, size_t tmout_ms);

    private:
        using ExecutorRequest = struct ExecutorRequest{
            Executor::ExecFunc func;
            CortEngine::CortId cort_id;
        };

        using ExecutorResponse = struct ExecutorResponse{
            ExecutorResponse(){}
        ExecutorResponse(CortEngine::CortId id)
        :cort_id(id){}
            CortEngine::CortId cort_id;
        };

    private:
        void add_worker();
        size_t get_idle_worker_index();
        void worker_entry(size_t worker_index, std::shared_ptr<BlockingQueue<ExecutorRequest>> req_queue);
        void detach(size_t worker_index);
        void handle_resp(int fd);

    private:
        size_t min_core_ = 1;
        size_t max_core_ = 1;
        size_t idle_sec_ = 0;
        size_t max_queue_size_ = -1;
        soce::transport::TransportIf* transport_;
        std::shared_ptr<BlockingQueue<ExecutorRequest>> req_queue_;
        DispatchQueue<ExecutorResponse> resp_queue_;
        std::mutex mtx_;
        std::unordered_map<size_t, std::thread> workers_;
        std::vector<std::thread> w_;
        size_t worker_index_ = 0;
    };

} // namespace utils
} // namespace soce
#endif
