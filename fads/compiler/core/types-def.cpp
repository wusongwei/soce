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

#include <memory>
#include "schema-parser.h"
#include "types-def.h"
#include "type-factory.hpp"
#include "utils.h"
#include "log4rel/logger.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

namespace soce{
namespace compiler{

    int TypeStruct::add_elem(string&& type,
                             string&& name,
                             bool required,
                             vector<string>&& ext)
    {
        if (elems_.find(name)){
            SOCE_FATAL << _N("line", SSchemaParser.get_cur_line()) << _S("dup elem", name);
            return -1;
        }

        std::shared_ptr<TypeStructElemBase> elem;
        if (Utils::is_primitive_type(type)){
            elem = FactoryStructElemPrime::create(std::move(type), std::move(name), required);
        }
        else if (type == "list"){
            if (ext.empty() || STypeCreater.get_type_nature(ext.front()).empty()){
                return -1;
            }
            elem = FactoryStructElemList::create(std::move(type), std::move(name), required, std::move(ext));
            STypeCreater.add_complex_type("vector");
        }
        else if (type == "set"){
            if (ext.empty() || STypeCreater.get_type_nature(ext.front()).empty()){
                return -1;
            }
            elem = FactoryStructElemSet::create(std::move(type), std::move(name), required, std::move(ext));
            STypeCreater.add_complex_type("set");
        }
        else if (type == "map"){
            if (ext.size() < 2
                || STypeCreater.get_type_nature(ext.front()).empty()
                || STypeCreater.get_type_nature(ext.back()).empty()){
                return -1;
            }
            elem = FactoryStructElemMap::create(std::move(type), std::move(name), required, std::move(ext));
            STypeCreater.add_complex_type("map");
        }
        else if (type == "byte"){
            elem = FactoryStructElemByte::create(std::move(type), std::move(name), required, std::move(ext));
        }
        else{
            elem = FactoryStructElemCustom::create(std::move(type), std::move(name), required);
        }

        elems_.push_back(elem->get_name(), elem);
        return 0;
    }

    void TypeStruct::add_required_attr(const string& name)
    {
        std::shared_ptr<TypeStructElemBase>* p = NULL;
        if (elems_.get(name, &p)){
            SOCE_WARN << _S("unknown elem", name) << _S("struct", name_);
            return;
        }

        (*p)->required();
    }

    void TypeStruct::add_service_header()
    {
        string type = "SoceServiceHeader";
        string name = "header";
        std::shared_ptr<TypeStructElemBase> elem;

        elem = FactoryStructElemCustom::create(std::move(type), std::move(string(name)), true);
        elems_.push_back(elem->get_name(), elem);
    }

} // namespace compiler
} // namespace soce
