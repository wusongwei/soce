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

#ifndef _SOCE_UTILS_BIKCsm5eMY_H_
#define _SOCE_UTILS_BIKCsm5eMY_H_

#include <string>
#include "utils/nocopyable.hpp"

namespace soce {
namespace compiler{

    class Utils : public soce::utils::NoCopyable
    {
    public:
        Utils() = delete;
        ~Utils() = delete;

        static void version();
        static void usage();
        static void help();
        static bool directory(const std::string& path);
        static std::string parse_input_file_name(const std::string& file);
        static const size_t get_indent() {return 4;}
        static bool is_primitive_type(const std::string& type) ;
        static std::string map_type (const std::string& type);
    };

} // namespace compiler
} // namespace soce

#endif
