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

#include "local-req-processor.h"
#include "crpc/log.h"

namespace soce{
namespace crpc{

    void LocalReqProcessor::set_remote_req_processor(std::shared_ptr<RemoteReqProcessor> remote_req_processor)
    {
        remote_req_processor_ = remote_req_processor;
    }

    void LocalReqProcessor::process(uint64_t conn_id, std::string&& request)
    {
        remote_req_processor_->reply(std::move(request));
    }

    void LocalReqProcessor::on_connected(uint64_t conn_id)
    {
    }

    void LocalReqProcessor::on_error(uint64_t conn_id)
    {
    }

} // namespace crpc
} // namespace soce
