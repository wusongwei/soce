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

#include <unistd.h>
#include <sys/syscall.h>
#include "utils/snowflake.h"
#include "utils/time-helper.h"

namespace soce{
namespace utils{

    Snowflake::Snowflake()
    {
        int64_t worker_id = (int64_t)syscall(__NR_gettid);
        gen_mask();
        worker_id_ = (worker_id & worker_id_mask_) << seq_size_;
    }

    int Snowflake::reset(int64_t worker_id)
    {
        if (worker_id & worker_id_mask_){
            return -1;
        }

        worker_id_ = (worker_id & worker_id_mask_) << seq_size_;
        return 0;
    }

    int Snowflake::reset(int64_t worker_id,
                         uint32_t worker_id_size,
                         uint32_t seq_size)
    {
        return reset(worker_id, worker_id_size, seq_size, 0);
    }

    int Snowflake::reset(int64_t worker_id,
                         uint32_t worker_id_size,
                         uint32_t seq_size,
                         int64_t epoch)
    {
        if ((63 - worker_id_size - seq_size) == 0){
            return -1;
        }

        worker_id_size_ = worker_id_size;
        seq_size_ = seq_size_;
        gen_mask();

        if (worker_id & worker_id_mask_){
            return -1;
        }
        worker_id_ = (worker_id & worker_id_mask_) << seq_size_;

        int64_t cur_time = TimeHelper::get_time_ms();
        if (epoch == -1){
            epoch_ = cur_time;
        }
        else if (epoch > cur_time){
            return -1;
        }

        return 0;
    }

    int64_t Snowflake::gen_id()
    {
        int64_t id = 0;
        int64_t last_time = 0;
        int64_t cur_time = 0;
        int64_t next_id = 0;
        int64_t last_seq = 0;

        do{
            id = id_.load();
            last_seq = id & seq_mask_;
            cur_time = get_timestamp();

            if (cur_time == last_time){
                if (last_seq == seq_mask_){
                    do{
                        cur_time = get_timestamp();
                    }while(cur_time == last_time);
                }
            }

            next_id = cur_time | worker_id_ | ((last_seq + 1) & seq_mask_);
            if (id_.compare_exchange_strong(id, next_id)){
                break;
            }

        }while(1);

        return next_id;
    }

    void Snowflake::gen_mask()
    {
        worker_id_mask_ = 0;
        for (size_t i=0; i<worker_id_size_; ++i){
            worker_id_mask_ = (worker_id_mask_ << 1) | 1;
        }

        seq_mask_ = 0;
        for (size_t i=0; i<seq_size_; ++i){
            seq_mask_ = (seq_mask_ << 1) | 1;
        }

        size_t timestamp_size = 63 - worker_id_size_ - seq_size_;
        timestamp_mask_ = 0;
        for (size_t i=0; i<timestamp_size; ++i){
            timestamp_mask_ = (timestamp_mask_ << 1) | 1;
        }
    }

    int64_t Snowflake::get_timestamp()
    {
        return ((TimeHelper::get_time_ms() - epoch_) << (worker_id_size_ + seq_size_)) & reserve_mask_;
    }

} // namespace utils
} // namespace soce
