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

#include <cassert>
#include <string.h>
#include "proto/binary-proto.h"

using std::string;

namespace soce{
namespace proto{

#define BIGEND_SET(buff, pos, src, type)                        \
    for (size_t i = 0; i < sizeof(type); ++i){                  \
        buff[pos + i] = (*(type*)(&src)                         \
                         >> (sizeof(type) - 1 - i) * 8) & 0xFF; \
    }                                                           \
    pos += sizeof(type);

#define BIGEND_GET(buff, pos, dst, type)                        \
    for (size_t i = 0; i < sizeof(type); ++i){                  \
        *((char*)&dst + sizeof(type) - i - 1) = buff[pos + i];  \
    }                                                           \
    pos += sizeof(type);

#define ZIGZAG_COMPRESS(zigzag, max_size, pos, type)    \
    for (int i = 0; i < max_size; i++) {                \
        if ((zigzag & (~0x7f)) == 0) {                  \
            data_[pos] = zigzag & 0xFF;                 \
            ++pos;                                      \
            ++rc;                                       \
            break;                                      \
        }                                               \
        else{                                           \
            data_[pos] = (zigzag & 0x7f) | 0x80;        \
            ++pos;                                      \
            ++rc;                                       \
            zigzag = ((type)zigzag) >> 7;               \
        }                                               \
    }

#define ZIGZAG_UNCOMPRESS(zigzag, max_size, pos, type)  \
    uint32_t rc = 0;                                    \
    int offset = 0;                                     \
    zigzag = 0;                                         \
    for (int i = 0; i < max_size; i++, offset += 7) {   \
        if (pos + 1 > cap_){                            \
            return 0;                                   \
        }                                               \
                                                        \
        char n = data_[pos];                            \
        ++rc;                                           \
        ++pos;                                          \
        if ((n & 0x80) != 0x80) {                       \
            zigzag |= ((type)n << offset);              \
            break;                                      \
        }                                               \
        else {                                          \
            zigzag |= ((type)(n & 0x7f) << offset);     \
        }                                               \
    }

    uint32_t BinaryProto::read_enum(int32_t& arg)
    {
        return read_int32(arg);
    }

    uint32_t BinaryProto::read_bool(bool& arg)
    {
        if (size_ + 1 > cap_){
            return 0;
        }

        arg = (bool)data_[size_];
        ++size_;
        return 1;
    }

    uint32_t BinaryProto::read_int32(int32_t& arg)
    {
        ZIGZAG_UNCOMPRESS(arg, 5, size_, uint32_t);
        arg = (((uint32_t)arg) >> 1) ^ -(arg & 1);
        return rc;
    }

    uint32_t BinaryProto::read_int64(int64_t& arg)
    {
        ZIGZAG_UNCOMPRESS(arg, 10, size_, uint64_t);
        arg = (((uint64_t)arg) >> 1) ^ -(arg & 1);
        return rc;
    }

    uint32_t BinaryProto::read_float(float& arg)
    {
        if (size_ + 4 > cap_){
            return 0;
        }

        BIGEND_GET(data_, size_, arg, uint32_t);
        return 4;
    }

    uint32_t BinaryProto::read_double(double& arg)
    {
        if (size_ + 8 > cap_){
            return 0;
        }

        BIGEND_GET(data_, size_, arg, uint64_t);
        return 8;
    }

    uint32_t BinaryProto::read_string(string& arg)
    {
        uint32_t len = 0;
        uint32_t size = read_len(len);
        if (!size){
            return 0;
        }

        if (size_ + len > cap_){
            return 0;
        }

        arg.resize(len);
        memcpy((void*)const_cast<char*>(arg.c_str()), data_ + size_, len);
        size_ += len;
        return size + len;
    }

    uint32_t BinaryProto::read_bytes(char* arg, uint32_t expect_len)
    {
        uint32_t len = 0;
        uint32_t size = read_len(len);
        if (!size || len != expect_len){
            return 0;
        }

        if (size_ + len > cap_){
            return 0;
        }

        memcpy(arg, data_ + size_, len);
        size_ += len;
        return size + len;
    }

    uint32_t BinaryProto::read_byte(char& arg)
    {
        if (size_ + 1 > cap_){
            return 0;
        }

        arg = data_[size_];
        ++size_;
        return 1;
    }

    uint32_t BinaryProto::read_type(SoceDataType& type)
    {
        char t;
        if (!read_byte(t)){
            return 0;
        }

        type = (SoceDataType)t;
        return 1;
    }

    uint32_t BinaryProto::read_len(uint32_t& len)
    {
        len = 0;
        ZIGZAG_UNCOMPRESS(len, 5, size_, uint32_t);
        return rc;
    }

    uint32_t BinaryProto::read_len(uint32_t& len, uint32_t pos)
    {
        uint32_t i = 0;
        len = 0;
        for (i = 0; i<4; ++i){
            if (pos + 1 > cap_){
                return 0;
            }

            len += (uint32_t)(data_[pos] & 0x7F) * (1 << (i * 7));
            if (!(data_[pos++] & 0x80)){
                break;
            }
        }

        return i + 1;
    }

