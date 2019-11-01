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

#include "service-creater-cpp.h"
#include "utils.h"

using std::string;
using std::unordered_map;
using std::ostringstream;

namespace soce{
namespace compiler{

    static string indent(size_t margin, int level)
    {
        return std::move(string(margin + level * Utils::get_indent(), ' '));
    }

    void ServiceCreaterCpp::gen_header(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        gen_server_header(services, oss);
        gen_sync_client_header(services, oss);
        gen_cort_client_header(services, oss);
    }

    void ServiceCreaterCpp::gen_impl(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        gen_server_impl(services, oss);
        gen_sync_client_impl(services, oss);
        gen_cort_client_impl(services, oss);
    }

    void ServiceCreaterCpp::gen_server_header(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        for (auto& i : services){
            oss << "class "
                << i.first
                << "If: public soce::crpc::BaseService\n{\npublic:\n"
                << indent(0, 1)
                << i.first
                << "If(){\n"
                << indent(0, 2)
                << "attrs_.set_name(\""
                << i.first
                << "\");\n"
                << indent(0, 2)
                << "soce::crpc::attr::MethodFilters filter;\n";
            for (auto& j : i.second.get_methods()){
                oss << indent(0, 2)
                    << "attrs_.mutable_methods()[\""
                    << j.first
                    << "\"] = filter;\n";

                oss << indent(0, 2)
                    << "handlers_[\""
                    << j.first
                    << "\"] = std::bind(&"
                    << i.first
                    << "If::"
                    << j.first
                    << "_handler, this, std::placeholders::_1);\n";
            }
            oss << indent(0, 1)
                << "}\n\n";

            for (auto& j : i.second.get_methods()){
                if (j.second.get_return_type() != "uncondcast"){
                    oss << indent(0, 1)
                        << "int set_"
                        << j.first
                        << "_filter(const std::string& filter);\n";
                }

                oss << indent(0, 1)
                    << "std::string "
                    << j.first
                    << "_handler(const std::string& data);\n"
                    << indent(0, 1)
                    << "virtual ";

                if (j.second.get_return_type() == "void"
                    || j.second.get_return_type() == "null"
                    || j.second.get_return_type() == "condcast"
                    || j.second.get_return_type() == "uncondcast"){
                    oss << "void";
                }else {
                    oss << j.second.get_return_type();
                }

                oss << " "
                    << j.first
                    << "(";

                bool with_comma = false;
                bool skip = true;
                for (auto& x : j.second.get_params()){
                    if (skip){
                        skip = false;
                        continue;
                    }

                    if (with_comma){
                        oss << ", ";
                    }
                    else{
                        with_comma = true;
                    }
                    oss << "const " << x.first << "& " << x.second;
                }
                oss << ") = 0;\n\n";
            }
            oss << indent(0, 1)
                << "virtual std::string get_name(){\n"
                << indent(0, 2)
                << "return \""
                << i.first
                << "\";\n"
                << indent(0, 1)
                << "}\n\n";
            oss << "};\n\n";
        }
    }

    void ServiceCreaterCpp::gen_sync_client_header(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        gen_client_header(services, "Sync", oss);
    }

    void ServiceCreaterCpp::gen_cort_client_header(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        gen_client_header(services, "Cort", oss);
    }


    void ServiceCreaterCpp::gen_server_impl(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        oss << "\n";
        for (auto& i : services){
            for (auto& j : i.second.get_methods()){
                bool has_return = true;
                bool has_resp = true;
                if (j.second.get_return_type() == "void"
                    || j.second.get_return_type() == "condcast"
                    || j.second.get_return_type() == "uncondcast"){
                    has_return = false;
                }
                if (j.second.get_return_type() == "null"){
                    has_return = false;
                    has_resp = false;
                }

                if (j.second.get_return_type() != "uncondcast"){
                    oss << "int "
                        << i.first
                        <<"If::set_"
                        << j.first
                        << "_filter(const std::string& filter)\n"
                        << "{\n"
                        << indent(0, 1)
                        << "return add_filter("
                        << i.first
                        << j.first
                        << "Req::get_type_tree(), \""
                        << i.first
                        << "\", \""
                        << j.first
                        << "\", filter);\n"
                        << indent(0, 0)
                        << "}\n\n";
                }

                oss << "std::string "
                    << i.first
                    << "If::"
                    << j.first
                    << "_handler(const std::string& data){\n"
                    << indent(0, 1)
                    << i.first
                    << j.first
                    << "Req req;\n";
                if (has_resp) {
                    oss << indent(0, 1)
                        << i.first
                        << j.first
                        << "Resp resp;\n"
                        << indent(0, 1)
                        << "return handle_request(data, \""
                        << i.first
                        << "\", \""
                        << j.first
                        << "\", &req, req.mutable_header(), &resp, resp.mutable_header(), [&]{\n"
                        << indent(0, 2);
                }else{
                    oss << indent(0, 1)
                        << "return handle_request(data, &req, req.mutable_header(), [&]{\n"
                        << indent(0, 2);
                }

                if (has_return){
                    oss << "resp.mutable_result() = ";
                }

                oss << j.first
                    << "(";

                bool with_comma = false;
                bool skip = true;
                for (auto& x : j.second.get_params()){
                    if (skip){
                        skip = false;
                        continue;
                    }

                    if (with_comma){
                        oss << ", ";
                    }
                    else{
                        with_comma = true;
                    }
                    oss << "req.get_"
                        << x.second
                        << "()";
                }
                oss << ");\n"
                    << indent(0, 1)
                    << "});\n"
                    << indent(0, 0)
                    << "}\n\n";
            }
        }
    }

