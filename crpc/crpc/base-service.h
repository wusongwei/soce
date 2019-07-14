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

#ifndef _SOCE_BASE_SERVICE_TwLokTqkCJ_H_
#define _SOCE_BASE_SERVICE_TwLokTqkCJ_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "service-if.h"
#include "gen-cpp/server-attr.h"
#include "proto/type-tree.h"
#include "cort-engine/cort-engine.h"
#include "crpc/interceptor.h"
#include "crpc/worker-thread.h"

namespace soce{
namespace crpc{

    enum Status
    {
        kOk = 0,
        kReqProtoError,
        kRespProtoError,
        kCortTimeout,
        kServerNotAvailable,
        kUnknown
    };

    class BaseService : public ServiceIf
    {
    public:
        virtual std::string process(const std::string& data);
        int add_filter(soce::proto::TypeTree* type_tree,
                       const std::string& service,
                       const std::string& method,
                       const std::string& filter);
        virtual std::string get_service_info();

    protected:
        inline void* get_raw_req(){return raw_req_;}

    protected:
        soce::crpc::attr::ServerAttrs attrs_;
        std::unordered_map<std::string, std::function<std::string(const std::string&)>> handlers_;
        std::vector<uint32_t> method_index_{0, 2};
        std::vector<uint32_t> reqid_index_{0, 3};
        void* raw_req_ = NULL;
    };

} // namespace crpc
} // namespace soce

#endif
