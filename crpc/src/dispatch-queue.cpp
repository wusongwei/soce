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

namespace soce{
namespace crpc{

    RequestIn::RequestIn(size_t consumers)
    {
        queue_.reset(new soce::utils::DispatchQueue<QueueData>(consumers));
    }

    void RequestIn::produce(uint64_t conn_id, std::string&& service, std::string&& data)
    {
        CRPC_DEBUG << _S("conn_id", conn_id) << _S("service", service);
        queue_->produce(conn_id, std::move(service), std::move(data));
    }

    RequestOut::RequestOut()
    {
        queue_.reset(new soce::utils::DispatchQueue<QueueData>());
    }

    void RequestOut::produce(const std::set<uint64_t>& conn_ids, int32_t tid, int64_t req_id, std::string&& data)
    {
        queue_->produce(conn_ids, tid, req_id, std::move(data));
    }

    ResponseIn::ResponseIn(size_t consumers)
    {
        queue_.reset(new soce::utils::DispatchQueue<QueueData>(consumers, [](const QueueData& data, size_t){
                    return data.consumer_id_;
                }));
    }

    void ResponseIn::produce(size_t consumer_id, uint64_t conn_id, int64_t reqid, std::string&& data)
    {
        queue_->produce(consumer_id, conn_id, reqid, std::move(data));
    }

    ResponseOut::ResponseOut()
    {
        queue_.reset(new soce::utils::DispatchQueue<QueueData>());
    }

    void ResponseOut::produce(uint64_t conn_id, std::string&& data)
    {
        queue_->produce(conn_id, std::move(data));
    }

} // namespace crpc
} // namespace soce
