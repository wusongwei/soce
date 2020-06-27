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

#ifndef _SOCE_PROTO_IF_yBSt2RMR24_H_
#define _SOCE_PROTO_IF_yBSt2RMR24_H_

#include <stdint.h>
#include <string>
#include <cstring>

namespace soce{
namespace proto{

    typedef enum class SoceDataType:uint8_t
    {
        kTypeNull,
            kTypeBool,
            kTypeInt32,
            kTypeInt64,
            kTypeFloat,
            kTypeDouble,
            kTypeString,
            kTypeBytes,
            kTypeEnum,
            kTypeList,
            kTypeSet,
            kTypeMap,
            kTypeStruct
            }SoceDataType;

    class ProtoIf
    {
    public:
        ProtoIf() = default;
        virtual ~ProtoIf()
        {
            reset();
        }

        int init(char* data, uint32_t len, bool own = false)
        {
            if (!data){
                return -1;
            }

            reset();

            if (own){
                data_ = new char[len];
                memcpy(data_, data, len);
            }
            else{
                data_ = (char*)data;
            }

            cap_ = len;
            size_ = 0;
            own_ = own;
            return 0;
        }

        void reset(){
            if (own_ && data_){
                delete [] data_;
            }
            data_ = NULL;
            cap_ = 0;
            size_ = 0;
            own_ = true;
        }

        virtual uint32_t read_enum(int32_t& arg) = 0;
        virtual uint32_t read_bool(bool& arg) = 0;
        virtual uint32_t read_int32(int32_t& arg) = 0;
        virtual uint32_t read_int64(int64_t& arg) = 0;
        virtual uint32_t read_float(float& arg) = 0;
        virtual uint32_t read_double(double& arg) = 0;
        virtual uint32_t read_string(std::string& arg) = 0;
        virtual uint32_t read_bytes(char* arg, uint32_t expect_len) = 0;
        virtual uint32_t read_byte(char& arg) = 0;
        virtual uint32_t read_type(SoceDataType& type) = 0;
        virtual uint32_t read_len(uint32_t& len) = 0;
        virtual uint32_t read_len(uint32_t& len, uint32_t pos) = 0;

        virtual uint32_t write_null() = 0;
        virtual uint32_t write_enum(int32_t arg, bool with_type = false) = 0;
        virtual uint32_t write_bool(bool arg, bool with_type = false) = 0;
        virtual uint32_t write_int32(int32_t arg, bool with_type = false) = 0;
        virtual uint32_t write_int64(int64_t arg, bool with_type = false) = 0;
        virtual uint32_t write_float(float arg, bool with_type = false) = 0;
        virtual uint32_t write_double(double arg, bool with_type = false) = 0;
        virtual uint32_t write_string(const std::string& arg, bool with_type = false) = 0;
        virtual uint32_t write_bytes(const char* arg, uint32_t len) = 0;

        virtual uint32_t write_raw_bytes(const char* arg, uint32_t len) = 0;
        virtual uint32_t write_byte(char arg) = 0;
        virtual uint32_t write_type(SoceDataType type) = 0;
        virtual uint32_t write_len(uint32_t len) = 0;
        virtual uint32_t write_len(uint32_t len, uint32_t pos) = 0;

        virtual uint32_t get_cur_pos() = 0;
        virtual int skip(uint32_t len) = 0;
        virtual int skip_to(uint32_t pos) = 0;
        virtual uint32_t calc_len_size(uint32_t) = 0;
        virtual void move(uint32_t pos, uint32_t size, uint32_t len) = 0;

        char* data(){
            return data_;
        }

        uint32_t size(){
            return size_;
        }

    protected:
        void hold_place(uint32_t size){
            if (size_ + size > cap_){
                char* buff = new char[cap_ + 1024];
                memcpy(buff, data_, size_);
                delete [] data_;
                data_ = buff;
                cap_ += 1024;
            }
        }

    protected:
        char* data_ = NULL;
        uint32_t cap_ = 0;
        uint32_t size_ = 0;
        bool own_ = true;
    };

} // namespace proto
} // namespace soce

#endif
