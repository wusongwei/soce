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

#include "utils/str-helper.h"

using std::string;
using std::vector;

namespace soce{
namespace utils{

    int StrHelper::parse_addr(const string& addr, string& ip, string& port)
    {
        size_t pos = addr.find_first_of(':');
        if (pos == string::npos){
            return -1;
        }

        if (pos == addr.size()) {
            return -1;
        }

        ip = addr.substr(0, pos);
        port = addr.substr(pos + 1, addr.size() - pos);

        return 0;
    }

    int StrHelper::split(const string& str, const string& sep, vector<string>& out)
    {
        size_t beg = 0;
        size_t end = str.find(sep);

        out.clear();

        while (end != std::string::npos){
            if (beg == end){
                return -1;
            }

            if (beg != end){
                out.emplace_back(str.c_str() + beg, end - beg);
            }

            beg = end + sep.length();
            end = str.find(sep, beg);
        }

        if (beg == str.size()){
            return -1;
        }else if (beg < str.size()){
            out.emplace_back(str.c_str() + beg);
        }

        return 0;
    }

    void StrHelper::trim(std::string& str)
    {
        size_t pos = 0;
        size_t len = str.size();

        for (size_t i=0; i<len;){
            if (str[i] == ' ' || str[i] == '\t'){
                ++i;
                continue;
            }

            while (i < len && str[i] != ' '){
                str[pos] = str[i];
                ++pos;
                ++i;
            }
        }

        str.resize(pos);
    }

} // namespace utils
} // namespace soce
