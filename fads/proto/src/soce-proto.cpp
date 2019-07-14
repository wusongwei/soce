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
#include <algorithm>
#include "proto/soce-proto.h"

using std::string;
using namespace std;

namespace soce{
namespace proto{

    uint32_t SoceProto::read_bool(ProtoIf* proto, bool required, bool& out)
    {
        return read(proto, required, SoceDataType::kTypeBool, [&]{
                return proto->read_bool(out);
            });
    }

    uint32_t SoceProto::read_int32(ProtoIf* proto, bool required, int32_t& out)
    {
        return read(proto, required, SoceDataType::kTypeInt32, [&]{
                return proto->read_int32(out);
            });
    }

    uint32_t SoceProto::read_int64(ProtoIf* proto, bool required, long& out)
    {
        return read(proto, required, SoceDataType::kTypeInt64, [&]{
                return proto->read_int64(out);
            });
    }

    uint32_t SoceProto::read_float(ProtoIf* proto, bool required, float& out)
    {
        return read(proto, required, SoceDataType::kTypeFloat, [&]{
                return proto->read_float(out);
            });
    }

    uint32_t SoceProto::read_double(ProtoIf* proto, bool required, double& out)
    {
        return read(proto, required, SoceDataType::kTypeDouble, [&]{
                return proto->read_double(out);
            });
    }

    uint32_t SoceProto::read_string(ProtoIf* proto, bool required, string& out)
    {
        return read(proto, required, SoceDataType::kTypeString, [&]{
                return proto->read_string(out);
            });
    }

    uint32_t SoceProto::read_bytes(ProtoIf* proto, bool required, char* out, uint32_t len)
    {
        return read(proto, required, SoceDataType::kTypeBytes, [&]{
                return proto->read_bytes(out, len);
            });
    }

    uint32_t SoceProto::read_enum(ProtoIf* proto, bool required, int32_t& out)
    {
        return read(proto, required, SoceDataType::kTypeEnum, [&]{
                return proto->read_enum(out);
            });
    }

    uint32_t SoceProto::read_list(ProtoIf* proto,
                                  bool required,
                                  bool with_type,
                                  SoceDataType elem_type,
                                  SoceProto::ListResizer resizer,
                                  SoceProto::ElemHandler reader)
    {
        uint32_t count = 0;
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t remain_len = 0;
        uint32_t len_size = 0;
        uint32_t size = 0;
        SoceDataType type;

        if (with_type){
            if (!proto->read_type(type)){
                return 0;
            }

            if (type == SoceDataType::kTypeNull){
                return required ? 0 : 1;
            }
            else if (type != SoceDataType::kTypeList){
                return 0;
            }
        }

        count = proto->read_len(remain_len);
        if (!count){
            return 0;
        }
        len_size = count;

        count = proto->read_len(size);
        if (!count){
            return 0;
        }
        len += count;

        count = proto->read_type(type);
        if (!count){
            return 0;
        }
        if (type != elem_type){
            return 0;
        }
        len += count;

        resizer(size);
        for (uint32_t i=0; i<size; ++i){
            elem_len = reader(i);
            if (!elem_len){
                return 0;
            }
            len += elem_len;
        }
        if (len != remain_len){
            return 0;
        }
        return (with_type ? 1 : 0) + len_size + len;
    }

    uint32_t SoceProto::read_set(ProtoIf* proto,
                                 bool required,
                                 bool with_type,
                                 SoceDataType elem_type,
                                 SoceProto::ObjHandler reader)
    {
        uint32_t count = 0;
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t remain_len = 0;
        uint32_t len_size = 0;
        uint32_t size = 0;
        SoceDataType type;

        if (with_type){
            if (!proto->read_type(type)){
                return 0;
            }

            if (type == SoceDataType::kTypeNull){
                return required ? 0 : 1;
            }
            else if (type != SoceDataType::kTypeSet){
                return 0;
            }
        }

        count = proto->read_len(remain_len);
        if (!count){
            return 0;
        }
        len_size = count;

        count = proto->read_len(size);
        if (!count){
            return 0;
        }
        len += count;

        count = proto->read_type(type);
        if (!count){
            return 0;
        }
        if (type != elem_type){
            return 0;
        }
        len += count;

        for (uint32_t i=0; i<size; ++i){
            elem_len = reader();
            if (!elem_len){
                return 0;
            }
            len += elem_len;
        }
        if (len != remain_len){
            return 0;
        }
        return (with_type ? 1 : 0) + len_size + len;
    }

