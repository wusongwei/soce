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

#ifndef _SOCE_NODE_IF_2SwWE0E5qn_H_
#define _SOCE_NODE_IF_2SwWE0E5qn_H_

#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include "proto/proto-if.h"

using soce::proto::ProtoIf;
using soce::proto::SoceDataType;

namespace soce{
namespace fadsdb{

    class NodeIf
    {
    public:
        virtual std::shared_ptr<NodeIf> clone() = 0; // clone without value
        virtual SoceDataType get_type() = 0;
        virtual uint32_t serialize(ProtoIf* proto, bool with_type) = 0;
        virtual uint32_t deserialize(ProtoIf* proto, bool with_type) = 0;
        virtual void reset() {has_set_ = false;}
        bool bool_func(const std::string& func, const std::string& rvalue);
        int do_update(const std::string& func, const std::vector<std::string>& params);
        std::shared_ptr<NodeIf> do_select(const std::string& func, const std::vector<std::string>& params);
        inline bool required() {return required_;}
        bool has_set(){return has_set_;}
        virtual void show(){}

        uint32_t read(ProtoIf* proto, SoceDataType type, bool with_type, bool& out);
        uint32_t read(ProtoIf* proto, SoceDataType type, bool with_type, int32_t& out);
        uint32_t read(ProtoIf* proto, SoceDataType type, bool with_type, int64_t& out);
        uint32_t read(ProtoIf* proto, SoceDataType type, bool with_type, float& out);
        uint32_t read(ProtoIf* proto, SoceDataType type, bool with_type, double& out);
        uint32_t read(ProtoIf* proto, SoceDataType type, bool with_type, std::string& out);
        uint32_t write(ProtoIf* proto, SoceDataType type, bool with_type, bool arg);
        uint32_t write(ProtoIf* proto, SoceDataType type, bool with_type, int32_t arg);
        uint32_t write(ProtoIf* proto, SoceDataType type, bool with_type, int64_t arg);
        uint32_t write(ProtoIf* proto, SoceDataType type, bool with_type, float arg);
        uint32_t write(ProtoIf* proto, SoceDataType type, bool with_type, double arg);
        uint32_t write(ProtoIf* proto, SoceDataType type, bool with_type, const std::string& arg);

        using BoolCallback = std::function<bool(const std::string&)>;
        using UpdateCallback = std::function<int(const std::vector<std::string>&)>;
        using SelectCallback = std::function<std::shared_ptr<NodeIf>(const std::vector<std::string>&)>;

    protected:
        std::string type_;
        bool required_ = false;
        bool has_set_ = false;
        std::unordered_map<std::string, BoolCallback> bool_funcs_;
        std::unordered_map<std::string, UpdateCallback> update_funcs_;
        std::unordered_map<std::string, SelectCallback> select_funcs_;
    };

} // namespace fadsdb
} // namespace soce

#endif
