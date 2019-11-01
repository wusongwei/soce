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

#ifndef _SOCE_LOG4REL_ASYNC_SINK_IGN5e6KmzE_H_
#define _SOCE_LOG4REL_ASYNC_SINK_IGN5e6KmzE_H_

#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include "sink-if.h"
#include "utils/funnel-buffer.h"

namespace soce{
namespace log4rel{

    class AsyncSink : public ISink
    {
    public:
        AsyncSink();
        AsyncSink(size_t buff_size, size_t water_mark, size_t log_interval);
        ~AsyncSink();

        void add_sink(std::shared_ptr<ISink> sink);
        virtual void log(const char* data, size_t len);

    private:
        void start();
        void worker_entry();

    private:
        size_t buff_size_ = 1024 * 1024;
        size_t water_mark_ = 1024;
        size_t log_interval_ = 500; // ms
        std::shared_ptr<soce::utils::FunnelBuffer> fb_;
        std::atomic<bool> run_{false};
        std::thread worker_;
        std::vector<std::shared_ptr<ISink>> sinks_;
    };

} // namespace log4rel
} // namespace soce


#endif