    uint32_t BinaryProto::write_null()
    {
        write_type(SoceDataType::kTypeNull);
        return 1;
    }

    uint32_t BinaryProto::write_enum(int32_t arg, bool with_type)
    {
        hold_place(5);
        uint32_t rc = 0;
        if (with_type){
            rc += write_type(SoceDataType::kTypeEnum);
        }

        rc += write_int32((uint32_t)arg, false);
        return rc;
    }

    uint32_t BinaryProto::write_bool(bool arg, bool with_type)
    {
        hold_place(2);

        uint32_t rc = 1;
        if (with_type){
            rc += write_type(SoceDataType::kTypeBool);
        }

        write_byte((char)arg);
        return rc;
    }

    uint32_t BinaryProto::write_int32(int32_t arg, bool with_type)
    {
        hold_place(6);

        uint32_t rc = 0;
        if (with_type){
            rc += write_type(SoceDataType::kTypeInt32);
        }

        int32_t zigzag = (arg << 1) ^ (arg >> 31);
        ZIGZAG_COMPRESS(zigzag, 5, size_, uint32_t);
        return rc;
    }

    uint32_t BinaryProto::write_int64(int64_t arg, bool with_type)
    {
        hold_place(11);

        uint32_t rc = 0;
        if (with_type){
            rc += write_type(SoceDataType::kTypeInt64);
        }

        int64_t zigzag = (arg << 1) ^ (arg >> 63);
        ZIGZAG_COMPRESS(zigzag, 10, size_, uint64_t);
        return rc;
    }

    uint32_t BinaryProto::write_float(float arg, bool with_type)
    {
        hold_place(5);

        uint32_t rc = 4;
        if (with_type){
            rc += write_type(SoceDataType::kTypeFloat);
        }

        BIGEND_SET(data_, size_, arg, uint32_t);
        return rc;
    }

    uint32_t BinaryProto::write_double(double arg, bool with_type)
    {
        hold_place(9);

        uint32_t rc = 8;
        if (with_type){
            rc += write_type(SoceDataType::kTypeDouble);
        }

        BIGEND_SET(data_, size_, arg, uint64_t);
        return rc;
    }

    uint32_t BinaryProto::write_string(const std::string& arg, bool with_type)
    {
        hold_place(5 + arg.size());

        uint32_t len = arg.size();
        uint32_t rc = 0;
        if (with_type){
            rc += write_type(SoceDataType::kTypeString);
        }
        rc += write_len(len);
        memcpy(data_ + size_, arg.c_str(), len);
        size_ += len;
        return rc + len;
    }

    uint32_t BinaryProto::write_bytes(const char* arg, uint32_t len)
    {
        hold_place(6 + len);

        uint32_t rc = 0;
        rc = write_type(SoceDataType::kTypeBytes);
        rc += write_len(len);
        memcpy(data_ + size_, arg, len);
        size_ += len;
        return  rc + len;
    }

    uint32_t BinaryProto::write_raw_bytes(const char* arg, uint32_t len)
    {
        hold_place(len);

        memcpy(data_ + size_, arg, len);
        size_ += len;
        return  len;
    }

    uint32_t BinaryProto::write_byte(char arg)
    {
        hold_place(1);

        data_[size_] = arg;
        ++size_;
        return 1;
    }

    uint32_t BinaryProto::write_type(SoceDataType type)
    {
        write_byte((char)type);
        return 1;
    }

    uint32_t BinaryProto::write_len(uint32_t len)
    {
        hold_place(5);

        uint32_t rc = 0;
        ZIGZAG_COMPRESS(len, 5, size_, uint32_t);
        return rc;
    }

    uint32_t BinaryProto::write_len(uint32_t len, uint32_t pos)
    {
        uint32_t rc = 0;
        ZIGZAG_COMPRESS(len, 5, pos, uint32_t);
        return rc;
    }

    uint32_t BinaryProto::get_cur_pos()
    {
        return size_;
    }

    int BinaryProto::skip(uint32_t len)
    {
        if (size_ + len > cap_){
            return -1;
        }

        size_ += len;
        return 0;
    }

    int BinaryProto::skip_to(uint32_t pos)
    {
        if (pos > cap_){
            return -1;
        }

        size_ = pos;
        return 0;
    }

    uint32_t BinaryProto::calc_len_size(uint32_t len)
    {
        uint32_t i = 0;
        for (; i<5; i++) {
            if ((len & (~0x7f)) == 0) {
                break;
            }
            else{
                len = ((unsigned int)len) >> 7;
            }
        }

        return i + 1;
    }

    void BinaryProto::move(uint32_t pos, uint32_t size, uint32_t len)
    {
        if (size == 0){
            return;
        }
        hold_place(size);
        memmove(data_ + pos + size, data_ + pos, len);
        size_ += size;
    }

} // namespace proto
} // namespace soce
