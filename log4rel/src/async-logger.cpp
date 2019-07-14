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

#include <thread>
#include <sstream>
#include "log4rel/async-logger.h"

namespace soce{
namespace crpc{

    void AsyncLogger::set_sink(std::shared_ptr<soce::log4rel::ISink> sink)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        sink_.set_sink(sink);
    }

    void AsyncLogger::set_logger_creator(LoggerCreator creator)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        creator_ = creator;
    }

    void AsyncLogger::init_thread_logger()
    {
        std::lock_guard<std::mutex> lck(mtx_);

        std::shared_ptr<soce::log4rel::Logger> logger;
        if (creator_){
            logger = creator_();
        }else{
            logger.reset(new soce::log4rel::Logger);
        }

        auto sink = sink_.create_thread_sink();
        logger->add_sink(sink);

        std::ostringstream oss;
        oss << "thread-" << std::this_thread::get_id();
        std::string tid = oss.str();
        SOCE_LOGGER_MGR.add_logger(tid, logger);
        SOCE_LOGGER_MGR.switch_logger(tid);

        logger_mgr_.add_logger(tid, logger);
    }

    soce::log4rel::LoggerMgr& AsyncLogger::get_logger_mgr()
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return logger_mgr_;
    }

    void AsyncLogger::start()
    {
        std::lock_guard<std::mutex> lck(mtx_);
        if (started_){
            return;
        }
        started_ = true;
        sink_.server();
    }

}
}
