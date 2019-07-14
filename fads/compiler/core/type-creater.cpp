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

#include <cassert>
#include "type-factory.hpp"
#include "utils.h"
#include "schema-parser.h"
#include "log4rel/logger.h"

using std::string;

namespace soce{
namespace compiler{

    int TypeCreater::add_type(std::shared_ptr<TypeDefIf> type)
    {
        assert(type);

        const string& name = type->get_name();
        if (types_.find(name)){
            SOCE_FATAL << _N("line", SSchemaParser.get_cur_line())
                       << _S("dup element", name);

            return -1;
        }

        types_.push_back(name, type);
        add_complex_type(type->get_type());

        return 0;
    }

    bool TypeCreater::find_type(const string& type) const
    {
        if (Utils::is_primitive_type(type)){
            return true;
        }

        return types_.find(type);
    }

    string TypeCreater::get_type_nature(const string& type)
    {
        string rc;
        if (type == "bool"
            || type == "int32_t"
            || type == "int64_t"
            || type == "float"
            || type == "double"
            || type == "string"
            || type == "byte"
            || type == "list"
            || type == "set"
            || type == "map"){
            rc = type;
        }
        else if(type == "soce::crpc::CrpcReqHeader"
                || type == "soce::crpc::CrpcRespHeader"){
            rc = "struct";
        }
        else{
            std::shared_ptr<TypeDefIf>* p = NULL;
            if (types_.get(type, &p)){
                SOCE_ERROR << _N("line", SSchemaParser.get_cur_line()) << _S("invalid", type);
            }
            else{
                rc = (*p)->get_type();
            }
        }

        return move(rc);
    }

    void TypeCreater::gen_header(std::ostringstream& oss) const
    {
        FactoryRef::create()->gen_header(oss, complex_type_);

        for (auto iter = types_.begin(); iter != types_.end(); ++iter){
            iter->second->gen_header(oss, 0);
        }
    }

    void TypeCreater::gen_impl(std::ostringstream& oss) const
    {
        for (auto iter = types_.begin(); iter != types_.end(); ++iter){
            iter->second->gen_impl(oss, 0);
        }
    }

} // namespace compiler
} // namespace soce
