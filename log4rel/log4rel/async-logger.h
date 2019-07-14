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

#ifndef _SOCE_CRPC_ASYNC_LOGGER_XoJ8xeZ9UA_H_
#define _SOCE_CRPC_ASYNC_LOGGER_XoJ8xeZ9UA_H_

#include "log4rel/logger.h"
#include "log4rel/thread-sink.h"

namespace soce{
namespace crpc{

    class AsyncLogger
    {
    public:
        using LoggerCreator = std::function<std::shared_ptr<soce::log4rel::Logger>()>;
    public:
        void set_sink(std::shared_ptr<soce::log4rel::ISink> sink);
        void set_logger_creator(LoggerCreator creator);
        void init_thread_logger();
        soce::log4rel::LoggerMgr& get_logger_mgr();
        void start();

    private:
        std::mutex mtx_;
        soce::log4rel::MultiThreadSink sink_;
        soce::log4rel::LoggerMgr logger_mgr_;
        LoggerCreator creator_;
        bool started_ = false;
    };

}
}

#define SAsyncLogger soce::utils::SoceSingleton<soce::crpc::AsyncLogger>::get_instance()

#endif
