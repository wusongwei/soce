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

#include "service-creater-cpp.h"
#include "type-factory.hpp"

using std::string;
using std::ostringstream;

namespace soce{
namespace compiler{

    SCMethod& SCService::add_method(const string& name, const string& type)
    {
        SCMethod& method = methods_[name];
        method.set_return_type(type);
        return method;
    }

    SCService& ServiceCreater::add_service(const string& name)
    {
        return services_[name];
    }

    void ServiceCreater::gen_header(ostringstream& oss)
    {
        FactoryService::create()->gen_header(services_, oss);
    }

    void ServiceCreater::gen_impl(ostringstream& oss)
    {
        FactoryService::create()->gen_impl(services_, oss);
    }

} // namespace compiler
} // namespace soce
