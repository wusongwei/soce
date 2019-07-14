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

#ifndef _SOCE_TYPE_CREATER_wLfMHWeAD6_H_
#define _SOCE_TYPE_CREATER_wLfMHWeAD6_H_

#include <stdint.h>
#include <memory>
#include <vector>
#include <set>
#include <sstream>
#include "utils/nocopyable.hpp"
#include "typedef-if.h"
#include "utils/index-list.hpp"

namespace soce{
namespace compiler{

    class TypeCreater : public soce::utils::NoCopyable
    {
    public:
        int add_type(std::shared_ptr<TypeDefIf> type);
        bool find_type(const std::string& type) const;
        std::string get_type_nature(const std::string& type);
        void gen_header(std::ostringstream& oss) const;
        void gen_impl(std::ostringstream& oss) const;

        void add_complex_type(const std::string& type){
            complex_type_.insert(type);
        }

    private:
        void gen_reference(std::ostringstream& oss) const;

    private:
        soce::utils::IndexList<std::string, std::shared_ptr<TypeDefIf>> types_;
        std::set<std::string> complex_type_;
    };

} // namespace compiler
} // namespace soce

#endif
