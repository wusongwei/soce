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

#ifndef _SOCE_DISPATHCH_QUEUE_p1rml3z3nr_H_
#define _SOCE_DISPATHCH_QUEUE_p1rml3z3nr_H_

#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <stdint.h>
#include "crpc/service-if.h"
#include "transport/processor-if.h"
#include "utils/dispatch-queue.hpp"
#include "gen-cpp/crpc-header.h"

namespace soce{
namespace crpc{

    class RequestIn
    {
    public:
        using QueueData = struct QueueData{
        QueueData(uint64_t conn_id, std::string&& service, std::string&& data)
        : conn_id_(conn_id), service_(std::move(service)), data_(std::move(data))
                {
                }

            uint64_t conn_id_ = 0;
            std::string service_;
            std::string data_;
        };

    public:
        explicit RequestIn(size_t consumers);
        void produce(uint64_t conn_id, std::string&& service, std::string&& data);
        inline bool good() {return queue_->good();}
        inline int get_consumer_fd(size_t cid) {return queue_->get_consumer_fd(cid);}
        inline int try_consume(size_t cid, soce::utils::DQVector<QueueData>& out)
        {
            return queue_->try_consume(cid, out);
        }
        inline bool empty(size_t cid){return queue_->empty(cid);}
        inline void rebalance(size_t cid){queue_->rebalance(cid);}

    private:
        std::shared_ptr<soce::utils::DispatchQueue_1n<QueueData>> queue_;
    };

    class RequestOut
    {
    public:
        using QueueData = struct QueueData{
        QueueData(const std::vector<uint64_t> conn_ids, int32_t tid, int64_t req_id, std::string&& data, bool p2p)
        : conn_ids_(conn_ids), tid_(tid), req_id_(req_id), data_(std::move(data)), p2p_(p2p)
                {
                }

            std::vector<uint64_t> conn_ids_;
            int32_t tid_ = 0;
            int64_t req_id_ = 0;
            std::string data_;
            bool p2p_ = true;
        };

    public:
        explicit RequestOut(size_t producers);
        void produce(size_t pid, const std::vector<uint64_t>& conn_ids, int32_t tid, int64_t req_id, std::string&& data, bool p2p);
        inline bool good() {return queue_->good();}
        inline int get_consumer_fd() {return queue_->get_consumer_fd();}
        inline int try_consume(soce::utils::DQVector<QueueData>& out)
        {
            return queue_->try_consume(out);
        }

    private:
        std::shared_ptr<soce::utils::DispatchQueue_n1<QueueData>> queue_;
    };

    class ResponseIn
    {
    public:
        using QueueData = struct QueueData{
        QueueData(size_t consumer_id, int64_t reqid, std::string&& data)
        :consumer_id_(consumer_id), reqid_(reqid), data_(std::move(data))
                {
                }

            size_t consumer_id_ = 0;
            int64_t reqid_ = 0;
            std::string data_;
        };

    public:
        explicit ResponseIn(size_t consumers);
        void produce(size_t consumer_id, int64_t reqid, std::string&& data);
        inline bool good() {return queue_->good();}
        inline int get_consumer_fd(size_t cid) {return queue_->get_consumer_fd(cid);}
        inline int try_consume(size_t cid, soce::utils::DQVector<QueueData>& out)
        {
            return queue_->try_consume(cid, out);
        }

    private:
        std::shared_ptr<soce::utils::DispatchQueue_1n<QueueData>> queue_;
    };

    class ResponseOut
    {
    public:
        using QueueData = struct QueueData{
        QueueData(uint64_t conn_id, std::string&& data)
        : conn_id_(conn_id), data_(std::move(data))
                {
                }

            uint64_t conn_id_ = 0;
            std::string data_;
        };

    public:
        explicit ResponseOut(size_t producers);
        void produce(size_t pid, uint64_t conn_id, std::string&& data);
        inline bool good() {return queue_->good();}
        inline int get_consumer_fd() {return queue_->get_consumer_fd();}
        inline int try_consume(soce::utils::DQVector<QueueData>& out)
        {
            return queue_->try_consume(out);
        }

    private:
        std::shared_ptr<soce::utils::DispatchQueue_n1<QueueData>> queue_;
    };

} // namespace crpc
} // namespace soce
#endif
