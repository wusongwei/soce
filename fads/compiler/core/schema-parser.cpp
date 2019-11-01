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

#include <algorithm>
#include <cctype>
#include <vector>
#include <utility>
#include "schema-parser.h"
#include "log4rel/logger.h"
#include "utils/str-helper.h"
#include "type-factory.hpp"
#include "utils.h"

using std::string;
using std::vector;
using std::pair;

namespace soce{
namespace compiler{

    SchemaParser::SchemaParser()
    {
        register_keyword_parser("enum", std::bind(&SchemaParser::parse_enum, this));
        register_keyword_parser("list", std::bind(&SchemaParser::parse_list, this));
        register_keyword_parser("set", std::bind(&SchemaParser::parse_set, this));
        register_keyword_parser("map", std::bind(&SchemaParser::parse_map, this));
        register_keyword_parser("struct", std::bind(&SchemaParser::parse_struct, this));
        register_keyword_parser("include", std::bind(&SchemaParser::parse_include, this));
        register_keyword_parser("namespace", std::bind(&SchemaParser::parse_namespace, this));
        register_keyword_parser("table", std::bind(&SchemaParser::parse_struct, this));
        register_keyword_parser("service", std::bind(&SchemaParser::parse_service, this));
    }

    int SchemaParser::parse_file(const std::string& file)
    {
        if (symbol_parser_.add_file(file)){
            return -1;
        }

        return parse();
    }

    int SchemaParser::parse_schema(const std::string& schema)
    {
        if (symbol_parser_.add_schema(schema)){
            return -1;
        }

        return parse();
    }

    int SchemaParser::parse()
    {
        string keyword;

        while (!symbol_parser_.end()){
            if (symbol_parser_.get_token(keyword).error()){
                return -1;
            }

            soce::utils::StrHelper::tolower(keyword);
            auto iter = keyword_parser_.find(keyword);
            if (iter == keyword_parser_.end()){
                SOCE_FATAL << _N("line", get_cur_line()) << _S("invalid keyword", keyword);
                return -1;
            }

            if (iter->second()){
                return -1;
            }
        }

        return 0;
    }

    int SchemaParser::parse_enum()
    {
        string name;
        if (symbol_parser_.get_token(name).skip_sep('{').error()){
            return -1;
        }

        bool go = true;
        vector<pair<string, string>> elems;

        do {
            string key;
            string val;
            char sep;

            if (symbol_parser_.get_token(key).get_sep(sep).error()){
                return -1;
            }

            if (sep == '='){
                if (symbol_parser_.get_str(val, ",}").get_sep(sep).error()){
                    return -1;
                }
            }

            switch (sep){
            case ',':
                break;

            case '}':
                go = false;
                break;

            default:
                SOCE_FATAL << _N("line", get_cur_line())
                           << _S("enum name", name)
                           << _S("invalid sep", sep)
                           << _S("expect", "',' or '}'");
                return -1;
            }

            elems.push_back(std::make_pair(std::move(key), std::move(val)));
        }while (go);

        std::shared_ptr<TypeDefIf> ptype = FactoryEnum::create(std::move(name), std::move(elems));
        return STypeCreater.add_type(ptype);
    }

    int SchemaParser::parse_list()
    {
        string name;
        string elem_type;

        if (symbol_parser_.get_token(name)
            .skip_sep('{')
            .get_token(elem_type)
            .skip_sep('}')
            .error()){
            return -1;
        }

        elem_type = Utils::map_type(elem_type);
        if (!STypeCreater.find_type(elem_type)){
            SOCE_FATAL << _N("line", get_cur_line())
                       << _S("list name", name)
                       << _S("invalid type", elem_type);
            return -1;
        }
        std::shared_ptr<TypeDefIf> ptype = FactoryList::create(std::move(name), std::move(elem_type));
        return STypeCreater.add_type(ptype);
    }

