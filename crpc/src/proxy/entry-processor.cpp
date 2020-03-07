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

#include <sys/sysinfo.h>        // get_nprocs
#include "entry-processor.h"
#include "crpc/log.h"
#include "transport/bypass-processor.h"

using namespace soce::transport;
using namespace soce::utils;

namespace soce {
namespace crpc {

    ProxyEntryProcessor::ProxyEntryProcessor(size_t thread_num,
                                             TransportIf* transport)
    {
        if (shared_thread_num_ == 0){
            shared_thread_num_ = 2 * (size_t)get_nprocs() + 1;
        }

        req_queue_ = std::make_shared<DispatchQueue<ProxyDispatcher::ProxyRequest>>(shared_thread_num_);
        resp_queue_ = std::make_shared<DispatchQueue<ProxyDispatcher::ProxyResponse>>();

        set_transport(transport);

        transport->watch(resp_queue_->get_consumer_fd(0),
                         std::shared_ptr<BypassProcessor>(
                             new BypassProcessor(
                                 std::bind(&ProxyEntryProcessor::handle_resp,
                                           this,
                                           std::placeholders::_1))));
    }

    void ProxyEntryProcessor::process(uint64_t conn_id, std::string&& data)
    {
        req_queue_->produce(conn_id, std::move(data));
    }

    void ProxyEntryProcessor::on_error(uint64_t conn_id)
    {

    }

    std::shared_ptr<DispatchQueue<ProxyDispatcher::ProxyRequest>> ProxyEntryProcessor::get_req_queue()
    {
        return req_queue_;
    }

    std::shared_ptr<DispatchQueue<ProxyDispatcher::ProxyResponse>> ProxyEntryProcessor::get_resp_queue()
    {
        return resp_queue_;
    }

    void ProxyEntryProcessor::handle_resp(int fd)
    {
        (void) fd;
        soce::utils::FQVector<ProxyDispatcher::ProxyResponse> resps;
        if (resp_queue_->try_consume(0, resps)){
            return;
        }

        for (auto& i : resps){
            CRPC_DEBUG << _S("SendRespTo", i.conn_id) << _S("RespSize", i.response.size());
            transport_->send(i.conn_id, i.response.data(), i.response.size());
            if (i.error) {
                close(i.conn_id);
            }
        }
    }

} // namespace soce
} // namespace crpc
