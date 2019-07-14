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
#include "dispatch-queue.h"
#include "crpc/log.h"

using std::string;
using std::vector;

namespace soce{
namespace crpc{

    RequestIn::RequestIn(size_t consumers)
    {
        queue_.reset(new soce::utils::DispatchQueue_1n<QueueData>(consumers));
    }

    void RequestIn::produce(uint64_t conn_id, std::string&& service, std::string&& data)
    {
        CRPC_DEBUG << _S("conn_id", conn_id) << _S("service", service);
        queue_->produce(conn_id, std::move(service), std::move(data));
    }

    RequestOut::RequestOut(size_t producers)
    {
        queue_.reset(new soce::utils::DispatchQueue_n1<QueueData>(producers));
    }

    void RequestOut::produce(size_t pid, const std::vector<uint64_t>& conn_ids, int32_t tid, int64_t req_id, std::string&& data, bool p2p)
    {
        queue_->produce(pid, conn_ids, tid, req_id, std::move(data), p2p);
    }

    ResponseIn::ResponseIn(size_t consumers)
    {
        queue_.reset(new soce::utils::DispatchQueue_1n<QueueData>(consumers, [](const QueueData& data, size_t){
                    return data.consumer_id_;
                }));
    }

    void ResponseIn::produce(size_t consumer_id, int64_t reqid, std::string&& data)
    {
        queue_->produce(consumer_id, reqid, std::move(data));
    }

    ResponseOut::ResponseOut(size_t producers)
    {
        queue_.reset(new soce::utils::DispatchQueue_n1<QueueData>(producers));
    }

    void ResponseOut::produce(size_t pid, uint64_t conn_id, std::string&& data)
    {
        queue_->produce(pid, conn_id, std::move(data));
    }

} // namespace crpc
} // namespace soce
