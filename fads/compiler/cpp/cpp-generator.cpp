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

#include <time.h>
#include "cpp-generator.h"
#include "../core/utils.h"
#include "../core/fads.h"

using namespace std;

namespace soce{
namespace compiler{

    int CppGenerator::gen_code(const std::string& file)
    {
        file_name_ = Utils::parse_input_file_name(file);
        if (file_name_.empty()) {
            return -1;
        }

        string dir = SOpMgr.get_output_path() + "/gen-cpp";
        if (make_dir(dir)){
            return -1;
        }

        open_file_stream(dir);
        create_header();
        create_implement();

        if (!SSvrCrt.empty()){
            create_service_header();
            create_service_implement();
        }

        return 0;
    }

    void CppGenerator::open_file_stream(const std::string& dir)
    {
        string file_prefix = dir + "/" + file_name_;
        header_file_.open(file_prefix + ".h");
        impl_file_.open(file_prefix + ".cpp");

        if (!SSvrCrt.empty()){
            service_header_file_.open(file_prefix + "-service.h");
            service_impl_file_.open(file_prefix + "-service.cpp");
        }
    }

    void CppGenerator::create_header()
    {
        oss_.clear();
        oss_.str("");

        add_header_protection_begin(file_name_);
        STypeCreater.gen_header(oss_);
        add_ns_end();
        add_header_protection_end();
        header_file_ << oss_.str() << endl;
    }

    void CppGenerator::create_implement()
    {
        oss_.clear();
        oss_.str("");
        oss_ << "#include \"" << file_name_ << ".h\"\n\n";
        add_ns_begin();
        oss_ << "static soce::proto::TypeTree g_type_tree;\n";
        if (!SSvrCrt.empty()){
            oss_ << "static soce::crpc::CrpcReqHeaderTypeTree s_crpc_req_header_type_tree(g_type_tree);\n"
                << "static soce::crpc::CrpcRespHeaderTypeTree s_crpc_resp_header_type_tree(g_type_tree);\n";
        }
        oss_ << "\n";
        STypeCreater.gen_impl(oss_);
        add_ns_end();
        impl_file_ << oss_.str() << endl;
    }

    void CppGenerator::create_service_header()
    {
        oss_.clear();
        oss_.str("");

        add_header_protection_begin(file_name_ + "_service");
        oss_ << "#include \"crpc/base-service.h\"\n"
            "#include \"crpc/client/base-sync-client.h\"\n"
            "#include \"crpc/client/base-cort-client.h\"\n\n";
        add_ns_begin();
        SSvrCrt.gen_header(oss_);
        add_ns_end();
        add_header_protection_end();
        service_header_file_ << oss_.str();
    }

    void CppGenerator::create_service_implement()
    {
        oss_.clear();
        oss_.str("");
        oss_ << "#include \"" << file_name_ << "-service.h\"\n"
          << "#include \"" << file_name_ << ".h\"\n"
          << "#include \"proto/binary-proto.h\"\n\n";

        add_ns_begin();
        SSvrCrt.gen_impl(oss_);
        add_ns_end();
        service_impl_file_ << oss_.str();
    }

    void CppGenerator::add_header_protection_begin(const std::string& name)
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

        oss_ << "#ifndef " << hp.str() << "\n"
            << "#define " << hp.str() << "\n\n";
    }

    void CppGenerator::add_header_protection_end()
    {
        oss_ << "#endif\n";
    }

    void CppGenerator::add_ns_begin()
    {
        vector<string> ns;
        (void)soce::utils::StrHelper::split(SOpMgr.get_namespace(), ".", ns);
        for (auto& i : ns){
            oss_ << "namespace " << i << "{ ";
        }
        oss_ << "\n";
    }

    void CppGenerator::add_ns_end()
    {
        vector<string> ns;
        (void)soce::utils::StrHelper::split(SOpMgr.get_namespace(), ".", ns);
        for (auto& i : ns){
            oss_ << "}";
        }
        oss_ << "//end namespace\n\n";
    }

} // end compiler
} // end soce
