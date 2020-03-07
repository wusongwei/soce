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

#ifndef _SOCE_CRPC_PROXY_LOCAL_REQ_PROCESSOR_LJmGbLQbYd_H_
#define _SOCE_CRPC_PROXY_LOCAL_REQ_PROCESSOR_LJmGbLQbYd_H_

#include <memory>
#include "crpc/base-processor.h"
#include "remote-req-processor.h"

namespace soce{
namespace crpc{

    class LocalReqProcessor : public BaseProcessor
    {
    public:
        void set_remote_req_processor(std::shared_ptr<RemoteReqProcessor> remote_req_processor);
        virtual void process(uint64_t conn_id, std::string&& data);
        virtual void on_connected(uint64_t conn_id);
        virtual void on_error(uint64_t conn_id);

    private:
        std::shared_ptr<RemoteReqProcessor> remote_req_processor_;
    };

} // namespace crpc
} // namespace soce

#endif
