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

#ifndef _SIMPLE_LOGGER_ZdSovXVxcu_H_
#define _SIMPLE_LOGGER_ZdSovXVxcu_H_

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "logger.h"

namespace soce{
namespace log4rel{

    class PlainSink : public ISink
    {
    public:
        PlainSink();
        ~PlainSink();

        virtual void log(const char* data, size_t len);
        inline void set_cap(size_t size) {
            std::lock_guard<std::mutex> lck(mtx_);
            cap_ = size;
        }

        inline void set_file_name(const std::string& name) {
            std::lock_guard<std::mutex> lck(mtx_);
            file_name_ = name;
        }

        inline void set_file_path(const std::string& path) {
            std::lock_guard<std::mutex> lck(mtx_);
            file_path_ = path;
        }

    private:
        void open_log_file();
        void switch_log_file();
        std::string get_date();

    private:
        size_t cap_ = 300 * 1024 * 1024;
        size_t cur_size_ = 0;
        std::string cur_date_;
        std::ofstream ofs_;
        std::string file_name_ = "soce";
        std::string file_path_ = ".";
        std::mutex mtx_;
    };

} // namespace log4rel
} // namespace soce
#endif
