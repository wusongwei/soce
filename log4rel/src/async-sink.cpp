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

#include "log4rel/async-sink.h"

namespace soce{
namespace log4rel{

    AsyncSink::AsyncSink()
    {
        start();
    }

    AsyncSink::AsyncSink(size_t buff_size, size_t water_mark, size_t log_interval)
        : buff_size_(buff_size),
          water_mark_(water_mark),
          log_interval_(log_interval)
    {
        start();
    }

    AsyncSink::~AsyncSink()
    {
        if (run_) {
            run_ = false;
            worker_.join();
        }
    }

    // it's not thread safe right now
    void AsyncSink::add_sink(std::shared_ptr<ISink> sink)
    {
        sinks_.push_back(sink);
    }

    void AsyncSink::log(const char* data, size_t len)
    {
        fb_->put(data, len);
    }

    void AsyncSink::start()
    {
        bool run = false;
        if (run_.compare_exchange_weak(run, true)) {
            fb_ = std::make_shared<soce::utils::FunnelBuffer>(soce::utils::FunnelBuffer::kStrategyYielding,
                                                              buff_size_,
                                                              water_mark_);

            worker_ = std::thread(&AsyncSink::worker_entry, this);
        }
    }

    void AsyncSink::worker_entry()
    {
        while (run_) {
            fb_->flush([this](const char* data, size_t len){
                    for (auto& i : sinks_) {
                        i->log(data, len);
                    }
                },
                log_interval_);
        }

        fb_->flush([this](const char* data, size_t len){
                for (auto& i : sinks_) {
                    i->log(data, len);
                }
            }, 1);
    }

} // namespace log4rel
} // namespace soce
