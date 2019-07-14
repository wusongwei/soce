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

#include "node-if.h"
#include "proto/soce-proto.h"
#include "plugin.h"

using soce::proto::SoceProto;

namespace soce{
namespace fadsdb{

    bool NodeIf::bool_func(const std::string& func, const std::string& rvalue)
    {
        auto iter = bool_funcs_.find(func);
        if (iter == bool_funcs_.end()){
            return false;
        }

        return iter->second(rvalue);
    }

    int NodeIf::do_update(const std::string& func, const std::vector<std::string>& params)
    {
        int rc = -1;
        UpdatePlugin plugin;
        if (SPlugin.get_update_func(get_type(), func, plugin) == 0){
            rc = plugin(params, this);
        }
        else{
            auto iter = update_funcs_.find(func);
            if (iter != update_funcs_.end()){
                rc = iter->second(params);
            }
        }

        return rc;
    }

    std::shared_ptr<NodeIf> NodeIf::do_select(const std::string& func, const std::vector<std::string>& params)
    {
        std::shared_ptr<NodeIf> node;
        SelectPlugin plugin;
        if (SPlugin.get_select_func(get_type(), func, plugin) == 0){
            node = plugin(params, this);
        }
        else{
            auto iter = select_funcs_.find(func);
            if (iter != select_funcs_.end()){
                node = iter->second(params);
            }
        }

        return node;
    }

    uint32_t NodeIf::read(ProtoIf* proto, SoceDataType type, bool with_type, bool& out)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.read_bool(proto, required_, out);
        }
        else{
            size = proto->read_bool(out);
        }

        return size;
    }

    uint32_t NodeIf::read(ProtoIf* proto, SoceDataType type, bool with_type, int32_t& out)
    {
        SoceProto sproto;
        uint32_t size = 0;
        if (type == SoceDataType::kTypeInt32){
            if (with_type){
                size = sproto.read_int32(proto, required_, out);
            }
            else{
                size = proto->read_int32(out);
            }
        }
        else if (type == SoceDataType::kTypeEnum){
            if (with_type){
                size = sproto.read_enum(proto, required_, out);
            }
            else{
                size = proto->read_enum(out);
            }
        }
        return size;
    }

    uint32_t NodeIf::read(ProtoIf* proto, SoceDataType type, bool with_type, int64_t& out)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.read_int64(proto, required_, out);
        }
        else{
            size = proto->read_int64(out);
        }
        return size;
    }

    uint32_t NodeIf::read(ProtoIf* proto, SoceDataType type, bool with_type, float& out)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.read_float(proto, required_, out);
        }
        else{
            size = proto->read_float(out);
        }
        return size;
    }

    uint32_t NodeIf::read(ProtoIf* proto, SoceDataType type, bool with_type, double& out)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.read_double(proto, required_, out);
        }
        else{
            size = proto->read_double(out);
        }
        return size;
    }

    uint32_t NodeIf::read(ProtoIf* proto, SoceDataType type, bool with_type, std::string& out)
    {
        (void) type;
        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.read_string(proto, required_, out);
        }
        else{
            size = proto->read_string(out);
        }
        return size;
    }

    uint32_t NodeIf::write(ProtoIf* proto, SoceDataType type, bool with_type, bool arg)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.write_bool(proto, required_, has_set_, arg);
        }
        else{
            size = proto->write_bool(arg);
        }
        return size;
    }

    uint32_t NodeIf::write(ProtoIf* proto, SoceDataType type, bool with_type, int32_t arg)
    {
        SoceProto sproto;
        uint32_t size = 0;
        if (type == SoceDataType::kTypeInt32){
            if (with_type){
                size = sproto.write_int32(proto, required_, has_set_, arg);
            }
            else{
                size = proto->write_int32(arg);
            }
        }
        else if (type == SoceDataType::kTypeEnum){
            if (with_type){
                size = sproto.write_enum(proto, required_, has_set_, arg);
            }
            else{
                size = proto->write_enum(arg);
            }
        }

        return size;
    }

    uint32_t NodeIf::write(ProtoIf* proto, SoceDataType type, bool with_type, int64_t arg)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.write_int64(proto, required_, has_set_, arg);
        }
        else{
            size = proto->write_int64(arg);
        }
        return size;
    }

    uint32_t NodeIf::write(ProtoIf* proto, SoceDataType type, bool with_type, float arg)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.write_float(proto, required_, has_set_, arg);
        }
        else{
            size = proto->write_float(arg);
        }
        return size;
    }

    uint32_t NodeIf::write(ProtoIf* proto, SoceDataType type, bool with_type, double arg)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.write_double(proto, required_, has_set_, arg);
        }
        else{
            size = proto->write_double(arg);
        }
        return size;
    }

    uint32_t NodeIf::write(ProtoIf* proto, SoceDataType type, bool with_type, const std::string& arg)
    {
        (void) type;

        uint32_t size = 0;
        if (with_type){
            SoceProto sproto;
            size = sproto.write_string(proto, required_, has_set_, arg);
        }
        else{
            size = proto->write_string(arg);
        }
        return size;
    }

} // namespace fadsdb
} // namespace soce
