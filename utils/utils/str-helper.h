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

#ifndef _SOCE_STR_HELPER_QUvtoNW7RN_H_
#define _SOCE_STR_HELPER_QUvtoNW7RN_H_

#include <string>
#include <cctype>
#include <vector>
#include <algorithm>

namespace soce{
namespace utils{

    class StrHelper
    {
    public:
        static std::string tolower(const std::string& str)
            {
                std::string result = str;
                transform(result.begin(), result.end(), result.begin(), ::tolower);
                return result;
            }

            static void tolower(std::string& str)
            {
                transform(str.begin(), str.end(), str.begin(), ::tolower);
            }

        static std::string to_upper(const std::string& str)
            {
                std::string result = str;
                transform(result.begin(), result.end(), result.begin(), ::toupper);
                return result;
            }

        static void to_upper(std::string& str)
            {
                transform(str.begin(), str.end(), str.begin(), ::toupper);
            }

        static int parse_addr(const std::string& addr, std::string& ip, std::string& port);

        static int split(const std::string& str,
                         const std::string& separator,
                         std::vector<std::string>& out);
        static void trim(std::string& str);
    };
}} // namespace

#endif
