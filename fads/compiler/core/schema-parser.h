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

#ifndef _SOCE_SCHEMA_PARSER_qX1BVmLWgf_H_
#define _SOCE_SCHEMA_PARSER_qX1BVmLWgf_H_

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "utils/nocopyable.hpp"
#include "symbol-parser.h"
#include "types-def.h"

namespace soce{
namespace compiler{

    class SchemaParser : public soce::utils::NoCopyable
    {
    public:
        typedef enum{
            kTypeAsStructElem,
            kTypeAsServiceReturnType,
            kTypeAsServiceParam
        }TypeAs;

    public:
        SchemaParser();
        ~SchemaParser() = default;

        int parse_file(const std::string& file);
        int parse_schema(const std::string& schema);
        inline uint32_t get_cur_line() const {
            return symbol_parser_.get_cur_line();
        }

    private:
        typedef std::function<int()> KeywordParser;

    private:
        int parse();
        int parse_include();
        int parse_service();
        int parse_namespace();

        int parse_enum();
        int parse_list();
        int parse_set();
        int parse_map();
        int parse_struct();
        int parse_struct_required(std::shared_ptr<TypeStruct> tstruct);
        int parse_struct_elem(std::shared_ptr<TypeStruct> tstruct, std::string& type, TypeAs tas);
        int parse_primitive_elem(std::shared_ptr<TypeStruct> tstruct, std::string& type, TypeAs tas);
        int parse_custom_elem(std::shared_ptr<TypeStruct> tstruct, std::string& type, TypeAs tas);
        int parse_list_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas);
        int parse_set_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas);
        int parse_map_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas);
        int parse_byte_elem(std::shared_ptr<TypeStruct> tstruct, TypeAs tas);
        void register_keyword_parser(const std::string& keyword, KeywordParser parser);
        void unregister_keyword_parser(const std::string& keyword);
        std::string get_type_as_string(TypeAs tas);

    private:
        SymbolParser symbol_parser_;
        std::unordered_map<std::string, KeywordParser> keyword_parser_;
    };

} // namespace compiler
} // namespace soce

#endif