    uint32_t SoceProto::read_map(ProtoIf* proto,
                                 bool required,
                                 bool with_type,
                                 SoceDataType key_type,
                                 SoceDataType val_type,
                                 SoceProto::ObjHandler reader)
    {
        uint32_t count = 0;
        uint32_t len = 0;
        uint32_t elem_len = 0;
        uint32_t remain_len = 0;
        uint32_t len_size = 0;
        uint32_t size = 0;
        SoceDataType type;

        if (with_type){
            if (!proto->read_type(type)){
                return 0;
            }

            if (type == SoceDataType::kTypeNull){
                return required ? 0 : 1;
            }
            else if (type != SoceDataType::kTypeMap){
                return 0;
            }
        }

        count = proto->read_len(remain_len);
        if (!count){
            return 0;
        }
        len_size = count;

        count = proto->read_len(size);
        if (!count){
            return 0;
        }
        len += count;

        count = proto->read_type(type);
        if (!count){
            return 0;
        }
        if (type != key_type){
            return 0;
        }
        len += count;

        count = proto->read_type(type);
        if (!count){
            return 0;
        }
        if (type != val_type){
            return 0;
        }
        len += count;

        for (uint32_t i=0; i<size; ++i){
            elem_len = reader();
            if (!elem_len){
                return 0;
            }
            len += elem_len;
        }
        if (len != remain_len){
            return 0;
        }
        return (with_type ? 1 : 0) + len_size + len;
    }

    uint32_t SoceProto::read_struct(ProtoIf* proto,
                                    bool required,
                                    bool with_type,
                                    uint32_t schema_elem_num,
                                    SoceProto::RequiredChecker checker,
                                    SoceProto::ElemHandler reader)
    {

        uint32_t len = 0;
        uint32_t len_size = 0;
        uint32_t elem_len = 0;
        uint32_t remain_len = 0;
        uint32_t in_elem_num = 0;
        SoceDataType type;

        if (with_type){
            if (!proto->read_type(type)){
                return 0;
            }

            if (type == SoceDataType::kTypeNull){
                return required ? 0 : 1;
            }
            else if (type != SoceDataType::kTypeStruct){
                return 0;
            }
        }

        len_size = proto->read_len(remain_len);
        if (!len_size){
            return 0;
        }

        elem_len = proto->read_len(in_elem_num);
        if (!elem_len){
            return 0;
        }
        len += elem_len;

        uint32_t valid_num = std::min(in_elem_num, schema_elem_num);
        if (!checker(valid_num, schema_elem_num)){
            return 0;
        }

        elem_len = reader(valid_num);
        if (!elem_len){
            return 0;
        }
        len += elem_len;

        if (in_elem_num <= schema_elem_num && len != remain_len){
            return 0;
        }
        else{
            len = remain_len;
        }

        return (with_type ? 1 : 0) + len_size + len;
    }


