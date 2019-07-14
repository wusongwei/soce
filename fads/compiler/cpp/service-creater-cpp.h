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

#ifndef _SOCE_SERVICE_CREATER_CPP_szJaQG6hPG_H_
#define _SOCE_SERVICE_CREATER_CPP_szJaQG6hPG_H_

#include "service-creater.h"

namespace soce{
namespace compiler{

    class ServiceCreaterCpp : public IServiceCreater
    {
    public:
        virtual void gen_header(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss);
        virtual void gen_impl(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss);

    private:
        void gen_server_header(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss);
        void gen_cort_client_header(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss);
        void gen_server_impl(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss);
        void gen_cort_client_impl(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss);
    };

} // namespace compiler
} // namespace soce
#endif
