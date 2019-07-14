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

#include <regex>
#include "node-primary.h"
#include "proto/soce-proto.h"
#include "log4rel/logger.h"

using soce::proto::ProtoIf;
using soce::proto::SoceProto;

namespace soce{
namespace fadsdb{

#define DEF_NODE_IMPL(N)                                                \
    std::shared_ptr<NodeIf> Node##N::clone()                            \
    {                                                                   \
        std::shared_ptr<Node##N> node(new Node##N);                     \
        return node;                                                    \
    }                                                                   \
                                                                        \
    uint32_t Node##N::serialize(ProtoIf* proto, bool with_type)         \
    {                                                                   \
        return write(proto, SoceDataType::kType##N, with_type, val_);   \
    }                                                                   \
                                                                        \
    uint32_t Node##N::deserialize(ProtoIf* proto, bool with_type)       \
    {                                                                   \
        uint32_t size = read(proto,                                     \
                             SoceDataType::kType##N,                    \
                             with_type,                                 \
                             val_);                                     \
                                                                        \
        uint32_t type_size = with_type ? 1 : 0;                         \
        has_set_ = (size > type_size) ? true : false;                   \
        return size;                                                    \
    }                                                                   \

    DEF_NODE_IMPL(Bool)
    DEF_NODE_IMPL(Int32)
    DEF_NODE_IMPL(Int64)
    DEF_NODE_IMPL(Float)
    DEF_NODE_IMPL(Double)
    DEF_NODE_IMPL(String)
#undef DEF_NODE_IMPL

    NodeBool::NodeBool()
    {
        bool_funcs_["=="] = std::bind(&NodeBool::eq, this, std::placeholders::_1);
        bool_funcs_["!="] = std::bind(&NodeBool::neq, this, std::placeholders::_1);

        update_funcs_["set"] = std::bind(&NodeBool::set, this, std::placeholders::_1);
        update_funcs_["unset"] = std::bind(&NodeBool::unset, this, std::placeholders::_1);
    }

    bool NodeBool::eq(const std::string& val)
    {
        bool b = (val == "true") ? true : false;
        return (val_ == b) ? true : false;
    }

    bool NodeBool::neq(const std::string& val)
    {
        bool b = (val == "true") ? true : false;
        return (val_ != b) ? true : false;
    }

    bool NodeBool::gt(const std::string& val)
    {
        (void) val;
        return false;
    }

    bool NodeBool::egt(const std::string& val)
    {
        (void) val;
        return false;
    }

    bool NodeBool::lt(const std::string& val)
    {
        (void) val;
        return false;
    }

    bool NodeBool::elt(const std::string& val)
    {
        (void) val;
        return false;
    }

    int NodeBool::set(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (params.size() != 1
            || (params.front() != "true" && params.front() != "false")){
            rc = -1;
        }
        else{
            val_ = (params.front() == "true") ? true : false;
            has_set_ = true;
        }

        return rc;
    }

    int NodeBool::unset(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (!params.empty() || !has_set_){
            rc = -1;
        }
        else{
            val_ = false;
            has_set_ = false;
        }

        return rc;
    }

    NodeInt32::NodeInt32()
    {
        bool_funcs_["=="] = std::bind(&NodeInt32::eq, this, std::placeholders::_1);
        bool_funcs_["!="] = std::bind(&NodeInt32::neq, this, std::placeholders::_1);
        bool_funcs_[">"] = std::bind(&NodeInt32::gt, this, std::placeholders::_1);
        bool_funcs_[">="] = std::bind(&NodeInt32::egt, this, std::placeholders::_1);
        bool_funcs_["<"] = std::bind(&NodeInt32::lt, this, std::placeholders::_1);
        bool_funcs_["<="] = std::bind(&NodeInt32::elt, this, std::placeholders::_1);

        update_funcs_["set"] = std::bind(&NodeInt32::set, this, std::placeholders::_1);
        update_funcs_["unset"] = std::bind(&NodeInt32::unset, this, std::placeholders::_1);
    }

    bool NodeInt32::eq(const std::string& val)
    {
        return (val_ == std::stoi(val)) ? true : false;
    }

    bool NodeInt32::neq(const std::string& val)
    {
        return (val_ != std::stoi(val)) ? true : false;
    }

    bool NodeInt32::gt(const std::string& val)
    {
        return (val_ > std::stoi(val)) ? true : false;
    }

    bool NodeInt32::egt(const std::string& val)
    {
        return (val_ >= std::stoi(val)) ? true : false;
    }

    bool NodeInt32::lt(const std::string& val)
    {
        return (val_ < std::stoi(val)) ? true : false;
    }

    bool NodeInt32::elt(const std::string& val)
    {
        return (val_ <= std::stoi(val)) ? true : false;
    }

    int NodeInt32::set(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (params.size() != 1){
            rc = -1;
        }
        else{
            val_ = std::stoi(params.front());
            has_set_ = true;
        }

        return rc;
    }

    int NodeInt32::unset(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (!params.empty() || !has_set_){
            rc = -1;
        }
        else{
            val_ = 0;
            has_set_ = false;
        }

        return rc;
    }

    NodeInt64::NodeInt64()
    {
        bool_funcs_["=="] = std::bind(&NodeInt64::eq, this, std::placeholders::_1);
        bool_funcs_["!="] = std::bind(&NodeInt64::neq, this, std::placeholders::_1);
        bool_funcs_[">"] = std::bind(&NodeInt64::gt, this, std::placeholders::_1);
        bool_funcs_[">="] = std::bind(&NodeInt64::egt, this, std::placeholders::_1);
        bool_funcs_["<"] = std::bind(&NodeInt64::lt, this, std::placeholders::_1);
        bool_funcs_["<="] = std::bind(&NodeInt64::elt, this, std::placeholders::_1);

        update_funcs_["set"] = std::bind(&NodeInt64::set, this, std::placeholders::_1);
        update_funcs_["unset"] = std::bind(&NodeInt64::unset, this, std::placeholders::_1);
    }

    bool NodeInt64::eq(const std::string& val)
    {
        return (val_ == std::stoll(val)) ? true : false;
    }

    bool NodeInt64::neq(const std::string& val)
    {
        return (val_ != std::stoll(val)) ? true : false;
    }

    bool NodeInt64::gt(const std::string& val)
    {
        return (val_ > std::stoll(val)) ? true : false;
    }

    bool NodeInt64::egt(const std::string& val)
    {
        return (val_ >= std::stoll(val)) ? true : false;
    }

    bool NodeInt64::lt(const std::string& val)
    {
        return (val_ < std::stoll(val)) ? true : false;
    }

    bool NodeInt64::elt(const std::string& val)
    {
        return (val_ <= std::stoll(val)) ? true : false;
    }

    int NodeInt64::set(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (params.size() != 1){
            rc = -1;
        }
        else{
            val_ = std::stoll(params.front());
            has_set_ = true;
        }

        return rc;
    }

    int NodeInt64::unset(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (!params.empty() || !has_set_){
            rc = -1;
        }
        else{
            val_ = 0;
            has_set_ = false;
        }

        return rc;
    }

    NodeFloat::NodeFloat()
    {
        bool_funcs_["=="] = std::bind(&NodeFloat::eq, this, std::placeholders::_1);
        bool_funcs_["!="] = std::bind(&NodeFloat::neq, this, std::placeholders::_1);
        bool_funcs_[">"] = std::bind(&NodeFloat::gt, this, std::placeholders::_1);
        bool_funcs_[">="] = std::bind(&NodeFloat::egt, this, std::placeholders::_1);
        bool_funcs_["<"] = std::bind(&NodeFloat::lt, this, std::placeholders::_1);
        bool_funcs_["<="] = std::bind(&NodeFloat::elt, this, std::placeholders::_1);

        update_funcs_["set"] = std::bind(&NodeFloat::set, this, std::placeholders::_1);
        update_funcs_["unset"] = std::bind(&NodeFloat::unset, this, std::placeholders::_1);
    }

    bool NodeFloat::eq(const std::string& val)
    {
        return (val_ == std::stof(val)) ? true : false;
    }

    bool NodeFloat::neq(const std::string& val)
    {
        return (val_ != std::stof(val)) ? true : false;
    }

    bool NodeFloat::gt(const std::string& val)
    {
        return (val_ > std::stof(val)) ? true : false;
    }

    bool NodeFloat::egt(const std::string& val)
    {
        return (val_ >= std::stof(val)) ? true : false;
    }

    bool NodeFloat::lt(const std::string& val)
    {
        return (val_ < std::stof(val)) ? true : false;
    }

    bool NodeFloat::elt(const std::string& val)
    {
        return (val_ <= std::stof(val)) ? true : false;
    }

    int NodeFloat::set(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (params.size() != 1){
            rc = -1;
        }
        else{
            val_ = std::stof(params.front());
            has_set_ = true;
        }

        return rc;
    }

    int NodeFloat::unset(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (!params.empty() || !has_set_){
            rc = -1;
        }
        else{
            val_ = 0;
            has_set_ = false;
        }

        return rc;
    }

    NodeDouble::NodeDouble()
    {
        bool_funcs_["=="] = std::bind(&NodeDouble::eq, this, std::placeholders::_1);
        bool_funcs_["!="] = std::bind(&NodeDouble::neq, this, std::placeholders::_1);
        bool_funcs_[">"] = std::bind(&NodeDouble::gt, this, std::placeholders::_1);
        bool_funcs_[">="] = std::bind(&NodeDouble::egt, this, std::placeholders::_1);
        bool_funcs_["<"] = std::bind(&NodeDouble::lt, this, std::placeholders::_1);
        bool_funcs_["<="] = std::bind(&NodeDouble::elt, this, std::placeholders::_1);

        update_funcs_["set"] = std::bind(&NodeDouble::set, this, std::placeholders::_1);
        update_funcs_["set"] = std::bind(&NodeDouble::unset, this, std::placeholders::_1);
    }

    bool NodeDouble::eq(const std::string& val)
    {
        return (val_ == std::stod(val)) ? true : false;
    }

    bool NodeDouble::neq(const std::string& val)
    {
        return (val_ != std::stod(val)) ? true : false;
    }

    bool NodeDouble::gt(const std::string& val)
    {
        return (val_ > std::stod(val)) ? true : false;
    }

    bool NodeDouble::egt(const std::string& val)
    {
        return (val_ >= std::stod(val)) ? true : false;
    }

    bool NodeDouble::lt(const std::string& val)
    {
        return (val_ < std::stod(val)) ? true : false;
    }

    bool NodeDouble::elt(const std::string& val)
    {
        return (val_ <= std::stod(val)) ? true : false;
    }

    int NodeDouble::set(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (params.size() != 1){
            rc = -1;
        }
        else{
            val_ = std::stod(params.front());
            has_set_ = true;
        }

        return rc;
    }

    int NodeDouble::unset(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (!params.empty() || !has_set_){
            rc = -1;
        }
        else{
            val_ = 0;
            has_set_ = false;
        }

        return rc;
    }

    NodeString::NodeString()
    {
        bool_funcs_["=="] = std::bind(&NodeString::eq, this, std::placeholders::_1);
        bool_funcs_["!="] = std::bind(&NodeString::neq, this, std::placeholders::_1);
        bool_funcs_[">"] = std::bind(&NodeString::gt, this, std::placeholders::_1);
        bool_funcs_[">="] = std::bind(&NodeString::egt, this, std::placeholders::_1);
        bool_funcs_["<"] = std::bind(&NodeString::lt, this, std::placeholders::_1);
        bool_funcs_["<="] = std::bind(&NodeString::elt, this, std::placeholders::_1);
        bool_funcs_["match"] = std::bind(&NodeString::match, this, std::placeholders::_1);

        update_funcs_["set"] = std::bind(&NodeString::set, this, std::placeholders::_1);
        update_funcs_["unset"] = std::bind(&NodeString::unset, this, std::placeholders::_1);

        select_funcs_["size"] = std::bind(&NodeString::select_size, this, std::placeholders::_1);
    }

    bool NodeString::eq(const std::string& val)
    {
        return (val_ == val) ? true : false;
    }

    bool NodeString::neq(const std::string& val)
    {
        return (val_ != val) ? true : false;
    }

    bool NodeString::gt(const std::string& val)
    {
        return (val_ > val) ? true : false;
    }

    bool NodeString::egt(const std::string& val)
    {
        return (val_ >= val) ? true : false;
    }

    bool NodeString::lt(const std::string& val)
    {
        return (val_ < val) ? true : false;
    }

    bool NodeString::elt(const std::string& val)
    {
        return (val_ <= val) ? true : false;
    }

    bool NodeString::match(const std::string& str)
    {
        bool rc = false;
        try{
            rc = regex_match(val_, std::regex(str));
        }
        catch(...){
            rc = false;
        }

        return rc;
    }

    int NodeString::set(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (params.size() != 1){
            rc = -1;
        }
        else{
            val_ = params.front();
            has_set_ = true;
        }

        return rc;
    }

    int NodeString::unset(const std::vector<std::string>& params)
    {
        int rc = 0;
        if (!params.empty() || !has_set_){
            rc = -1;
        }
        else{
            val_.clear();
            has_set_ = false;
        }

        return rc;
    }

    std::shared_ptr<NodeIf> NodeString::select_size(const std::vector<std::string>& params)
    {
        std::shared_ptr<NodeInt32> node;
        if (params.empty()){
            node.reset(new NodeInt32);
            node->set_val(val_.size());
        }

        return node;
    }

} // namespace fadsdb
} // namespace soce
