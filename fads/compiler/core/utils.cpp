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

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "log4rel/logger.h"
#include "utils.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

namespace soce{
namespace compiler{

    void Utils::version()
    {
        cout << "Fads version 0.0.1" << endl;
    }

    void Utils::usage()
    {
        cerr << "Usage: fadsgen [options] file" << endl;
        cerr << "Use fadsgen -help for a list of options" << endl;
    }

    void Utils::help()
    {
        cerr << "Usage: fadsgen [options] file" << endl
             << "       -help List the help" << endl
             << "       -version Show the version" << endl
             << "       -debug Turn on the debug mode" << endl
             << "       -n Set the namespace" << endl
             << "       -o Set output directory" << endl
             << "       -out Set output directory" << endl;
    }

    bool Utils::directory(const std::string& path)
    {
        struct stat sb;
        if (stat(path.c_str(), &sb) < 0){
            cerr << "path is not director, path = " << path << endl;
            return false;
        }
        if (!S_ISDIR(sb.st_mode)){
            cerr << "path is not director, path = " << path << endl;
            return false;
        }
        return true;
    }

    std::string Utils::parse_input_file_name(const std::string& file)
    {
        string name;
        size_t pos = file.find_last_of('/');
        if (pos == string::npos){
            name = file;
        }
        else{
            name = file.substr(pos + 1);
        }

        if (name.empty() || name.size() < 6){
            SOCE_ERROR << _S("invalid input file", file);
            return string();
        }

        size_t suffix_pos = name.size() - 5;
        if (name.compare(suffix_pos, 5, ".fads")){
            SOCE_ERROR << _S("file", file) << _D("expect suffix .fads");
            return string();
        }

        return name.substr(0, suffix_pos);
    }

    bool Utils::is_primitive_type(const std::string& type)
    {
        return (type == "bool"
                ||type == "int32_t"
                || type == "int64_t"
                || type == "float"
                || type == "double"
                || type == "string"
            ) ? true : false;
    }

    std::string Utils::map_type(const std::string& type)
    {
        if (type == "i32"){
            return "int32_t";
        }
        else if (type == "i64"){
            return "int64_t";
        }

        return type;
    }

    } // namespace compiler
} // namespace soce
