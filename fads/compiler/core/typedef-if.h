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

#ifndef _SOCE_TYPEDEF_IF_CM1orhZrjG_H_
#define _SOCE_TYPEDEF_IF_CM1orhZrjG_H_

#include <sstream>
#include <set>
#include <string>

namespace soce{
namespace compiler{

    class TypeDefIf
    {
    public:
        TypeDefIf(std::string&& type, std::string&& name)
            : type_(std::move(type)), name_(std::move(name))
        {
        }

        inline const std::string& get_type() const {return type_;}
        inline const std::string& get_name() const {return name_;}

        virtual void gen_header(std::ostringstream& oss, size_t indent) = 0;
        virtual void gen_impl(std::ostringstream& oss, size_t indent) = 0;

    protected:
        std::string type_;
        std::string name_;
    };

    class ReferenceIf
    {
    public:
        virtual void gen_header(std::ostringstream& oss, const std::set<std::string>& types) = 0;
    };

} // namespace compiler
} // namespace soce

#endif
