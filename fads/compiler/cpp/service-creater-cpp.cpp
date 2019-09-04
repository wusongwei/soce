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
        gen_cort_client_header(services, oss);
    }

    void ServiceCreaterCpp::gen_impl(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        gen_server_impl(services, oss);
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

    void ServiceCreaterCpp::gen_cort_client_header(const unordered_map<string, SCService>& services, ostringstream& oss)
    {
        for (auto& i : services){
            oss << "class "
                << i.first
                << "CortClient\n{\npublic:\n";

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
                    oss << "int ";
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
                        << "& _return";
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

            oss << indent(0, 1)
                << "void set_timeout(uint32_t timeout){\n"
                << indent(0, 2)
                << "timeout_ = timeout;\n"
                << indent(0, 1)
                << "}\n\n";

            for (auto& j : i.second.get_methods()){
                oss << indent(0, 1)
                    << "void set_"
                    << j.first
                    << "_timeout(uint32_t timeout){\n"
                    << indent(0, 2)
                    << "method_timeout_[\""
                    << j.first
                    << "\"] = timeout;\n"
                    << indent(0, 1)
                    << "}\n\n";
            }

            oss << indent(0, 1)
                << "void set_target_addr(const std::string& addr){\n"
                << indent(0, 2)
                << "target_addr_ = addr;"
                << indent(0, 1)
                << "}\n\nprivate:\n"
                << indent(0, 1)
                << "uint32_t timeout_ = 0;\n"
                << indent(0, 1)
                << "std::unordered_map<std::string, uint32_t> method_timeout_;\n"
                << indent(0, 1)
                << "std::string target_addr_;\n"
                << indent(0, 0)
                << "};\n\n";
        }
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
                    << "std::string rc;\n"
                    << indent(0, 1)
                    << i.first
                    << j.first
                    << "Req req;\n"
                    << indent(0, 1)
                    << "soce::proto::BinaryProto bp;\n"
                    << indent(0, 1)
                    << "bp.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);\n"
                    << indent(0, 1)
                    << "if (req.deserialize((soce::proto::ProtoIf*)&bp) == 0){\n"
                    << indent(0, 2)
                    << "return rc;\n"
                    << indent(0, 1)
                    << "}\n\n";

                if (has_resp){
                    oss << indent(0, 1)
                        << i.first
                        << j.first
                        << "Resp resp;\n"
                        << indent(0, 1)
                        << "resp.mutable_header().set_type(soce::crpc::kRpcTypeResp);\n"
                        << indent(0, 1)
                        << "resp.mutable_header().set_req_id(req.get_header().get_req_id());\n"
                        << indent(0, 1)
                        << "resp.mutable_header().set_tid(req.get_header().get_tid());\n"
                        << indent(0, 1)
                        << "resp.mutable_header().set_status(soce::crpc::kRpcStatusOK);\n";
                }

                oss << indent(0, 1)
                    << "rc = SInterceptor.do_before_itcptor(req.mutable_header(), \""
                    << i.first
                    << "\", \""
                    << j.first
                    << "\", false);\n"
                    << indent(0, 1)
                    << "if (rc.empty()){\n";
                oss << indent(0, 2)
                    << "raw_req_ = (void*)&req;\n"
                    << indent(0, 2);
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
                    << indent(0, 2)
                    << "raw_req_ = NULL;\n";
                if (has_resp){
                    oss << indent(0, 2)
                        << "rc = SInterceptor.do_after_itcptor(resp.mutable_header(), \""
                        << i.first
                        << "\", \""
                        << j.first
                        << "\", false);\n"
                        << indent(0, 2)
                        << "if (rc.empty()){\n"
                        << indent(0, 3)
                        << "soce::proto::BinaryProto out;\n"
                        << indent(0, 3)
                        << "if (resp.serialize((soce::proto::ProtoIf*)&out)){\n"
                        << indent(0, 4)
                        << "rc.assign(out.data(), out.size());\n"
                        << indent(0, 3)
                        << "}\n"
                        << indent(0, 2)
                        << "}\n";
                }
                oss << indent(0, 1)
                    << "}\n";

                if (!has_resp){
                    oss << indent(0, 1)
                        << "rc.clear();\n";
                }
                oss << indent(0, 1)
                    << "return std::move(rc);\n"
                    << indent(0, 0)
                    << "}\n\n";
            }
        }
    }

    void ServiceCreaterCpp::gen_cort_client_impl(const unordered_map<string, SCService>& services, ostringstream& oss)
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

                oss << indent(0, 0);
                if (has_resp){
                    oss << "int ";
                }
                else{
                    oss << "void ";
                }

                oss << i.first
                    << "CortClient::"
                    << j.first
                    << "(";
                bool with_comma = false;
                if (has_return){
                    with_comma = true;
                    oss << j.second.get_return_type()
                        << "& _return";
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
                    << "soce::crpc::WorkerThread* _dthread = soce::crpc::WorkerThread::s_self;\n"
                    << indent(0, 1)
                    << "int64_t _req_id = _dthread->get_reqid();\n"
                    << indent(0, 1)
                    << "int64_t _tid = _dthread->get_tid();\n"
                    << indent(0, 1)
                    << "soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;\n"
                    << indent(0, 1)
                    << i.first
                    << j.first
                    << "Req _req;\n";

                if (j.second.get_return_type() == "condcast"){
                    oss << indent(0, 1)
                        << "_type = soce::crpc::kRpcTypeCondCast;\n";
                }
                else if (j.second.get_return_type() == "uncondcast"){
                    oss << indent(0, 1)
                        << "_type = soce::crpc::kRpcTypeUncondCast;\n";
                }

                oss << indent(0, 1)
                    << "_req.mutable_header().set_type(_type);\n"
                    << indent(0, 1)
                    << "_req.mutable_header().set_service(\""
                    << i.first
                    << "\");\n"
                    << indent(0, 1)
                    << "_req.mutable_header().set_method(\""
                    << j.first
                    << "\");\n"
                    << indent(0, 1)
                    << "_req.mutable_header().set_req_id(_req_id);\n"
                    << indent(0, 1)
                    << "_req.mutable_header().set_tid(_tid);\n"
                    << indent(0, 1)
                    << "if (!target_addr_.empty()){\n"
                    << indent(0, 2)
                    << "_req.mutable_header().set_target_addr(target_addr_);\n"
                    << indent(0, 1)
                    << "}\n";

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

                oss << indent(0, 1)
                    << "std::string rc = SInterceptor.do_before_itcptor(_req.mutable_header(), \""
                    << i.first
                    << "\", \""
                    << j.first
                    << "\", true);\n"
                    << indent(0, 1)
                    << "soce::proto::BinaryProto _bp;\n"
                    << indent(0, 1)
                    << "std::string* resp_str = nullptr;\n"
                    << indent(0, 1)
                    << "(void)resp_str;\n"
                    << indent(0, 1)
                    << "if (!rc.empty()){\n"
                    << indent(0, 2)
                    << "resp_str = &rc;\n"
                    << indent(0, 1)
                    << "}\n"
                    << indent(0, 1)
                    << "else{\n"
                    << indent(0, 2)
                    << "if (_req.serialize((soce::proto::ProtoIf*)&_bp) == 0){\n"
                    << indent(0, 3);

                if (has_resp){
                    oss << "return soce::crpc::kReqProtoError;\n";
                }
                else{
                    oss << "return;\n";
                }

                oss << indent(0, 2)
                    << "}\n"
                    << indent(0, 2)
                    << "_dthread->append_req(_req.get_header(), std::move(std::string(_bp.data(), _bp.size())));\n\n";

                if (has_resp){
                    oss << indent(0, 2)
                        << "uint32_t _req_timeout = timeout_;\n"
                        << indent(0, 2)
                        << "auto _iter = method_timeout_.find(\""
                        << j.first
                        << "\");\n"
                        << indent(0, 2)
                        << "if (_iter != method_timeout_.end()){\n"
                        << indent(0, 3)
                        << "_req_timeout = _iter->second;\n"
                        << indent(0, 2)
                        << "}\n"
                        << indent(0, 2)
                        << "soce::cortengine::CortEngine::CortId _id = SCortEngine.get_cur_cort_id();\n"
                        << indent(0, 2)
                        << "_dthread->add_request(_req_id, _id);\n"
                        << indent(0, 2)
                        << "SCortEngine.yield(_req_timeout);\n"
                        << indent(0, 2)
                        << "if(SCortEngine.is_timeout()){\n"
                        << indent(0, 3)
                        << "_dthread->del_request(_req_id);\n"
                        << indent(0, 3)
                        << "return soce::crpc::kCortTimeout;\n"
                        << indent(0, 2)
                        << "}\n"
                        << indent(0, 2)
                        << "_dthread = soce::crpc::WorkerThread::s_self;\n"
                        << indent(0, 2)
                        << "resp_str = _dthread->get_last_resp();\n"
                        << indent(0, 1)
                        << "}\n"
                        << indent(0, 1)
                        << "if (!resp_str || resp_str->empty()){\n"
                        << indent(0, 2)
                        << "return soce::crpc::kServerNotAvailable;\n"
                        << indent(0, 1)
                        << "}\n"
                        << indent(0, 1)
                        << "if (*resp_str == \"BroadcastOk\"){\n"
                        << indent(0, 2)
                        << "return soce::crpc::kOk;\n"
                        << indent(0, 1)
                        << "}\n"
                        << indent(0, 1)
                        << "if (*resp_str == \"BroadcastError\"){\n"
                        << indent(0, 2)
                        << "return soce::crpc::kBroadcastError;\n"
                        << indent(0, 1)
                        << "}\n"
                        << indent(0, 1)
                        << "_bp.reset();\n"
                        << indent(0, 1)
                        << "_bp.init(const_cast<char*>(resp_str->c_str() + 4), resp_str->size() - 4);\n"
                        << indent(0, 1)
                        << i.first
                        << j.first
                        << "Resp _resp;\n"
                        << indent(0, 1)
                        << "if (_resp.deserialize((soce::proto::ProtoIf*)&_bp) == 0){\n"
                        << indent(0, 2)
                        << "return soce::crpc::kRespProtoError;\n"
                        << indent(0, 1)
                        << "}\n"
                        << indent(0, 1)
                        << "SInterceptor.do_after_itcptor(_resp.mutable_header(), \""
                        << i.first
                        << "\", \""
                        << j.first
                        << "\", true);\n"
                        << indent(0, 1)
                        << "if (_resp.get_header().get_status() != soce::crpc::kRpcStatusOK){\n"
                        << indent(0, 2)
                        << "return _resp.get_header().get_status();\n"
                        << indent(0, 1)
                        << "}\n";

                    if (has_return){
                        oss << indent(0, 1)
                            << "_return = _resp.get_result();\n";
                    }

                    oss << indent(0, 1)
                        << "return soce::crpc::kOk;\n";
                }
                else{
                    oss << indent(0, 1)
                        << "}\n";
                }

                oss << indent(0, 0)
                    << "}\n\n";
            }
        }
    }

} // namespace compiler
} // namespace soce
