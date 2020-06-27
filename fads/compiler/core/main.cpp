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


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "log4rel/logger.h"
#include "../cpp/cpp-generator.h"
#include "utils.h"
#include "fads.h"

using namespace std;
using namespace soce::compiler;


static int parse_option(int argc, char* argv[])
{
    if (argc < 2){
        Utils::usage();
        return -1;
    }

    SOCE_CUR_LOGGER->set_log_level(soce::log4rel::kLogLevelInfo);

    for (int i=1; i<=argc - 1; i++){
        char* arg = argv[i];
        if (strlen(arg) <= 1){
            Utils::usage();
            exit(0);
        }

        if (arg[0] == '-' && arg[1] == '-') {
            ++arg;
        }

        if (strcmp(arg, "-help") == 0) {
            Utils::help();
            exit(0);
        }
        else if (strcmp(arg, "-version") == 0) {
            Utils::version();
            exit(0);
        }
        else if (strcmp(arg, "-debug") == 0) {
            SOCE_CUR_LOGGER->set_log_level(soce::log4rel::kLogLevelAll);
        }
        else if (strcmp(arg, "-n") == 0){
            arg = argv[++i];
            SOpMgr.set_namespace(arg);
        }
        else if (strcmp(arg, "-lang") == 0) {
            arg = argv[++i];
            if (strcmp(arg, "cpp")!=0 && strcmp(arg, "java")!=0) {
                std::cerr << "not supported language : " << arg;
                exit(1);
            }
            SOpMgr.set_lang(arg);
        }
        else if ((strcmp(arg, "-o") == 0) || (strcmp(arg, "-out") == 0)) {
            arg = argv[++i];

            if (!Utils::directory(arg)){
                return -1;
            }
            SOpMgr.set_output_path(arg);
        }

    }

    return 0;
}

int main(int argc, char* argv[])
{
    SOCE_CUR_LOGGER->reserve_field(soce::log4rel::kLogFieldLevel, true);

    if (parse_option(argc, argv)){
        return -1;
    }

    string file = argv[argc - 1];
    if (SSchemaParser.parse_file(file)){
        return -1;
    }

    std::shared_ptr<CodeGenerator> cg;
    string lang = SOpMgr.get_lang();
    if (lang == "cpp") {
        cg.reset(new CppGenerator);
    }
    else if (lang == "java") {
        //cg.reset(new JavaGenerator);
    }
    else{
        cerr << "Not supported language : " << lang << endl;
        exit(1);
    }

    return cg->gen_code(file);
}
