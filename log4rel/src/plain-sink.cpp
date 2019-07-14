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

#include <time.h>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include "log4rel/plain-sink.h"
using namespace std;

namespace soce{
namespace log4rel{

    PlainSink::PlainSink()
    {
        flush_thread_ = std::thread(&PlainSink::flush_thread_entry, this, &flush_req_);
        while(!run_);
    }

    PlainSink::~PlainSink()
    {
        {
            std::unique_lock<std::mutex> lck(cv_mtx_);
            run_.store(false);
            cv_.notify_one();
        }

        if (flush_thread_.joinable()){
            flush_thread_.join();
        }

        flush();
    }

    void PlainSink::log(const struct timeval& time, LogLevel level, const std::string& msg)
    {
        (void) time;
        (void) level;
        string cur_date = get_date();

        if (cur_date_ != cur_date){
            cur_date_ = cur_date;
            open_log_file();
        }
        else if (msg.size() + cur_size_ > cap_){
            switch_log_file();
            open_log_file();
        }

        log(msg);
        cur_size_ += msg.size();
    }

    void PlainSink::flush_thread_entry(uint32_t* flush_freq)
    {
        run_.store(true);

        while (1){
            std::unique_lock<std::mutex> lck(cv_mtx_);
            if (!run_){
                break;
            }
            cv_.wait_for(lck, std::chrono::seconds(*flush_freq));

            mtx_.lock();
            flush();
            mtx_.unlock();
        }
    }

    void PlainSink::open_log_file()
    {
        string file_name = file_path_ + "/" + file_name_ + "_" + cur_date_ + ".log";
        ofs_.close();
        ofs_.open (file_name.c_str(), std::ofstream::out | std::ofstream::app);
        cur_size_ = ofs_.tellp();
    }

    void PlainSink::switch_log_file()
    {
        int i = 1;
        string file_name = file_path_ + "/" + file_name_ + "_" + cur_date_;

        while (true){
            ostringstream oss;
            oss << file_name << "_" << i << ".log";
            if (access(oss.str().c_str(), 0) != 0){
                break;
            }
            ++i;
        }

        for (; i>0; --i){
            ostringstream new_file;
            new_file << file_name << "_" << i << ".log";
            ostringstream old_file;
            if (i == 1){
                old_file << file_name << ".log";
            }
            else{
                old_file << file_name << "_" << (i-1) << ".log";
            }

            rename(old_file.str().c_str(), new_file.str().c_str());
        }
    }

    string PlainSink::get_date()
    {
        struct tm curr;
        time_t t;
        time(&t);
        curr = *localtime(&t);
        char sTmp[1024];
        strftime(sTmp,sizeof(sTmp),"%Y%m%d",&curr);
        return move(string(sTmp));
    }

    void PlainSink::log(const std::string& msg)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        buffer_oss_ << msg << endl;
        buffer_size_ += msg.size();
        if (buffer_size_ > max_buffer_size_){
            flush();
        }
   }

    void PlainSink::flush()
    {
        ofs_ << buffer_oss_.str();
        buffer_oss_.clear();
        buffer_oss_.str("");
        buffer_size_ = 0;
        ofs_.flush();
    }

} // namespace log4rel
} // namespace soce
