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

#include "dynamic-getter-impl.h"
#include "proto/binary-proto.h"
#include "utils/str-helper.h"

using std::string;
using std::vector;

namespace soce{
namespace proto{

    int DynamicGetterImpl::init(char* data, size_t len)
    {
        if (!data || !len){
            return -1;
        }
        root_.reset(new DynamicGetterImpl::Component);
        data_ = data;
        len_ = len;
        return 0;
    }

    int DynamicGetterImpl::get_bool(const std::vector<uint32_t>& indexes, bool& val)
    {
        return get(indexes, SoceDataType::kTypeBool, [&](ProtoIf* proto){
                return proto->read_bool(val);
            });
    }

    int DynamicGetterImpl::get_int32(const std::vector<uint32_t>& indexes, int32_t& val)
    {
        return get(indexes, SoceDataType::kTypeInt32, [&](ProtoIf* proto){
                return proto->read_int32(val);
            });
    }

    int DynamicGetterImpl::get_int64(const std::vector<uint32_t>& indexes, int64_t& val)
    {
        return get(indexes, SoceDataType::kTypeInt64, [&](ProtoIf* proto){
                return proto->read_int64(val);
            });
    }

    int DynamicGetterImpl::get_float(const std::vector<uint32_t>& indexes, float& val)
    {
        return get(indexes, SoceDataType::kTypeFloat, [&](ProtoIf* proto){
                return proto->read_float(val);
            });
    }

    int DynamicGetterImpl::get_double(const std::vector<uint32_t>& indexes, double& val)
    {
        return get(indexes, SoceDataType::kTypeDouble, [&](ProtoIf* proto){
                return proto->read_double(val);
            });
    }

    int DynamicGetterImpl::get_string(const std::vector<uint32_t>& indexes, std::string& val)
    {
        return get(indexes, SoceDataType::kTypeString, [&](ProtoIf* proto){
                return proto->read_string(val);
            });
    }

    int DynamicGetterImpl::get_bytes(const std::vector<uint32_t>& indexes, char* val, uint32_t expect_len)
    {
        return get(indexes, SoceDataType::kTypeBytes, [&](ProtoIf* proto){
                return proto->read_bytes(val, expect_len);
            });
    }

    int DynamicGetterImpl::get_enum(const std::vector<uint32_t>& indexes, int32_t& val)
    {
        return get(indexes, SoceDataType::kTypeEnum, [&](ProtoIf* proto){
                return proto->read_enum(val);
            });
    }

    int DynamicGetterImpl::get_bool(const std::string& path, bool& val, bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeBool(val));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::get_int32(const std::string& path, int32_t& val, bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeI32(val));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::get_int64(const std::string& path, int64_t& val, bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeI64(val));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::get_float(const std::string& path, float& val, bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeFloat(val));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::get_double(const std::string& path, double& val, bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeDouble(val));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::get_string(const std::string& path, string& val, bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeString(val));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::get_bytes(const std::string& path,
                                 char* val,
                                 uint32_t expect_len,
                                 bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeBytes(val, expect_len));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::get_enum(const std::string& path, int32_t& val, bool delay)
    {
        std::shared_ptr<CompositeIf> composite(new CompositeEnum(val));
        return get(path, composite, delay);
    }

    int DynamicGetterImpl::do_get()
    {
        if (!data_ || !len_){
            return -1;
        }
        BinaryProto bp;
        bp.init(data_, len_, false);
        return do_get(root_, (soce::proto::ProtoIf*)&bp);
    }

    int DynamicGetterImpl::get(const std::vector<uint32_t>& indexes,
                               SoceDataType type,
                               std::function<int(ProtoIf*)> getter)
    {
        BinaryProto bp;
        bp.init(data_, len_, false);
        ProtoIf* proto = static_cast<ProtoIf*>(&bp);

        int rc = -1;
        size_t size = indexes.size();
        uint32_t len = 0;
        uint32_t elems = 0;
        uint32_t i = 0;
        uint32_t index = 0;

        if (indexes.empty()){
            return -1;
        }

        do{
            index = indexes[i];
            if ((proto->read_len(len) == 0)
                || (!proto->read_len(elems))
                || (index >= elems)
                || skip_elems(index, proto)){
                break;
            }

            SoceDataType t;
            if (!proto->read_type(t)){
                break;
            }

            if (i + 1 == size){
                if (t != type){
                    break;
                }
                rc = getter(proto)? 0 : -1;
                break;
            }
            else if(t != SoceDataType::kTypeStruct){
                break;
            }

            ++i;
        }while(1);

        return rc;
    }

    int DynamicGetterImpl::get(const std::string& path, std::shared_ptr<CompositeIf> composite, bool delay)
    {
        if (add_path(path, composite)){
            return -1;
        }

        int rc = 0;
        if (!delay){
            if (!data_ || !len_){
                return -1;
            }
            BinaryProto bp;
            bp.init(data_, len_, false);
            rc = do_get(root_, (soce::proto::ProtoIf*)&bp);
        }

        return rc;
    }

