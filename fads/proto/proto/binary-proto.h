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

#ifndef _SOCE_BINARY_PROTO_zJF3MCGnVE_H_
#define _SOCE_BINARY_PROTO_zJF3MCGnVE_H_

#include "proto-if.h"

namespace soce {
namespace proto{

    class BinaryProto : public ProtoIf
    {
    public:
        virtual uint32_t read_enum(int32_t& arg);
        virtual uint32_t read_bool(bool& arg);
        virtual uint32_t read_int32(int32_t& arg);
        virtual uint32_t read_int64(int64_t& arg);
        virtual uint32_t read_float(float& arg);
        virtual uint32_t read_double(double& arg);
        virtual uint32_t read_string(std::string& arg);
        virtual uint32_t read_bytes(char* arg, uint32_t expect_len);
        virtual uint32_t read_byte(char& arg);
        virtual uint32_t read_type(SoceDataType& type);
        virtual uint32_t read_len(uint32_t& len);
        virtual uint32_t read_len(uint32_t& len, uint32_t pos);

        virtual uint32_t write_null();
        virtual uint32_t write_enum(int32_t arg, bool with_type);
        virtual uint32_t write_bool(bool arg, bool with_type);
        virtual uint32_t write_int32(int32_t arg, bool with_type);
        virtual uint32_t write_int64(int64_t arg, bool with_type);
        virtual uint32_t write_float(float arg, bool with_type);
        virtual uint32_t write_double(double arg, bool with_type);
        virtual uint32_t write_string(const std::string& arg, bool with_type);
        virtual uint32_t write_bytes(const char* arg, uint32_t len);

        virtual uint32_t write_byte(char arg);
        virtual uint32_t write_type(SoceDataType type);
        virtual uint32_t write_len(uint32_t len);
        virtual uint32_t write_len(uint32_t len, uint32_t pos);

        virtual uint32_t get_cur_pos();
        virtual int skip(uint32_t len);
        virtual int skip_to(uint32_t pos);
        virtual uint32_t calc_len_size(uint32_t len);
        virtual void move(uint32_t pos, uint32_t size, uint32_t len);
    };

} // namespace proto
} // namespace soce

#endif
