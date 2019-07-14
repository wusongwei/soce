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
#include <unordered_set>
#include <mutex>
#include "utils/snowflake.h"
#include "log4rel/logger.h"

using namespace soce::utils;

class ExampleSnowflake
{
public:
    void start()
        {
            SOCE_DEBUG << _D("--------------- sample_snowflake ---------------");

            std::thread t[thread_num_];
            for (size_t i=0; i<thread_num_; ++i){
                t[i] = std::thread(&ExampleSnowflake::gen_id, this);
            }

            for (size_t i=0; i<thread_num_; ++i){
                t[i].join();
            }

            SOCE_DEBUG << _S("gen ids", ids_.size())
                       << _S("expect", (thread_num_ *  id_num_));
        }

private:
    void gen_id()
        {
            for (size_t i =0; i<id_num_; ++i){
                int64_t id = snowflake_.gen_id();
                mtx_.lock();
                ids_.insert(id);
                mtx_.unlock();
            }
        }

private:
    Snowflake snowflake_;
    std::mutex mtx_;
    const size_t thread_num_ = 5;
    const size_t id_num_ = 10000;
    std::unordered_set<int64_t> ids_;
};

void snowflake()
{
    ExampleSnowflake ss;
    ss.start();
}