    void ServiceCreaterCpp::gen_sync_client_impl(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        gen_client_impl(services, "Sync", oss);
    }

    void ServiceCreaterCpp::gen_cort_client_impl(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        gen_client_impl(services, "Cort", oss);
    }

    void ServiceCreaterCpp::gen_client_header(const unordered_map<string, SCService>& services,
                                              const std::string& client_type, ostringstream& oss)
    {
        for (auto& i : services){
            oss << "class "
                << i.first
                << client_type
                << "Client : public soce::crpc::Base"
                << client_type
                << "Client\n{\npublic:\n";

            for (auto& j : i.second.get_methods()){
                bool has_return = true;
                bool has_resp = true;
                if (j.second.get_return_type() == "void"
                    || j.second.get_return_type() == "condcast"
                    || j.second.get_return_type() == "uncondcast"){
                    has_return = false;
                }
                if (j.second.get_return_type() == "null"){
                    has_return = false;
                    has_resp = false;
                }

                oss << indent(0, 1);
                if (has_resp){
                    oss << "soce::crpc::RpcStatus ";
                }
                else{
                    oss << "void ";
                }
                oss << j.first
                    << "(";

                bool with_comma = false;
                if (has_return){
                    with_comma = true;
                    oss << j.second.get_return_type()
                        << "& _result";
                }
                bool skip = true;
                for (auto& x : j.second.get_params()){
                    if (skip){
                        skip = false;
                        continue;
                    }

                    if (with_comma){
                        oss << ", ";
                    }
                    else{
                        with_comma = true;
                    }
                    oss << "const " << x.first << "& " << x.second;
                }
                oss << ");\n";
            }

            for (auto& j : i.second.get_methods()){
                oss << indent(0, 1)
                    << "void set_"
                    << j.first
                    << "_timeout(uint32_t timeout)\n"
                    << indent(0, 1)
                    << "{\n"
                    << indent(0, 2)
                    << "method_timeout_[\""
                    << j.first
                    << "\"] = timeout;\n"
                    << indent(0, 1)
                    << "}\n\n";
            }

            oss << "};\n\n";
        }
    }

    void ServiceCreaterCpp::gen_client_impl(const unordered_map<string, SCService>& services, const std::string& client_type, ostringstream& oss)
    {
        oss << "\n";
        for (auto& i : services){
            for (auto& j : i.second.get_methods()){
                bool has_result = true;
                bool has_resp = true;
                if (j.second.get_return_type() == "void"
                    || j.second.get_return_type() == "condcast"
                    || j.second.get_return_type() == "uncondcast"){
                    has_result = false;
                }
                if (j.second.get_return_type() == "null"){
                    has_result = false;
                    has_resp = false;
                }

                oss << indent(0, 0);
                if (has_resp){
                    oss << "soce::crpc::RpcStatus ";
                }
                else{
                    oss << "void ";
                }

                oss << i.first
                    << client_type
                    << "Client::"
                    << j.first
                    << "(";
                bool with_comma = false;
                if (has_result){
                    with_comma = true;
                    oss << j.second.get_return_type()
                        << "& _result";
                }
                bool skip = true;
                for (auto& x : j.second.get_params()){
                    if (skip){
                        skip = false;
                        continue;
                    }

                    if (with_comma){
                        oss << ", ";
                    }
                    else{
                        with_comma = true;
                    }
                    oss << "const " << x.first << "& " << x.second;
                }

                oss << ")\n{\n"
                    << indent(0, 1)
                    << "int64_t _req_id = 0;\n"
                    << indent(0, 1)
                    << "soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;\n"
                    << indent(0, 1)
                    << i.first
                    << j.first
                    << "Req _req;\n";
                if (has_resp) {
                    oss << indent(0, 1)
                        << i.first
                        << j.first
                        << "Resp _resp;\n";
                }

                if (j.second.get_return_type() == "null"){
                    oss << indent(0, 1)
                        << "_type = soce::crpc::kRpcTypeNullReq;\n";
                }
                else if (j.second.get_return_type() == "condcast"){
                    oss << indent(0, 1)
                        << "_type = soce::crpc::kRpcTypeCondCast;\n";
                }
                else if (j.second.get_return_type() == "uncondcast"){
                    oss << indent(0, 1)
                        << "_type = soce::crpc::kRpcTypeUncondCast;\n";
                }

                skip = true;
                for (auto& x : j.second.get_params()){
                    if (skip){
                        skip = false;
                        continue;
                    }
                    oss << indent(0, 1)
                        << "_req.set_"
                        << x.second
                        << "("
                        << x.second
                        << ");\n";
                }

                if (has_resp){
                    oss << indent(0, 1)
                        << "soce::crpc::RpcStatus status = do_req(\""
                        << i.first
                        << "\", \""
                        << j.first
                        << "\", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);\n"
                        << indent(0, 1)
                        << "if (status != soce::crpc::kRpcStatusOK){\n"
                        << indent(0, 2)
                        << "return status;\n"
                        << indent(0, 1)
                        << "}\n\n";


                    if (has_result){
                        oss << indent(0, 1)
                            << "_result = _resp.get_result();\n";
                    }

                    oss << indent(0, 1)
                        << "return soce::crpc::kRpcStatusOK;\n";
                }
                else{
                    oss << indent(0, 1)
                        << "do_req(\""
                        << i.first
                        << "\", \""
                        << j.first
                        << "\", _type, _req_id, _req.mutable_header(), &_req);\n";
                }

                oss << indent(0, 0)
                    << "}\n\n";
            }
        }
    }

} // namespace compiler
} // namespace soce
