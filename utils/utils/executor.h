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

#ifndef _SOCE_UTILS_EXECUTOR_jF44DGQsuV_H_
#define _SOCE_UTILS_EXECUTOR_jF44DGQsuV_H_

#include <functional>
#include <memory>
#include "transport/transport-if.h"

namespace soce {
namespace utils {

    class ExecutorImpl;

    class Executor
    {
    public:
        using Status = enum{
            kOk,
            kTimeout
        };

        using ExecFunc = std::function<void()>;

        Executor(size_t min_core,
                 size_t max_core,
                 size_t idle_sec,
                 size_t max_queue_size,
                 soce::transport::TransportIf* transport);
        Status submit(ExecFunc func);
        Status submit(ExecFunc func, size_t tmout_ms);

    private:
        std::shared_ptr<ExecutorImpl> impl_;
    };

} // namespace utils
} // namespace soce
#endif
