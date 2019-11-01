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
#include <chrono>
#include "log4rel/plain-sink.h"
using namespace std;

namespace soce{
namespace log4rel{

    PlainSink::PlainSink()
    {
    }

    PlainSink::~PlainSink()
    {
    }

    void PlainSink::log(const char* data, size_t len)
    {
        std::unique_lock<std::mutex> lck(mtx_);

        string cur_date = get_date();

        if (cur_date_ != cur_date){
            cur_date_ = cur_date;
            open_log_file();
        }
        else if (len + cur_size_ > cap_){
            switch_log_file();
            open_log_file();
        }

        ofs_.write(data, len);
        cur_size_ += len;
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

} // namespace log4rel
} // namespace soce
