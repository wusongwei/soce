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

#ifndef _SOCE_SNOWFLAKE_6ojZFoUbDv_H_
#define _SOCE_SNOWFLAKE_6ojZFoUbDv_H_

#include <atomic>
#include <sys/types.h>

namespace soce{
namespace utils{

    class Snowflake
    {
    public:
        Snowflake();
        int reset(int64_t worker_id);
        int reset(int64_t worker_id, uint32_t worker_id_size, uint32_t seq_size);
        int reset(int64_t worker_id,
                  uint32_t
                  worker_id_size,
                  uint32_t seq_size,
                  int64_t epoch);
        int64_t gen_id();

    private:
        void gen_mask();
        int64_t get_timestamp();

    private:
        int64_t worker_id_ = 0;
        size_t worker_id_size_ = 10;
        int64_t worker_id_mask_ = 0;
        size_t seq_size_ = 12;
        int64_t seq_mask_ = 0;
        int64_t timestamp_mask_ = 0;
        int64_t epoch_ = 1577808000000L; // 2020-01-01 00:00:00
        std::atomic<int64_t> id_{0};
        constexpr static int64_t reserve_mask_ = ~((int64_t)1 << 63);
    };

} // namespace utils
} // namespace soce

#endif
