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

#include "token-getter.h"

using std::string;

namespace soce{
namespace crpc{

    void TokenGetter::set_data(const char* data, size_t len)
    {
        data_ = data;
        len_ = len;
    }

    string TokenGetter::get_token()
    {
        skip_space();
        if (end()){
            return string();
        }

        size_t beg = pos_;

        if (data_[pos_] == '('){
            ++pos_;
            return "(";
        }
        else if (data_[pos_] == ')'){
            ++pos_;
            return ")";
        }

        while (pos_ < len_){
            if (data_[pos_] == ' '
                || data_[pos_] == '\t'
                || data_[pos_] == '('
                || data_[pos_] == ')'){
                break;
            }
            else{
                ++pos_;
            }
        }

        return std::move(string(data_ + beg, pos_ - beg));
    }

    bool TokenGetter::end()
    {
        skip_space();
        return (pos_ >= len_) ? true : false;
    }

    void TokenGetter::skip_space()
    {
        while (pos_ < len_){
            if (data_[pos_] == ' ' || data_[pos_] == '\t'){
                ++pos_;
            }
            else{
                break;
            }
        }
    }

} // namespace crpc
} // namespace soce
