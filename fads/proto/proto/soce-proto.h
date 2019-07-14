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

#ifndef _SOCE_SOCE_PROTO_S3sR6V5EZL_H_
#define _SOCE_SOCE_PROTO_S3sR6V5EZL_H_

#include <string>
#include <stdint.h>
#include <functional>
#include "proto-if.h"

namespace soce{
namespace proto{

    class SoceProto
    {
    public:
        typedef std::function<void(uint32_t)> ListResizer;
        typedef std::function<bool(uint32_t, uint32_t)> RequiredChecker;
        typedef std::function<uint32_t(uint32_t)> ElemHandler;
        typedef std::function<uint32_t()> ObjHandler;

    public:
        static uint32_t read(ProtoIf* proto,
                             bool required,
                             SoceDataType expect,
                             ObjHandler reader);
        static uint32_t read_bool(ProtoIf* proto, bool required, bool& out);
        static uint32_t read_int32(ProtoIf* proto, bool required, int32_t& out);
        static uint32_t read_int64(ProtoIf* proto, bool required, int64_t& out);
        static uint32_t read_float(ProtoIf* proto, bool required, float& out);
        static uint32_t read_double(ProtoIf* proto, bool required, double& out);
        static uint32_t read_string(ProtoIf* proto, bool required, std::string& out);
        static uint32_t read_bytes(ProtoIf* proto, bool required, char* out, uint32_t expect_len);
        static uint32_t read_enum(ProtoIf* proto, bool required, int32_t& out);
        static uint32_t read_list(ProtoIf* proto,
                                  bool required,
                                  bool with_type,
                                  SoceDataType elem_type,
                                  ListResizer resizer,
                                  ElemHandler reader);
        static uint32_t read_set(ProtoIf* proto,
                                 bool required,
                                 bool with_type,
                                 SoceDataType elem_type,
                                 ObjHandler reader);
        static uint32_t read_map(ProtoIf* proto,
                                 bool required,
                                 bool with_type,
                                 SoceDataType key_type,
                                 SoceDataType val_type,
                                 ObjHandler reader);
        static uint32_t read_struct(ProtoIf* proto,
                                    bool required,
                                    bool with_type,
                                    uint32_t schema_elem_num,
                                    RequiredChecker checker,
                                    ElemHandler reader);

        static uint32_t write_bool(ProtoIf* proto, bool required, bool has_set, bool arg);
        static uint32_t write_int32(ProtoIf* proto, bool required, bool has_set, int32_t arg);
        static uint32_t write_int64(ProtoIf* proto, bool required, bool has_set, int64_t arg);
        static uint32_t write_float(ProtoIf* proto, bool required, bool has_set, float arg);
        static uint32_t write_double(ProtoIf* proto, bool required, bool has_set, double arg);
        static uint32_t write_string(ProtoIf* proto, bool required, bool has_set, const std::string& arg);
        static uint32_t write_bytes(ProtoIf* proto, bool required, bool has_set, const char* arg, uint32_t len);
        static uint32_t write_enum(ProtoIf* proto, bool required, bool has_set, int32_t arg);
        static uint32_t write_list(ProtoIf* proto,
                                   bool required,
                                   bool has_set,
                                   bool with_type,
                                   uint32_t elem_num,
                                   SoceDataType elem_type,
                                   ElemHandler reader);

        static uint32_t write_set(ProtoIf* proto,
                                  bool required,
                                  bool has_set,
                                  bool with_type,
                                  uint32_t elem_num,
                                  SoceDataType elem_type,
                                  ObjHandler reader);

        static uint32_t write_map(ProtoIf* proto,
                                  bool required,
                                  bool has_set,
                                  bool with_type,
                                  uint32_t elem_num,
                                  SoceDataType key_type,
                                  SoceDataType val_type,
                                  ObjHandler reader);

        static uint32_t write_struct(ProtoIf* proto,
                                     bool required,
                                     bool has_set,
                                     bool with_type,
                                     uint32_t schema_elem_num,
                                     ObjHandler Writer);

        static uint32_t write(ProtoIf* proto,
                              bool required,
                              bool has_set,
                              ObjHandler Writer);

    };

} // namespace proto
} // namespace soce

#endif
