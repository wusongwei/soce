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

#include <fstream>
#include "symbol-parser.h"
#include "log4rel/logger.h"

using std::string;

namespace soce {
namespace compiler{

    int SymbolParser::add_file(const std::string& file)
    {
        string ctx;
        if (load(file, ctx)){
            return -1;
        }

        if (!ctx.empty()){
            info_.emplace(std::move(ctx));
            end_ = false;
        }

        return 0;
    }

    int SymbolParser::add_schema(const std::string& schema)
    {
        if (schema.empty()){
            SOCE_FATAL << _N("line", get_cur_line()) << _D("invalid schema");
            return -1;
        }

        info_.emplace(schema);
        end_ = false;

        SOCE_DEBUG << _S("schema", schema);
        return 0;
    }

    void SymbolParser::pop()
    {
        info_.pop();
        if (info_.empty()){
            end_ = true;
        }
    }

    SymbolParser& SymbolParser::get_token(std::string& token)
    {
        if (error_){
            return *this;
        }

        do{
            skip_space();
            if (end()){
                SOCE_FATAL << _N("line", get_cur_line()) << _D("failed to get a token");
                break;
            }

            SchemaInfo& info = info_.top();
            if (!isalpha(info.ctx[info.pos])){
                SOCE_FATAL << _N("line", get_cur_line()) << _D("invalid token, begin without alpha");
                break;
            }

            size_t pos = info.pos;
            while (info.pos < info.len
                   && (isalnum(info.ctx[info.pos])
                       || info.ctx[info.pos] == '_')){
                ++info.pos;
            }

            if (pos == info.pos){
                SOCE_FATAL << _N("line", get_cur_line()) << _D("empty token");
                break;
            }

            token = info.ctx.substr(pos, info.pos - pos);
            SOCE_DEBUG << _S("get_token", token);
            return *this;
        }while(0);

        error_ = true;
        return *this;
    }

    SymbolParser& SymbolParser::get_str(std::string& str, std::string sep)
    {
        if (error_){
            return *this;
        }

        do{
            skip_space();
            if (end()){
                SOCE_FATAL << _N("line", get_cur_line()) << _D("failed to get a string") << _S("seperator", sep);
                break;
            }

            SchemaInfo& info = info_.top();
            size_t pos = info.pos;

            while (info.pos < info.len
                   && !isspace(info.ctx[info.pos])
                   && [&]{
                       for (auto& i : sep){
                           if (i == info.ctx[info.pos]){
                               return false;
                           }
                       }
                       return true;
                   }()){
                ++info.pos;
            }

            if (pos == info.pos){
                SOCE_FATAL << _N("line", get_cur_line()) << _D("empty string");
                break;
            }


            str = info.ctx.substr(pos, info.pos - pos);
            SOCE_DEBUG << _S("get_str", str);
            return *this;
        }while(0);

        error_ = true;
        return *this;
    }

    SymbolParser& SymbolParser::get_sep(char& sep, bool forword)
    {
        if (error_){
            return *this;
        }

        do{
            skip_space();
            if (end()){
                SOCE_FATAL << _N("line", get_cur_line()) << _D("failed to get seperator");
                break;
            }

            SchemaInfo& info = info_.top();
            sep = info.ctx[info.pos];
            if (isalnum(sep)){
                sep = ' ';
            }
            else if (forword){
                ++info.pos;
            }

            SOCE_DEBUG << _S("get_seperator", sep);
            return *this;
        }while(0);

        error_ = true;
        return *this;
    }

    SymbolParser& SymbolParser::skip_sep(char sep)
    {
        if (error_){
            return *this;
        }

        char c;
        if (get_sep(c).error() || c != sep){
            SOCE_FATAL << _N("line", get_cur_line()) << _S("invalid sep", c) << _S("expect", sep);
            error_ = true;
        }

        SOCE_DEBUG << _S("skip seperator", sep);
        return *this;
    }

    bool SymbolParser::error() const
    {
        return error_;
    }

    bool SymbolParser::end()
    {
        skip_space();
        return end_;
    }

    size_t SymbolParser::get_cur_line() const
    {
        if (info_.empty()){
            return 0;
        }

        return info_.top().line + 1;
    }

    int SymbolParser::load(const std::string& file, std::string& schema)
    {
        int rc = 0;
        std::ifstream is(file.c_str());
        if (!is){
            SOCE_FATAL << _N("line", get_cur_line()) << _S("failed to load file", file);
            rc = -1;
        }
        else{
            is.seekg (0, is.end);
            int length = is.tellg();
            is.seekg (0, is.beg);

            schema.resize(length);
            is.read((char*)schema.c_str(),length);
            is.close();
        }

        SOCE_DEBUG << _S("load file", file);
        return rc;
    }

    void SymbolParser::skip_space()
    {
        if (end_){
            return;
        }

        do{
            SchemaInfo& info = info_.top();
            for (; info.pos < info.len;){
                if (!isspace(info.ctx[info.pos]) && info.ctx[info.pos] != '#'){
                    break;
                }

                if (info.ctx[info.pos] == '\n'){
                    ++info.line;
                }
                else if (info.ctx[info.pos] == '#'){
                    while (info.pos < info.len && info.ctx[info.pos] != '\n'){
                        ++info.pos;
                    }
                    ++info.line;
                }
                ++info.pos;
            }

            if (info.pos >= info.len){
                pop();
                if (end_){
                    break;
                }
            }
            else{
                break;
            }
        }while (true);
    }

} // namespace compiler
} // namespace soce