    int SchemaParser::parse_set()
    {
        string name;
        string elem_type;

        if (symbol_parser_.get_token(name)
            .skip_sep('{')
            .get_token(elem_type)
            .skip_sep('}')
            .error()){
            return -1;
        }

        elem_type = Utils::map_type(elem_type);
        if (!STypeCreater.find_type(elem_type)){
            SOCE_FATAL << _N("line", get_cur_line())
                       << _S("set name", name)
                       << _S("invalid type", elem_type);
            return -1;
        }
        std::shared_ptr<TypeDefIf> ptype = FactorySet::create(std::move(name), std::move(elem_type));
        return STypeCreater.add_type(ptype);
    }

    int SchemaParser::parse_map()
    {
        string name;
        string key_t;
        string val_t;

        if (symbol_parser_.get_token(name)
            .skip_sep('{')
            .get_token(key_t)
            .skip_sep(',')
            .get_token(val_t)
            .skip_sep('}')
            .error()){
            return -1;
        }

        key_t = Utils::map_type(key_t);
        val_t = Utils::map_type(val_t);

        if (!STypeCreater.find_type(key_t)){
            SOCE_FATAL << _N("line", get_cur_line())
                       << _S("map name", name)
                       << _S("invalid key type", key_t);
            return -1;
        }

        if (!STypeCreater.find_type(val_t)){
            SOCE_FATAL << _N("line", get_cur_line())
                       << _S("map name", name)
                       << _S("invalid val type", val_t);
            return -1;
        }

        std::shared_ptr<TypeDefIf> ptype = FactoryMap::create(std::move(name), std::move(key_t), std::move(val_t));
        return STypeCreater.add_type(ptype);
    }

    int SchemaParser::parse_struct()
    {
        string name;
        char sep;

        if (symbol_parser_.get_token(name).get_sep(sep).error()){
            return -1;
        }
        if (sep != '{'){
            SOCE_FATAL << _N("line", get_cur_line())
                       << _S("struct name", name)
                       << _S("separator", sep)
                       << _S("expect", sep);
            return -1;
        }

        std::shared_ptr<TypeStruct> tstruct = FactoryStruct::create(std::move(name));
        bool go = true;
        string type;

        do{
            if (symbol_parser_.get_token(type).error()){
                return -1;
            }

            if (type == "require"){
                if (parse_struct_required(tstruct)){
                    return -1;
                }
            }
            else if(parse_struct_elem(tstruct, type, kTypeAsStructElem)){
                return -1;
            }

            if (symbol_parser_.get_sep(sep).error()){
                return -1;
            }

            switch (sep){
            case ' ':
                break;

            case '}':
                go = false;
                break;

            default:
                SOCE_FATAL << _N("line", get_cur_line())
                           << _S("separator", sep)
                           << _S("expect", "' ' or '}'");
                return -1;
            }

        }while(go);

        return STypeCreater.add_type(tstruct);
    }

    int SchemaParser::parse_struct_required(std::shared_ptr<TypeStruct> tstruct)
    {
        char sep;
        if (symbol_parser_.skip_sep('{').error()){
            return -1;
        }

        string elem;
        bool go = true;
        do{
            if (symbol_parser_.get_token(elem).get_sep(sep).error()){
                return -1;
            }

            tstruct->add_required_attr(elem);
            switch(sep){
            case ',':
                break;

            case '}':
                go = false;
                break;

            default:
                SOCE_FATAL << _N("line", get_cur_line())
                           << _S("separator", sep)
                           << _S("xpect", "',' or '}'");

                return -1;
            }
        }while (go);

        return 0;
    }

