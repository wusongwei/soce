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

#include "node-any.h"
#include "log4rel/logger.h"

using namespace soce::proto;

namespace soce{
namespace fadsdb{

    size_t NodeAny::sbuff_size = 512;
    char* NodeAny::sbuff = new char[NodeAny::sbuff_size];

    NodeAny::~NodeAny()
    {
        if (data_){
            delete [] data_;
        }
    }

    std::shared_ptr<NodeIf> NodeAny::clone()
    {
        std::shared_ptr<NodeAny> node(new NodeAny);
        node->len_ = len_;
        node->cap_ = cap_;
        node->type_ = type_;
        if (cap_) {
            node->data_ = new char[cap_];
            memcpy(node->data_, data_, cap_);
        }
        return node;
    }

    void NodeAny::reset()
    {
        len_ = 0;
    }

    SoceDataType NodeAny::get_type()
    {
        return type_;
    }

    uint32_t NodeAny::serialize(soce::proto::ProtoIf* proto, bool with_type)
    {
        if (!with_type) {
            SOCE_ERROR << _D("serialize NodeAny without type");
            return 0;
        }

        uint32_t total = 1;
        uint32_t len = proto->write_type(type_);
        if (len == 0) {
            return 0;
        }

        if (len_){
            len = proto->write_raw_bytes(data_, len_);
            if (len == 0) {
                return 0;
            }
            total += len;
        }

        return total;
    }

    uint32_t NodeAny::deserialize(soce::proto::ProtoIf* proto, bool with_type)
    {
        if (!with_type) {
            SOCE_ERROR << _D("deserialize NodeAny without type");
            return 0;
        }

        reset();

        BinaryProto bp;
        uint32_t len = 0;

        if (proto->read_type(type_) == 0){
            return 0;
        }

        switch (type_){
        case SoceDataType::kTypeNull:
            // do nothing
            break;

        case SoceDataType::kTypeBool:
        case SoceDataType::kTypeEnum:
        case SoceDataType::kTypeInt32:
            len = copyInt32(proto, bp);
            break;

        case SoceDataType::kTypeInt64:
            len = copyInt64(proto, bp);
            break;

        case SoceDataType::kTypeFloat:
            len = copyFloat(proto, bp);
            break;

        case SoceDataType::kTypeDouble:
            len = copyDouble(proto, bp);
            break;

        default:
            len = copyString(proto, bp);
            break;
        }

        if (len == 0) {
            return 0;
        }

        reset(bp.size());
        memcpy(data_, bp.data(), bp.size());
        len_ = bp.size();

        return 1 + len;
    }

    void NodeAny::reset(uint32_t cap)
    {
        if (cap > cap_){
            if (data_) {
                delete [] data_;
            }

            len_ = 0;
            cap_ = cap;
            data_ = new char[cap_];
        }
    }

    uint32_t NodeAny::copyInt32(ProtoIf* src, BinaryProto& dst)
    {
        int i = 0;
        if (src->read_int32(i) == 0){
            return 0;
        }

        return dst.write_int32(i, false);
    }

    uint32_t NodeAny::copyInt64(ProtoIf* src, BinaryProto& dst)
    {
        int64_t i = 0;
        if (src->read_int64(i) == 0){
            return 0;
        }

        return dst.write_int64(i, false);
    }


    uint32_t NodeAny::copyFloat(ProtoIf* src, BinaryProto& dst)
    {
        float f = 0.0;
        if (src->read_float(f) == 0){
            return 0;
        }

        return dst.write_float(f, false);
    }

    uint32_t NodeAny::copyDouble(ProtoIf* src, BinaryProto& dst)
    {
        double d = 0.0;
        if (src->read_double(d) == 0){
            return 0;
        }

        return dst.write_double(d, false);
    }

    uint32_t NodeAny::copyString(ProtoIf* src, BinaryProto& dst)
    {
        std::string s;
        if (src->read_string(s) == 0){
            return 0;
        }

        return dst.write_string(s, false);
    }

}
}
