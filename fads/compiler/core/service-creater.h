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

#ifndef _SOCE_SERVICE_CREATER_US7l1ekKWL_H_
#define _SOCE_SERVICE_CREATER_US7l1ekKWL_H_

#include <unordered_map>
#include <vector>
#include <stdint.h>
#include <sstream>
#include "utils/nocopyable.hpp"

namespace soce{
namespace compiler{

    class SCMethod
    {
    public:
        inline void add_param(const std::string& type, const std::string& param){
            params_.push_back(make_pair(type, param));
        }
        inline void set_return_type(const std::string& type){rt_type_ = type;}
        inline const std::string& get_return_type() const {return rt_type_;}
        inline const std::vector<std::pair<std::string, std::string>>& get_params() const {return params_;}

    private:
        std::string rt_type_;
        std::vector<std::pair<std::string, std::string>> params_;
    };

    class SCService
    {
    public:
        SCMethod& add_method(const std::string& name, const std::string& rt_type);
        const std::unordered_map<std::string, SCMethod>& get_methods() const {return methods_;}

    private:
        std::unordered_map<std::string, SCMethod> methods_;
    };

    class ServiceCreater : public soce::utils::NoCopyable
    {
    public:
        SCService& add_service(const std::string& name);
        void gen_header(std::ostringstream& oss);
        void gen_impl(std::ostringstream& oss);

        inline bool empty() const {return services_.empty();}

    private:
        std::unordered_map<std::string, SCService> services_;
    };

    class IServiceCreater
    {
    public:
        virtual void gen_header(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss) = 0;
        virtual void gen_impl(const std::unordered_map<std::string, SCService>& services, std::ostringstream& oss) = 0;
    };

} // namespace compiler
} // namespace soce

#endif