    uint32_t SoceProto::write_bool(ProtoIf* proto, bool required, bool has_set, bool arg)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_bool(arg, true);
            });
    }

    uint32_t SoceProto::write_int32(ProtoIf* proto, bool required, bool has_set, int32_t arg)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_int32(arg, true);
            });
    }

    uint32_t SoceProto::write_int64(ProtoIf* proto, bool required, bool has_set, int64_t arg)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_int64(arg, true);
            });
    }

    uint32_t SoceProto::write_float(ProtoIf* proto, bool required, bool has_set, float arg)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_float(arg, true);
            });
    }

    uint32_t SoceProto::write_double(ProtoIf* proto, bool required, bool has_set, double arg)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_double(arg, true);
            });
    }

    uint32_t SoceProto::write_string(ProtoIf* proto, bool required, bool has_set, const std::string& arg)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_string(arg, true);
            });
    }

    uint32_t SoceProto::write_bytes(ProtoIf* proto, bool required, bool has_set, const char* arg, uint32_t len)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_bytes(arg, len);
            });
    }

    uint32_t SoceProto::write_enum(ProtoIf* proto, bool required, bool has_set, int32_t arg)
    {
        return write(proto, required, has_set, [&]{
                return proto->write_enum(arg, true);
            });
    }

    uint32_t SoceProto::write_list(ProtoIf* proto,
                                   bool required,
                                   bool has_set,
                                   bool with_type,
                                   uint32_t elem_num,
                                   SoceDataType elem_type,
                                   SoceProto::ElemHandler Writer)
    {
        return write(proto, required, has_set, [&]()->uint32_t{
                if (with_type){
                    proto->write_type(SoceDataType::kTypeList);
                }

                uint32_t len = 0;
                uint32_t len_pos = proto->size();
                uint32_t elem_len = 0;
                uint32_t len_size  = 0;

                proto->write_byte(0);
                len += proto->write_len(elem_num);
                len += proto->write_type(elem_type);
                for (uint32_t i = 0; i < elem_num; ++i){
                    elem_len = Writer(i);
                    if (!elem_len){
                        return 0;
                    }
                    len += elem_len;
                }

                len_size = proto->calc_len_size(len);
                if (len_size == 1){
                    len_size = proto->write_len(len, len_pos);
                }
                else{
                    proto->move(len_pos + 1, len_size - 1, len);
                    len_size = proto->write_len(len, len_pos);
                }
                len += len_size;

                return (with_type ? 1 : 0) + len;
            });
    }

    uint32_t SoceProto::write_set(ProtoIf* proto,
                                  bool required,
                                  bool has_set,
                                  bool with_type,
                                  uint32_t elem_num,
                                  SoceDataType elem_type,
                                  SoceProto::ObjHandler Writer)
    {
        return write(proto, required, has_set, [&]()->uint32_t{
                if (with_type){
                    proto->write_type(SoceDataType::kTypeSet);
                }

                uint32_t len = 0;
                uint32_t len_pos = proto->size();
                uint32_t elem_len = 0;
                uint32_t len_size  = 0;

                proto->write_byte(0);
                len += proto->write_len(elem_num);
                len += proto->write_type(elem_type);
                elem_len = Writer();
                if (!elem_len){
                    return 0;
                }
                len += elem_len;

                len_size = proto->calc_len_size(len);
                if (len_size == 1){
                    len_size = proto->write_len(len, len_pos);
                }
                else{
                    proto->move(len_pos + 1, len_size - 1, len);
                    len_size = proto->write_len(len, len_pos);
                }
                len += len_size;

                return (with_type ? 1 : 0) + len;
            });
    }

    uint32_t SoceProto::write_map(ProtoIf* proto,
                                  bool required,
                                  bool has_set,
                                  bool with_type,
                                  uint32_t elem_num,
                                  SoceDataType key_type,
                                  SoceDataType val_type,
                                  SoceProto::ObjHandler Writer)
    {
        return write(proto, required, has_set, [&]()->uint32_t{
                if (with_type){
                    proto->write_type(SoceDataType::kTypeMap);
                }

                uint32_t len = 0;
                uint32_t len_pos = proto->size();
                uint32_t elem_len = 0;
                uint32_t len_size  = 0;

                proto->write_byte(0);
                len += proto->write_len(elem_num);
                len += proto->write_type(key_type);
                len += proto->write_type(val_type);
                elem_len = Writer();
                if (!elem_len){
                    return 0;
                }
                len += elem_len;

                len_size = proto->calc_len_size(len);
                if (len_size == 1){
                    len_size = proto->write_len(len, len_pos);
                }
                else{
                    proto->move(len_pos + 1, len_size - 1, len);
                    len_size = proto->write_len(len, len_pos);
                }
                len += len_size;

                return (with_type ? 1 : 0) + len;
            });
    }

    uint32_t SoceProto::write_struct(ProtoIf* proto,
                                     bool required,
                                     bool has_set,
                                     bool with_type,
                                     uint32_t schema_elem_num,
                                     ObjHandler Writer)
    {
        return write(proto, required, has_set, [&]()->uint32_t{
                if (with_type){
                    proto->write_type(SoceDataType::kTypeStruct);
                }

                uint32_t len = 0;
                uint32_t len_pos = proto->size();
                uint32_t elem_len = 0;
                uint32_t len_size  = 0;

                proto->write_byte(0);
                len += proto->write_len(schema_elem_num);

                elem_len = Writer();
                if (!elem_len){
                    return 0;
                }
                len += elem_len;

                len_size = proto->calc_len_size(len);
                if (len_size == 1){
                    len_size = proto->write_len(len, len_pos);
                }
                else{
                    proto->move(len_pos + 1, len_size - 1, len);
                    len_size = proto->write_len(len, len_pos);
                }
                len += len_size;

                return (with_type ? 1 : 0) + len;
            });
    }

    uint32_t SoceProto::read(ProtoIf* proto,
                             bool required,
                             SoceDataType expect,
                             ObjHandler reader)
    {
        SoceDataType type;

        if (!proto->read_type(type)){
            return 0;
        }

        if (type == SoceDataType::kTypeNull){
            return required ? 0 : 1;
        }
        else if (type != expect){
            return 0;
        }

        uint32_t len = reader();
        if (!len){
            return 0;
        }

        return 1 + len;
    }

    uint32_t SoceProto::write(ProtoIf* proto,
                              bool required,
                              bool has_set,
                              ObjHandler Writer)
    {
        uint32_t rc = 0;

        if (required && !has_set){
            return 0;
        }

        if (has_set){
            rc = Writer();
        }
        else{
            rc = proto->write_null();
        }

        return rc;
    }

} // namespace proto
} // namespace soce