    int SchemaParser::parse_struct_elem(std::shared_ptr<TypeStruct> tstruct,
                                        std::string& type,
                                        TypeAs tas)
    {
        type = Utils::map_type(type);

        int rc = -1;
        if (Utils::is_primitive_type(type)){
            rc = parse_primitive_elem(tstruct, type, tas);
        }
        else if (type == "list"){
            rc = parse_list_elem(tstruct, tas);
        }
        else if (type == "set"){
            rc = parse_set_elem(tstruct, tas);
        }
        else if (type == "map"){
            rc = parse_map_elem(tstruct, tas);
        }
        else if (type == "byte"){
            rc = parse_byte_elem(tstruct, tas);
        }
        else if (STypeCreater.find_type(type)){
            rc = parse_custom_elem(tstruct, type, tas);
        }
        else{
            SOCE_FATAL << _N("line", get_cur_line())
                       << _S("invalid type", type);
        }
        return rc;
    }

    int SchemaParser::parse_primitive_elem(std::shared_ptr<TypeStruct> tstruct,
                                           std::string& type,
                                           TypeAs tas)
    {
        string name;
        char sep;

        switch (tas){
        case kTypeAsStructElem:
            if (symbol_parser_.get_token(name).skip_sep(';').error()){
                return -1;
            }
            break;

        case kTypeAsServiceReturnType:
            name = "result";
            break;

        case kTypeAsServiceParam:
            if (symbol_parser_.get_token(name).error()){
                return -1;
            }
            break;

        default:
            SOCE_FATAL << _N("line", get_cur_line())
                       << _S("invalid type as", tas);
            return -1;
        }

        SOCE_DEBUG << _S("Type", type)
                   << _S("Name", name)
                   << _S("As", get_type_as_string(tas));

        std::vector<string> ext;
        return tstruct->add_elem(std::move(type),
                                 std::move(name),
                                 false, std::move(ext));
    }

    int SchemaParser::parse_byte_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas)
    {
        char sep;
        string size;
        string name;

        if (symbol_parser_.skip_sep('[')
            .get_str(size, "]")
            .skip_sep(']')
            .error()){
            return -1;
        }

        switch (tas){
        case kTypeAsStructElem:
            if (symbol_parser_.get_token(name).skip_sep(';').error()){
                return -1;
            }
            break;

        case kTypeAsServiceReturnType:
            name = "result";
            break;

        case kTypeAsServiceParam:
            if (symbol_parser_.get_token(name).error()){
                return -1;
            }
            break;

        default:
            return -1;
        }

        SOCE_DEBUG << _S("Type", "byte")
                   << _S("Name", name)
                   << _S("Size", size)
                   << _S("As", get_type_as_string(tas));

        for (size_t i=0; i<size.size(); ++i){
            if (!isdigit(size[i])){
                SOCE_FATAL << _N("Line", get_cur_line())
                           << _S("Invalid byte size", size);
                return -1;
            }
        }

        std::vector<string> ext;
        ext.push_back(size);
        return tstruct->add_elem("byte",
                                 std::move(name),
                                 false,
                                 std::move(ext));
    }

    int SchemaParser::parse_custom_elem(std::shared_ptr<TypeStruct> tstruct, std::string& type, TypeAs tas)
    {
        string name;
        char sep;

        switch (tas){
        case kTypeAsStructElem:
            if (symbol_parser_.get_token(name).get_sep(sep).error()){
                return -1;
            }

            if (sep != ';'){
                SOCE_FATAL << _N("line", get_cur_line())
                           << _S("invalid separator", sep);
                return -1;
            }

            break;

        case kTypeAsServiceReturnType:
            name = "result";
            break;

        case kTypeAsServiceParam:
            if (symbol_parser_.get_token(name).error()){
                return -1;
            }
            break;

        default:
            return -1;
        }

        SOCE_DEBUG << _S("Type", type)
                   << _S("Name", name)
                   << _S("As", get_type_as_string(tas));

        std::vector<string> ext;
        return tstruct->add_elem(std::move(type),
                                 std::move(name),
                                 false,
                                 std::move(ext));
    }

