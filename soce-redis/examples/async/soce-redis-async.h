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

#ifndef _SOCE_REDIS_ASYNC_yFVwjRsrt2_H_
#define _SOCE_REDIS_ASYNC_yFVwjRsrt2_H_

#include <functional>
#include <thread>
#include "soce-redis/soce-redis.h"
#include "utils/dispatch-queue.hpp"
#include "transport/transport-libevent.h"
#include "transport/bypass-processor.h"
#include "cort-engine/cort-engine.h"
#include "log4rel/logger.h"

using namespace std;
using namespace soce::redis;
using namespace soce::transport;
using namespace soce::cortengine;
using namespace soce::log4rel;

class SoceRedisAsync
{
public:
    using Callback = std::function<void(std::shared_ptr<SoceRedis>)>;
    using AsyncRequest = struct AsyncRequest{
        AsyncRequest(){}
        AsyncRequest(SoceRedis::Fmt fmt, Callback callback){
            this->fmt = fmt;
            this->callback = callback;
        }

        SoceRedis::Fmt fmt;
        Callback callback;
    };

public:
    SoceRedisAsync();
    ~SoceRedisAsync();
    int init(std::string conf);
    void exec_async(SoceRedis::Fmt fmt, Callback callback);

private:
    void thread_entry();
    void handle_request(int fd);

private:
    std::thread thread_;
    soce::utils::DispatchQueue<AsyncRequest> queue_;
    std::shared_ptr<CortConnPool<std::shared_ptr<SoceRedis>>> pool_;
};

#endif
