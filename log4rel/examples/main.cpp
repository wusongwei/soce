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

#include <iostream>
#include <cassert>
#include "log4rel/logger.h"
#include "log4rel/async-logger.h"
#include "log4rel/plain-sink.h"
#include "log4rel/thread-sink.h"
#include "log4rel/console-sink.hpp"
#include "unistd.h"

#include <sys/time.h>
using namespace std;
using namespace soce::log4rel;

void sample()
{
    // std::shared_ptr<Logger> logger;
    // logger.reset(new Logger);
    // logger->add_sink(shared_ptr<ISink>(new ConsoleSink));
    // logger->add_sink(shared_ptr<ISink>(new PlainSink));
    // SOCE_LOGGER_MGR.add_logger("test", logger);
    // SOCE_LOGGER_MGR.switch_logger("test");

    // SOCE_CUR_LOGGER->set_fmt(soce::log4rel::kLogFmtJson);
    SOCE_CUR_LOGGER->reserve_field(kLogFieldLevel, true);
    // SOCE_CUR_LOGGER->reserve_field(kLogFieldPos, true);
    // SOCE_CUR_LOGGER->reserve_field(kLogFieldPid, true);
    // SOCE_CUR_LOGGER->reserve_field(kLogFieldTid, true);
    // SOCE_CUR_LOGGER->reserve_field(kLogFieldKey, true);
    // SOCE_CUR_LOGGER->set_log_level(kLogLevelInfo);
    // SOCE_CUR_LOGGER->add_key_filter(kLogFilterAllow, "your.*");
    // SOCE_CUR_LOGGER->add_key_filter(kLogFilterDeny, "your.key2");
    // SOCE_CUR_LOGGER->add_record_filter(kLogFilterAllow, ".*default.*");

    SOCE_DEBUG << _S("str", "hello") << _N("num", 1) << _D("default key1");

    int x = 3;
    SOCE_INFO_IF(x>2) << _N("x", x);

    KEY_DEBUG("your.key2") << _S("str", "hello") << _N("num", 1) << _D("default key2");
    string key1 = "your";
    string key2 = "key3";
    KEY_DEBUG(key1 << "." << key2) << _S("str", "hello") << _N("num", 2) << _D("default key3");
}

#define NTHREAD 100
#define NRECORD 10000

std::shared_ptr<Logger> global_logger;

void thread_entry()
{
    SOCE_LOGGER_MGR.add_logger("test", global_logger);
    SOCE_LOGGER_MGR.switch_logger("test");

    std::thread::id tid = std::this_thread::get_id();
    for (size_t i=0; i<NRECORD; ++i){
        SOCE_INFO << _S("tid", tid) << _N("i", i);
    }
}

void sample_multi_thread_thread_safe_logger()
{
    SOCE_CUR_LOGGER->add_sink(std::shared_ptr<PlainSink>(new PlainSink));
    global_logger = SOCE_LOGGER_MGR.get_cur_logger();

    std::thread t[NTHREAD];
    for (size_t i = 0; i<NTHREAD; ++i) {
        t[i] = std::thread(thread_entry);
    }

    for (size_t i = 0; i<NTHREAD; ++i) {
        t[i].join();
    }
}

void thread_sink_entry(int tid)
{
    SAsyncLogger.init_thread_logger();

    for (size_t i=0; i<NRECORD; ++i){
        SOCE_INFO << _S("tid", tid) << _N("i", i);
    }
}

void sample_multi_thread_thread_sink()
{
    SAsyncLogger.set_sink(std::shared_ptr<PlainSink>(new PlainSink));
    SAsyncLogger.set_logger_creator([](){
            std::shared_ptr<soce::log4rel::Logger> logger(new soce::log4rel::Logger);
            //logger->reserve_field(soce::log4rel::kLogFieldLevel, true);
            logger->reserve_field(soce::log4rel::kLogFieldPos, true);
            logger->reserve_field(soce::log4rel::kLogFieldTid, true);
            //logger->set_log_level(soce::log4rel::kLogLevelInfo);
            return logger;
        });

    SAsyncLogger.start();

    std::thread t[NTHREAD];
    for (size_t i = 0; i<NTHREAD; ++i) {
        t[i] = std::thread(thread_sink_entry, i);
    }

    for (size_t i = 0; i<NTHREAD; ++i) {
        t[i].join();
    }
}

int main()
{
    sample();
    // sample_multi_thread_thread_safe_logger();
    // sample_multi_thread_thread_sink();

    return 0;
}