    int DynamicGetterImpl::add_path(const std::string& path, std::shared_ptr<CompositeIf> composite)
    {
        if (!composite){
            return -1;
        }

        vector<string> vec_path;
        if (soce::utils::StrHelper::split(path, ".", vec_path) || vec_path.empty()){
            return -1;
        }

        std::shared_ptr<Component> node = root_;
        for (auto& i : vec_path){
            int n = std::stoi(i);
            std::shared_ptr<Component> child = node->get_node(n);
            if (!child){
                node = node->add_node(n);
            }
            else{
                if (child->is_leaf()){
                    return -1;
                }
                node = child;
            }
        }

        node->set_composite(composite);
        return 0;
    }

    int DynamicGetterImpl::do_get(std::shared_ptr<Component> node, ProtoIf* proto)
    {
        uint32_t len = 0;
        if (proto->read_len(len) == 0){
            return -1;
        }
        uint32_t pos = proto->get_cur_pos();
        uint32_t elems = 0;
        if (!proto->read_len(elems)){
            return -1;
        }

        int last = 0;
        for (auto& i : node->get_nodes()){
            if ((uint32_t)i.first >= elems){
                return -1;
            }

            if (skip_elems(i.first - last, proto)){
                return -1;
            }
            if (i.second->is_leaf()){
                if (i.second->get_val(proto)){
                    return -1;
                }
            }
            else{
                SoceDataType type;
                if (!proto->read_type(type) || type != SoceDataType::kTypeStruct){
                    return -1;
                }
                do_get(i.second, proto);
            }

            last = i.first + 1;
        }

        proto->skip_to(len + pos);
        return 0;
    }

    int DynamicGetterImpl::skip_elem(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        uint32_t size = 0;
        uint32_t len = 0;
        int32_t i32 = 0;
        int64_t i64 = 0;

        if (!proto->read_type(type)){
            return -1;
        }

        switch(type){
        case SoceDataType::kTypeNull:
        case SoceDataType::kTypeBool:
            if (proto->skip(1)){
                return -1;
            }
            break;

        case SoceDataType::kTypeInt32:
        case SoceDataType::kTypeEnum:
            size = proto->read_int32(i32);
            if (!size){
                return -1;
            }
            break;

        case SoceDataType::kTypeFloat:
            if (proto->skip(4)){
                return -1;
            }
            break;

        case SoceDataType::kTypeInt64:
            size = proto->read_int64(i64);
            if (!size){
                return -1;
            }
            break;

        case SoceDataType::kTypeDouble:
            if (proto->skip(8)){
                return -1;
            }
            break;

        default:
            size = proto->read_len(len);
            if (size == 0){
                return -1;
            }
            if (proto->skip(len)){
                return -1;
            }
            break;
        }

        return 0;
    }

    int DynamicGetterImpl::skip_elems(int n, soce::proto::ProtoIf* proto)
    {
        for (int i=0; i<n; ++i){
            if (skip_elem(proto)){
                return -1;
            }
        }

        return 0;
    }

    int DynamicGetterImpl::CompositeBool::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeBool){
            return -1;
        }
        return proto->read_bool(*val_) ? 0 : -1;
    }

    int DynamicGetterImpl::CompositeI32::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeInt32){
            return -1;
        }
        return proto->read_int32(*val_) ? 0 : -1;
    }

    int DynamicGetterImpl::CompositeI64::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeInt64){
            return -1;
        }
        return proto->read_int64(*val_) ? 0 : -1;
    }

    int DynamicGetterImpl::CompositeFloat::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeFloat){
            return -1;
        }
        return proto->read_float(*val_) ? 0 : -1;
    }

    int DynamicGetterImpl::CompositeDouble::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeDouble){
            return -1;
        }
        return proto->read_double(*val_) ? 0 : -1;
    }

    int DynamicGetterImpl::CompositeString::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeString){
            return -1;
        }
        return proto->read_string(*val_) ? 0 : -1;
    }

    int DynamicGetterImpl::CompositeBytes::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeBytes){
            return -1;
        }
        return proto->read_bytes(val_, expect_len_) ? 0 : -1;
    }

    int DynamicGetterImpl::CompositeEnum::get_val(soce::proto::ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeEnum){
            return -1;
        }
        return proto->read_enum(*val_) ? 0 : -1;
    }

    std::shared_ptr<DynamicGetterImpl::Component> DynamicGetterImpl::Component::get_node(int i)
    {
        std::shared_ptr<DynamicGetterImpl::Component> rc;
        auto iter = nodes_.find(i);
        if (iter != nodes_.end()){
            rc = iter->second;
        }

        return rc;
    }

    std::shared_ptr<DynamicGetterImpl::Component> DynamicGetterImpl::Component::add_node(int i)
    {
        std::shared_ptr<DynamicGetterImpl::Component> nc(new DynamicGetterImpl::Component);
        nodes_[i] = nc;
        return nc;
    }

} // namespace proto
} // namespace soce
