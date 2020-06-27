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

#ifndef _SOCE_FADS_CPP_GENERATOR_2Pfy9rcN8P_H_
#define _SOCE_FADS_CPP_GENERATOR_2Pfy9rcN8P_H_

#include <sstream>
#include <fstream>
#include "../core/code-generator.h"

namespace soce{
namespace compiler{

    class CppGenerator : public CodeGenerator
    {
    public:
        int gen_code(const std::string& file);

    private:
        void open_file_stream(const std::string& dir);
        void create_header();
        void create_implement();
        void create_service_header();
        void create_service_implement();
        void add_header_protection_begin(const std::string& name);
        void add_header_protection_end();
        void add_ns_begin();
        void add_ns_end();

    private:
        std::string file_name_;
        std::ofstream header_file_;
        std::ofstream impl_file_;
        std::ofstream service_header_file_;
        std::ofstream service_impl_file_;
        std::ostringstream oss_;
    };

} // end compiler
} // end soce

#endif