    int SchemaParser::parse_list_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas)
    {
        string name;
        string elem_type;
        std::vector<string> ext;
        char sep;

        if (symbol_parser_.skip_sep('<')
            .get_token(elem_type)
            .skip_sep('>')
            .error()){
            return -1;
        }
        elem_type = Utils::map_type(elem_type);
        ext.push_back(elem_type);

        switch (tas){
        case kTypeAsStructElem:
            if (symbol_parser_.get_token(name).skip_sep(';').error()){
                return -1;
            }
            break;

        case kTypeAsServiceReturnType:
            name = "result";
            break;

        case kTypeAsServiceParam:
            if (symbol_parser_.get_token(name).error()){
                return -1;
            }
            break;

        default:
            return -1;
        }

        SOCE_DEBUG << _S("Type", "list")
                   << _S("Name", name)
                   << _S("Elem type", elem_type)
                   << _S("As", get_type_as_string(tas));
        return tstruct->add_elem("list",
                                 std::move(name),
                                 false,
                                 std::move(ext));
    }

    int SchemaParser::parse_set_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas)
    {
        string name;
        string elem_type;
        std::vector<string> ext;
        char sep;

        if (symbol_parser_.skip_sep('<')
            .get_token(elem_type)
            .skip_sep('>')
            .error()){
            return -1;
        }
        elem_type = Utils::map_type(elem_type);
        ext.push_back(elem_type);

        switch (tas){
        case kTypeAsStructElem:
            if (symbol_parser_.get_token(name).skip_sep(';').error()){
                return -1;
            }
            break;

        case kTypeAsServiceReturnType:
            name = "result";
            break;

        case kTypeAsServiceParam:
            if (symbol_parser_.get_token(name).error()){
                return -1;
            }
            break;
        }

        SOCE_DEBUG << _S("Type", "set")
                   << _S("Name", name)
                   << _S("Elem type", elem_type)
                   << _S("As", get_type_as_string(tas));
        return tstruct->add_elem("set",
                                 std::move(name),
                                 false,
                                 std::move(ext));
    }

    int SchemaParser::parse_map_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas)
    {
        string name;
        string key_type;
        string val_type;
        std::vector<string> ext;
        char sep;

        if (symbol_parser_.skip_sep('<')
            .get_token(key_type)
            .skip_sep(',')
            .get_token(val_type)
            .skip_sep('>')
            .error()){
            return -1;
        }

        key_type = Utils::map_type(key_type);
        val_type = Utils::map_type(val_type);

        ext.push_back(key_type);
        ext.push_back(val_type);

        switch (tas){
        case kTypeAsStructElem:
            if (symbol_parser_.get_token(name).skip_sep(';').error()){
                SOCE_FATAL << _N("line", get_cur_line());
                return -1;
            }
            break;

        case kTypeAsServiceReturnType:
            name = "result";
            break;

        case kTypeAsServiceParam:
            if (symbol_parser_.get_token(name).error()){
                return -1;
            }
            break;

        default:
            return -1;
        }

        SOCE_DEBUG << _S("Type", "map")
                   << _S("Name", name)
                   << _S("Key type", key_type)
                   << _S("Val type", val_type)
                   << _S("As", get_type_as_string(tas));
        return tstruct->add_elem("map",
                                 std::move(name),
                                 false,
                                 std::move(ext));
    }

    int SchemaParser::parse_include()
    {
        string file;
        if (symbol_parser_.get_str(file).error()
            || symbol_parser_.add_file(file)){
            return -1;
        }

        return 0;
    }

    int SchemaParser::parse_namespace()
    {
        string ns;
        if (symbol_parser_.get_str(ns).error()){
            return -1;
        }

        SOpMgr.set_namespace(ns);
        return 0;
    }

    int SchemaParser::parse_service()
    {
        std::vector<string> ext;
        string name;
        if (symbol_parser_.get_token(name).skip_sep('{').error()){
            return -1;
        }

        SOCE_DEBUG << _S("Service", name);

        bool go = true;
        do{
            string rc_type;
            string method;
            char sep;

            string resp_name;
            std::shared_ptr<TypeStruct> ts_resp = FactoryStruct::create(std::move(resp_name));
            ts_resp->add_elem("soce::crpc::CrpcRespHeader",
                              "header",
                              false,
                              std::move(ext));

            if (symbol_parser_.get_token(rc_type).error()){
                return -1;
            }
            SOCE_DEBUG << _S("Type of result", rc_type);

            if (rc_type != "void"
                && rc_type != "null"
                && rc_type != "condcast"
                && rc_type != "uncondcast"){
                if (parse_struct_elem(ts_resp, rc_type, kTypeAsServiceReturnType)){
                    return -1;
                }
            }

            if (symbol_parser_.get_token(method).skip_sep('(').error()){
                return -1;
            }
            SOCE_DEBUG << _S("Method", method);
            if (rc_type != "null"){
                resp_name = name + method + "Resp";
                ts_resp->set_name(resp_name);
                if (STypeCreater.add_type(ts_resp)){
                    return -1;
                }
            }

            if (rc_type != "void"
                && rc_type != "null"
                && rc_type != "condcast"
                && rc_type != "uncondcast"){
                auto iter = ts_resp->begin();
                ++iter;
                rc_type = iter->second->format_type();
            }
            string req_name = name + method + "Req";
            std::shared_ptr<TypeStruct> ts_req = FactoryStruct::create(std::move(req_name));
            ts_req->add_elem("soce::crpc::CrpcReqHeader",
                             "header",
                             false,
                             std::move(ext));
            SCService& sc_service = SSvrCrt.add_service(name);
            SCMethod& sc_method = sc_service.add_method(method, rc_type);
            if (symbol_parser_.get_sep(sep, false).error()){
                return -1;
            }

            if (sep == ')'){
                if (symbol_parser_.skip_sep(')').skip_sep(';') .error()){
                    return -1;
                }
            }
            else{
                bool end = false;
                do{
                    string type;
                    if (symbol_parser_.get_token(type).error()){
                        return -1;
                    }

                    if (parse_struct_elem(ts_req, type, kTypeAsServiceParam)){
                        return -1;
                    }

                    if (symbol_parser_.get_sep(sep).error()){
                        return -1;
                    }

                    switch (sep){
                    case ',':
                        break;

                    case ')':
                        if (symbol_parser_.skip_sep(';').error()){
                            return -1;
                        }
                        end = true;
                        break;

                    default:
                        SOCE_FATAL << _N("line", get_cur_line())
                                   << _S("Invalid sep", sep)
                                   << _S("Expect", "',' or ')'");
                        return -1;
                    }
                }while(!end);
            }

            if (STypeCreater.add_type(ts_req)){
                return -1;
            }

            for (auto iter = ts_req->begin();
                 iter != ts_req->end(); ++iter){
                sc_method.add_param(iter->second->format_type(), iter->second->get_name());
            }


            if (symbol_parser_.get_sep(sep, false).error() || sep == '}'){
                go = false;
                if (sep == '}'){
                    symbol_parser_.skip_sep('}');
                }
            }

        }while(go);

        return 0;
    }

    void SchemaParser::register_keyword_parser(const std::string& keyword, KeywordParser parser)
    {
        keyword_parser_[keyword] = parser;
    }

    void SchemaParser::unregister_keyword_parser(const std::string& keyword)
    {
        keyword_parser_.erase(keyword);
    }

    std::string SchemaParser::get_type_as_string(TypeAs tas)
    {
        string type_str;

        switch (tas){
        case kTypeAsStructElem:
            type_str = "Struct element";
            break;

        case kTypeAsServiceReturnType:
            type_str = "Return type of service";
            break;

        case kTypeAsServiceParam:
            type_str = "Parameter of service";
            break;

        default:
            break;
        }

        return std::move(type_str);
    }

} // namespace compiler
} // namespace soce
