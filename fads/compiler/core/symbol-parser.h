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

#ifndef _SOCE_SCHEMA_PARSER_SVOuHvKjDo_H_
#define _SOCE_SCHEMA_PARSER_SVOuHvKjDo_H_

#include <string>
#include <stdint.h>
#include <stack>
#include <ctype.h>
#include "utils/nocopyable.hpp"

namespace soce{
namespace compiler{

    class SymbolParser : public soce::utils::NoCopyable
    {
    public:
        SymbolParser() = default;
        ~SymbolParser() = default;

        int add_file(const std::string& file);
        int add_schema(const std::string& schema);
        SymbolParser& get_token(std::string& token);
        SymbolParser& get_str(std::string& str, std::string sep = "");
        SymbolParser& get_sep(char& sep, bool forward = true);
        SymbolParser& skip_sep(char sep);
        bool error() const;
        bool end();
        size_t get_cur_line() const;

    private:
        int load(const std::string& file, std::string& schema);
        void skip_space();
        void pop();

    private:
        struct SchemaInfo
        {
        SchemaInfo(std::string&& _ctx):ctx(std::move(_ctx)) {len = ctx.size();}
        SchemaInfo(const std::string& _ctx):ctx(_ctx) {len = ctx.size();}

            std::string ctx;
            size_t pos = 0;
            size_t len = 0;
            uint32_t line = 0;
        };

    private:
        std::stack<SchemaInfo> info_;
        bool end_ = true;
        bool error_ = false;
    };

} // namespace compiler
} // namespace soce

#endif
