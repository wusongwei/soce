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

#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fads.h"
#include "log4rel/logger.h"
#include "log4rel/plain-sink.h"
#include "utils.h"

using namespace std;
using namespace soce::compiler;

static void add_header_protection_begin(const std::string& name, std::ostringstream& oss)
{
    string nstr = name;
    size_t len = nstr.size();
    for (size_t i=0; i<len; ++i){
        if (nstr[i] == '-'){
            nstr[i] = '_';
        }
    }
    std::ostringstream hp;
    hp << "_" << nstr << "_";
    srand(time(NULL));
    hp << std::to_string(rand()) << "_H_";

    oss << "#ifndef " << hp.str() << "\n"
        << "#define " << hp.str() << "\n\n";
}

static void add_header_protection_end(std::ostringstream& oss)
{
    oss << "#endif\n";
}

static void add_ns_begin(std::ostringstream& oss)
{
    vector<string> ns;
    (void)soce::utils::StrHelper::split(SOpMgr.get_namespace(), ".", ns);
    for (auto& i : ns){
        oss << "namespace " << i << "{ ";
    }
    oss << "\n";
}

static void add_ns_end(std::ostringstream& oss)
{
    vector<string> ns;
    (void)soce::utils::StrHelper::split(SOpMgr.get_namespace(), ".", ns);
    for (auto& i : ns){
        oss << "}";
    }
    oss << "//end namespace\n\n";
}


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

    string name = Utils::parse_input_file_name(argv[argc-1]);
    if (name.empty()){
        return -1;
    }

    if (SSchemaParser.parse_file(argv[argc-1])){
        return -1;
    }

    string gen_dir = SOpMgr.get_output_path();
    struct stat s_buf;
    gen_dir += "/gen-cpp";
    stat(gen_dir.c_str(), &s_buf);
    if (!S_ISDIR(s_buf.st_mode)){
        if (mkdir(gen_dir.c_str(), 0755)){
            SOCE_ERROR << _S("mkdir", gen_dir);
            return -1;
        }
    }

    string header_file_name = gen_dir + "/" + name + ".h";
    ofstream header_file;
    header_file.open(header_file_name);

    string impl_file_name = gen_dir + "/" + name + ".cpp";
    ofstream impl_file;
    impl_file.open(impl_file_name);

    ostringstream oss;
    add_header_protection_begin(name, oss);
    STypeCreater.gen_header(oss);
    add_ns_end(oss);
    add_header_protection_end(oss);
    header_file << oss.str() << endl;

    oss.clear();
    oss.str("");
    oss << "#include \"" << name << ".h\"\n\n";
    add_ns_begin(oss);
    oss << "static soce::proto::TypeTree g_type_tree;\n";
    if (!SSvrCrt.empty()){
        oss << "static soce::crpc::CrpcReqHeaderTypeTree s_crpc_req_header_type_tree(g_type_tree);\n"
            << "static soce::crpc::CrpcRespHeaderTypeTree s_crpc_resp_header_type_tree(g_type_tree);\n";
    }
    oss << "\n";
    STypeCreater.gen_impl(oss);
    add_ns_end(oss);
    impl_file << oss.str() << endl;

    if (!SSvrCrt.empty()){
        oss.clear();
        oss.str("");
        SSvrCrt.gen_header(oss);

        string service_header_file_name = gen_dir + "/" + name + "-service.h";
        ofstream service_header_file;
        service_header_file.open(service_header_file_name);
        ostringstream s;
        add_header_protection_begin(name + "_service", s);
        s << "#include \"crpc/base-service.h\"\n\n";
        add_ns_begin(s);
        add_ns_end(oss);
        add_header_protection_end(oss);

        service_header_file << s.str() << endl;;
        service_header_file << oss.str();

        oss.clear();
        oss.str("");
        SSvrCrt.gen_impl(oss);
        string service_impl_file_name = gen_dir + "/" + name + "-service.cpp";
        ofstream service_impl_file;
        s.clear();
        s.str("");
        s << "#include \"" << name << "-service.h\"\n"
          << "#include \"" << name << ".h\"\n"
          << "#include \"proto/binary-proto.h\"\n\n";

        add_ns_begin(s);
        add_ns_end(oss);
        service_impl_file.open(service_impl_file_name);
        service_impl_file << s.str();
        service_impl_file << oss.str();
    }

    return 0;
}
