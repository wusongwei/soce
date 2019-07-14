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

#include "token-parser.h"

namespace soce{
namespace fadsdb{

    TokenParser::TokenParser()
    {
    }

    TokenParser::TokenParser(const char* data, size_t len)
        : data_(data), len_(len)
    {
    }

    void TokenParser::set(const char* data, size_t len)
    {
        data_ = data;
        len_ = len;
    }

    int TokenParser::get_token(const std::string& sep, std::string& out)
    {
        skip_space();
        if (end()){
            return -1;
        }

        size_t pos = offset_;
        while (offset_ < len_){
            if (sep.find_first_of(data_[offset_]) != std::string::npos
                || data_[offset_] == ' '
                || data_[offset_] == '\t'){
                break;
            }

            ++offset_;
        }

        if (pos == offset_){
            return -1;
        }

        out.assign(data_ + pos, offset_ - pos);
        return 0;
    }

    int TokenParser::get_str(std::string& out)
    {
        size_t beg = offset_;
        int quotes = 0;

        while (offset_ < len_){
            if (data_[offset_] == '\''){
                if (offset_ + 1 >= len_ || data_[offset_ + 1] != '\''){
                    break;
                }
                ++offset_;
                ++quotes;
            }
            ++offset_;
        }

        if (offset_ >= len_ || offset_ == beg){
            return -1;
        }

        out.assign(data_ + beg, offset_ - beg);
        if (quotes){
            size_t i = 0;
            size_t j = 0;
            size_t len = out.size();
            while (j < len){
                out[i] = out[j];
                if (out[j] == '\'' && j + 1 < len && out[j+1] == '\''){
                    ++j;
                }
                ++i;
                ++j;
            }

            out.resize(len - quotes);
        }

        ++offset_;
        return 0;
    }

    int TokenParser::skip_sep(char sep)
    {
        skip_space();
        if (end()){
            return -1;
        }

        if (data_[offset_] != sep){
            return -1;
        }
        ++offset_;
        return 0;
    }

    void TokenParser::skip_space()
    {
        while (offset_ < len_){
            if (data_[offset_] == ' ' || data_[offset_] == '\t'){
                ++offset_;
            }
            else{
                break;
            }
        }
    }

    bool TokenParser::end()
    {
        skip_space();
        return (offset_ >= len_) ? true : false;
    }

} // namespace fadsdb
} // namespace soce
