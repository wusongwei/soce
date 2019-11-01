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

#include <sstream>
#include "proto/dynamic-node.h"
#include "proto/soce-proto.h"
#include "utils/str-helper.h"

namespace soce {
namespace proto {

    std::string DynamicNode::get_type_str(SoceDataType type)
    {
        std::string type_str;
        switch (type) {
        case SoceDataType::kTypeNull:
            type_str = "null";
            break;

        case SoceDataType::kTypeBool:
            type_str = "bool";
            break;

        case SoceDataType::kTypeInt32:
            type_str = "int32";
            break;

        case SoceDataType::kTypeInt64:
            type_str = "int64";
            break;

        case SoceDataType::kTypeFloat:
            type_str = "float";
            break;

        case SoceDataType::kTypeDouble:
            type_str = "double";
            break;

        case SoceDataType::kTypeString:
            type_str = "string";
            break;

        case SoceDataType::kTypeBytes:
            type_str = "bytes";
            break;

        case SoceDataType::kTypeEnum:
            type_str = "enum";
            break;

        case SoceDataType::kTypeList:
            type_str = "list";
            break;

        case SoceDataType::kTypeSet:
            type_str = "set";
            break;

        case SoceDataType::kTypeMap:
            type_str = "map";
            break;

        case SoceDataType::kTypeStruct:
            type_str = "struct";
            break;

        default:
            break;
        }

        return std::move(type_str);
    }

    std::shared_ptr<DynamicNode> DynamicNode::create(SoceDataType type)
    {
        std::shared_ptr<DynamicNode> elem;

        switch (type) {
        case SoceDataType::kTypeNull:
            elem = std::make_shared<DynamicNodeNull>();
            break;

        case SoceDataType::kTypeBool:
            elem = std::make_shared<DynamicNodeBool>();
            break;

        case SoceDataType::kTypeInt32:
            elem = std::make_shared<DynamicNodeInt32>();
            break;

        case SoceDataType::kTypeInt64:
            elem = std::make_shared<DynamicNodeInt64>();
            break;

        case SoceDataType::kTypeFloat:
            elem = std::make_shared<DynamicNodeFloat>();
            break;

        case SoceDataType::kTypeDouble:
            elem = std::make_shared<DynamicNodeDouble>();
            break;

        case SoceDataType::kTypeString:
            elem = std::make_shared<DynamicNodeString>();
            break;

        case SoceDataType::kTypeBytes:
            elem = std::make_shared<DynamicNodeBytes>();
            break;

        case SoceDataType::kTypeEnum:
            elem = std::make_shared<DynamicNodeEnum>();
            break;

        case SoceDataType::kTypeList:
            elem = std::make_shared<DynamicNodeList>();
            break;

        case SoceDataType::kTypeSet:
            elem = std::make_shared<DynamicNodeSet>();
            break;

        case SoceDataType::kTypeMap:
            elem = std::make_shared<DynamicNodeMap>();
            break;

        case SoceDataType::kTypeStruct:
            elem = std::make_shared<DynamicNodeStruct>();
            break;

        default:
            break;
        }

        return elem;
    }

    DynamicNode::DynamicNode(SoceDataType type)
        : type_(type)
    {
    }

    DynamicNode::DynamicNode(SoceDataType type, const std::string& name)
        : type_(type), name_(name)
    {
    }

    DynamicNode::~DynamicNode()
    {
    }

    std::string DynamicNode::to_json()
    {
        std::string json = "{\"name\" : \""
            + name_
            + "\", \"type\" : \""
            + DynamicNode::get_type_str(type_)
            + "\", \"value\" : "
            + get_value()
            + "}";
        return std::move(json);
    }

    DynamicNodeNull::DynamicNodeNull()
        : DynamicNodeNull(std::string())
    {
    }

    DynamicNodeNull::DynamicNodeNull(const std::string& name)
        : DynamicNode(SoceDataType::kTypeNull, name)

    {
    }

    std::string DynamicNodeNull::get_value()
    {
        return "null";
    }

    uint32_t DynamicNodeNull::serialize(ProtoIf* proto)
    {
        return 0;
    }

    uint32_t DynamicNodeNull::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return 0;
    }

    void DynamicNodeNull::reset()
    {
    }

    std::shared_ptr<DynamicNode> DynamicNodeNull::clone()
    {
        return std::make_shared<DynamicNodeNull>(name_);
    }

    DynamicNodeBool::DynamicNodeBool()
        : DynamicNodeBool(std::string(), 0)
    {
    }

    DynamicNodeBool::DynamicNodeBool(bool val)
        : DynamicNodeBool(std::string(), val)
    {
    }

    DynamicNodeBool::DynamicNodeBool(const std::string& name, bool val)
        : DynamicNode(SoceDataType::kTypeBool, name), val_(val)
    {
    }

    std::string DynamicNodeBool::get_value()
    {
        return val_ ? "true" : "false";
    }

    uint32_t DynamicNodeBool::serialize(ProtoIf* proto)
    {
        return proto->write_bool(val_);
    }

    uint32_t DynamicNodeBool::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return proto->read_bool(val_);
    }

    void DynamicNodeBool::reset()
    {
        setted_ = false;
        val_ = false;
    }

    std::shared_ptr<DynamicNode> DynamicNodeBool::clone()
    {
        auto node = std::make_shared<DynamicNodeBool>(name_, val_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeInt32::DynamicNodeInt32()
        : DynamicNodeInt32(std::string(), 0)
    {
    }

    DynamicNodeInt32::DynamicNodeInt32(int32_t val)
        : DynamicNodeInt32(std::string(), val)
    {
    }

    DynamicNodeInt32::DynamicNodeInt32(const std::string& name, int32_t val)
        : DynamicNode(SoceDataType::kTypeInt32, name), val_(val)
    {
    }

    std::string DynamicNodeInt32::get_value()
    {
        return std::to_string(val_);
    }

    uint32_t DynamicNodeInt32::serialize(ProtoIf* proto)
    {
        return proto->write_int32(val_);
    }

    uint32_t DynamicNodeInt32::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return proto->read_int32(val_);
    }

    void DynamicNodeInt32::reset()
    {
        setted_ = false;
        val_ = 0;
    }

    std::shared_ptr<DynamicNode> DynamicNodeInt32::clone()
    {
        auto node = std::make_shared<DynamicNodeInt32>(name_, val_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeInt64::DynamicNodeInt64()
        : DynamicNodeInt64(std::string(), 0)
    {
    }

    DynamicNodeInt64::DynamicNodeInt64(int64_t val)
        : DynamicNodeInt64(std::string(), val)
    {
    }

    DynamicNodeInt64::DynamicNodeInt64(const std::string& name, int64_t val)
        : DynamicNode(SoceDataType::kTypeInt64, name), val_(val)
    {
    }

    std::string DynamicNodeInt64::get_value()
    {
        return std::to_string(val_);
    }

    uint32_t DynamicNodeInt64::serialize(ProtoIf* proto)
    {
        return proto->write_int64(val_);
    }

    uint32_t DynamicNodeInt64::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return proto->read_int64(val_);
    }

    void DynamicNodeInt64::reset()
    {
        setted_ = false;
        val_ = 0;
    }

    std::shared_ptr<DynamicNode> DynamicNodeInt64::clone()
    {
        auto node = std::make_shared<DynamicNodeInt64>(name_, val_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeFloat::DynamicNodeFloat()
        : DynamicNodeFloat(std::string(), 0)
    {
    }

    DynamicNodeFloat::DynamicNodeFloat(float val)
        : DynamicNodeFloat(std::string(), val)
    {
    }

    DynamicNodeFloat::DynamicNodeFloat(const std::string& name, float val)
        : DynamicNode(SoceDataType::kTypeFloat, name), val_(val)
    {
    }

    std::string DynamicNodeFloat::get_value()
    {
        return std::to_string(val_);
    }

    uint32_t DynamicNodeFloat::serialize(ProtoIf* proto)
    {
        return proto->write_float(val_);
    }

    uint32_t DynamicNodeFloat::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return proto->read_float(val_);
    }

    void DynamicNodeFloat::reset()
    {
        setted_ = false;
        val_ = 0;
    }

    std::shared_ptr<DynamicNode> DynamicNodeFloat::clone()
    {
        auto node = std::make_shared<DynamicNodeFloat>(name_, val_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeDouble::DynamicNodeDouble()
        : DynamicNodeDouble(std::string(), 0)
    {
    }

    DynamicNodeDouble::DynamicNodeDouble(double val)
        : DynamicNodeDouble(std::string(), val)
    {
    }

    DynamicNodeDouble::DynamicNodeDouble(const std::string& name, double val)
        : DynamicNode(SoceDataType::kTypeDouble, name), val_(val)
    {
    }

    std::string DynamicNodeDouble::get_value()
    {
        return std::to_string(val_);
    }

    uint32_t DynamicNodeDouble::serialize(ProtoIf* proto)
    {
        return proto->write_double(val_);
    }

    uint32_t DynamicNodeDouble::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return proto->read_double(val_);
    }

    void DynamicNodeDouble::reset()
    {
        setted_ = false;
        val_ = 0;
    }

    std::shared_ptr<DynamicNode> DynamicNodeDouble::clone()
    {
        auto node = std::make_shared<DynamicNodeDouble>(name_, val_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeString::DynamicNodeString()
        : DynamicNodeString(std::string())
    {
    }

    DynamicNodeString::DynamicNodeString(const std::string& val)
        : DynamicNodeString(std::string(), val)
    {
    }

    DynamicNodeString::DynamicNodeString(const std::string& name, const std::string& val)
        : DynamicNode(SoceDataType::kTypeString, name), val_(val)
    {
    }

    std::string DynamicNodeString::get_value()
    {
        std::string val = "\"" + val_ + "\"";
        return std::move(val);
    }

    uint32_t DynamicNodeString::serialize(ProtoIf* proto)
    {
        return proto->write_string(val_);
    }

    uint32_t DynamicNodeString::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return proto->read_string(val_);
    }

    void DynamicNodeString::reset()
    {
        setted_ = false;
        val_.clear();
    }

    std::shared_ptr<DynamicNode> DynamicNodeString::clone()
    {
        auto node = std::make_shared<DynamicNodeString>(name_, val_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeBytes::DynamicNodeBytes()
        : DynamicNodeBytes(NULL, 0)
    {
    }

    DynamicNodeBytes::DynamicNodeBytes(const char* data, size_t len)
        : DynamicNodeBytes(std::string(), data, len)
    {
    }

    DynamicNodeBytes::DynamicNodeBytes(const std::string& name, const char* data, size_t len)
        : DynamicNode(SoceDataType::kTypeBytes, name)
    {
        if (data && len > 0) {
            if (val_) {
                delete [] val_;
            }

            val_ = new char[len];
            len_ = len;
            memcpy(val_, data, len);
        }
    }

    DynamicNodeBytes::~DynamicNodeBytes()
    {
        if (val_){
            delete [] val_;
        }
    }

    std::string DynamicNodeBytes::get_value()
    {
        std::string val = "\"";
        if (len_ > 0) {
            val += std::string(val_, len_);
        }
        val += "\"";

        return std::move(val);
    }

    uint32_t DynamicNodeBytes::serialize(ProtoIf* proto)
    {
        return proto->write_bytes(val_, len_);
    }

    uint32_t DynamicNodeBytes::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;
        uint32_t total_size = 0;
        uint32_t size = 0;

        uint32_t pos = proto->get_cur_pos();
        size = proto->read_len(len_);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        if (val_) {
            delete [] val_;
        }
        val_ = new char[len_];

        proto->skip_to(pos);
        size = proto->read_bytes(val_, len_);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        setted_ = true;
        return total_size;
    }

    void DynamicNodeBytes::reset()
    {
        setted_ = false;
        if (val_) {
            delete [] val_;
        }
        val_ = NULL;
        len_ = 0;
    }

    std::shared_ptr<DynamicNode> DynamicNodeBytes::clone()
    {
        auto node = std::make_shared<DynamicNodeBytes>(name_, val_, len_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeEnum::DynamicNodeEnum()
        : DynamicNodeEnum(std::string(), 0)
    {
    }

    DynamicNodeEnum::DynamicNodeEnum(int32_t val)
        : DynamicNodeEnum(std::string(), val)
    {
    }

    DynamicNodeEnum::DynamicNodeEnum(const std::string& name, int32_t val)
        : DynamicNode(SoceDataType::kTypeEnum, name), val_(val)
    {
    }

    std::string DynamicNodeEnum::get_value()
    {
        return std::to_string(val_);
    }

    uint32_t DynamicNodeEnum::serialize(ProtoIf* proto)
    {
        return proto->write_enum(val_);
    }

    uint32_t DynamicNodeEnum::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        setted_ = true;
        return proto->read_int32(val_);
    }

    void DynamicNodeEnum::reset()
    {
        setted_ = false;
        val_ = 0;
    }

    std::shared_ptr<DynamicNode> DynamicNodeEnum::clone()
    {
        auto node = std::make_shared<DynamicNodeEnum>(name_, val_);
        node->set_required(required_);
        node->set_setted(setted_);
        return node;
    }

    DynamicNodeList::DynamicNodeList()
        : DynamicNode(SoceDataType::kTypeList, std::string())
    {
    }

    DynamicNodeList::DynamicNodeList(SoceDataType type)
    : DynamicNodeList(std::string(), type)
    {
    }

    DynamicNodeList::DynamicNodeList(const std::string& name, SoceDataType type)
        : DynamicNode(SoceDataType::kTypeList, name), elem_type_(type)
    {
    }

    std::string DynamicNodeList::get_value()
    {
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (auto& i : elems_) {
            if (first) {
                first = false;
            }else{
                oss << ",";
            }
            oss << i->get_value();
        }
        oss << "]";
        return std::move(oss.str());
    }

    uint32_t DynamicNodeList::serialize(ProtoIf* proto)
    {
        return SoceProto::write_list(proto,
                                     required_,
                                     setted_,
                                     false,
                                     elems_.size(),
                                     elem_type_,
                                     [&, this](uint32_t i)->uint32_t{
                                         return elems_[i]->serialize(proto);
                                     });
    }

    uint32_t DynamicNodeList::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        uint32_t total_size = 0;
        uint32_t size = 0;
        uint32_t remain_len = 0;
        uint32_t elem_num = 0;

        size = proto->read_len(remain_len);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        size = proto->read_len(elem_num);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        size = proto->read_type(elem_type_);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        for (uint32_t i=0; i<elem_num; ++i) {
            auto n = DynamicNode::create(elem_type_);
            if (!n) {
                return 0;
            }

            size = n->deserialize(proto, false);
            if (size == 0) {
                return 0;
            }
            total_size += size;

            elems_.push_back(n);
        }

        setted_ = true;
        return total_size;
    }

    void DynamicNodeList::reset()
    {
        setted_ = false;
        elems_.clear();
    }

    std::shared_ptr<DynamicNode> DynamicNodeList::clone()
    {
        auto node = std::make_shared<DynamicNodeList>(name_, elem_type_);
        node->set_required(required_);
        node->set_setted(setted_);

        for (auto& i : elems_) {
            node->get_elems().push_back(i->clone());
        }

        return node;
    }

    DynamicNodeSet::DynamicNodeSet()
        : DynamicNode(SoceDataType::kTypeSet, std::string())
    {
    }

    DynamicNodeSet::DynamicNodeSet(SoceDataType type)
        : DynamicNodeSet(std::string(), type)
    {
    }

    DynamicNodeSet::DynamicNodeSet(const std::string& name, SoceDataType type)
        : DynamicNode(SoceDataType::kTypeSet, name), elem_type_(type)
    {
    }

    std::string DynamicNodeSet::get_value()
    {
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (auto& i : elems_) {
            if (first) {
                first = false;
            }else{
                oss << ",";
            }
            oss << i->get_value();
        }
        oss << "]";
        return std::move(oss.str());
    }

    uint32_t DynamicNodeSet::serialize(ProtoIf* proto)
    {
        return SoceProto::write_list(proto,
                                     required_,
                                     setted_,
                                     false,
                                     elems_.size(),
                                     elem_type_,
                                     [&, this](uint32_t i)->uint32_t{
                                         return elems_[i]->serialize(proto);
                                     });
    }

    uint32_t DynamicNodeSet::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        uint32_t total_size = 0;
        uint32_t size = 0;
        uint32_t remain_len = 0;
        uint32_t elem_num = 0;

        size = proto->read_len(remain_len);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        size = proto->read_len(elem_num);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        size = proto->read_type(elem_type_);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        for (uint32_t i=0; i<elem_num; ++i) {
            auto n = DynamicNode::create(elem_type_);
            if (!n) {
                return 0;
            }

            size = n->deserialize(proto, false);
            if (size == 0) {
                return 0;
            }
            total_size += size;

            elems_.push_back(n);
        }

        setted_ = true;
        return total_size;
    }

    void DynamicNodeSet::reset()
    {
        setted_ = false;
        elems_.clear();
    }

    std::shared_ptr<DynamicNode> DynamicNodeSet::clone()
    {
        auto node = std::make_shared<DynamicNodeSet>(name_, elem_type_);
        node->set_required(required_);
        node->set_setted(setted_);

        for (auto& i : elems_) {
            node->get_elems().push_back(i->clone());
        }

        return node;
    }

    DynamicNodeMap::DynamicNodeMap()
        : DynamicNode(SoceDataType::kTypeMap, std::string())
    {
    }

    DynamicNodeMap::DynamicNodeMap(SoceDataType key_type,
                                   SoceDataType val_type)
        : DynamicNodeMap(std::string(), key_type, val_type)
    {
    }

    DynamicNodeMap::DynamicNodeMap(const std::string& name,
                                   SoceDataType key_type,
                                   SoceDataType val_type)
        : DynamicNode(SoceDataType::kTypeMap, name),
          key_type_(key_type),
          val_type_(val_type)
    {
    }

    std::string DynamicNodeMap::get_value()
    {
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (auto& i : elems_) {
            if (first) {
                first = false;
            }else{
                oss << ",";
            }
            oss << "{\"k\" : " << i.first->get_value()
                << ", \"v\" : " << i.second->get_value()
                << "}";
        }
        oss << "]";
        return std::move(oss.str());
    }

    uint32_t DynamicNodeMap::serialize(ProtoIf* proto)
    {
        return SoceProto::write_map(proto,
                                    required_,
                                    setted_,
                                    false,
                                    elems_.size(),
                                    key_type_,
                                    val_type_,
                                    [&, this]()->uint32_t{
                                        uint32_t size = 0;
                                        for (auto& i : elems_) {
                                            size_t elem_len = i.first->serialize(proto);
                                            if (elem_len == 0) {
                                                return 0;
                                            }
                                            size += elem_len;

                                            elem_len = i.second->serialize(proto);
                                            if (elem_len == 0) {
                                                return 0;
                                            }
                                            size += elem_len;
                                        }

                                        return size;
                                    });
    }

    uint32_t DynamicNodeMap::deserialize(ProtoIf* proto, bool with_schema)
    {
        (void) with_schema;

        uint32_t total_size = 0;
        uint32_t size = 0;
        uint32_t remain_len = 0;
        uint32_t elem_num = 0;

        size = proto->read_len(remain_len);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        size = proto->read_len(elem_num);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        size = proto->read_type(key_type_);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        size = proto->read_type(val_type_);
        if (size == 0) {
            return 0;
        }
        total_size += size;

        for (uint32_t i=0; i<elem_num; ++i) {
            auto key = DynamicNode::create(key_type_);
            if (!key) {
                return 0;
            }
            size = key->deserialize(proto, false);
            if (size == 0) {
                return 0;
            }
            total_size += size;

            auto val = DynamicNode::create(val_type_);
            if (!val) {
                return 0;
            }
            size = val->deserialize(proto, false);
            if (size == 0) {
                return 0;
            }
            total_size += size;

            elems_.push_back(std::make_pair(key, val));
        }

        setted_ = true;
        return total_size;
    }

    void DynamicNodeMap::reset()
    {
        setted_ = false;
        elems_.clear();
    }

    std::shared_ptr<DynamicNode> DynamicNodeMap::clone()
    {
        auto node = std::make_shared<DynamicNodeMap>(name_, key_type_, val_type_);
        node->set_required(required_);
        node->set_setted(setted_);

        for (auto& i : elems_) {
            node->get_elems().push_back(
                std::make_pair(i.first->clone(),
                               i.second->clone()));
        }

        return node;
    }

    DynamicNodeStruct::DynamicNodeStruct()
        : DynamicNode(SoceDataType::kTypeStruct)
    {
    }

    DynamicNodeStruct::DynamicNodeStruct(const std::string& name)
        : DynamicNode(SoceDataType::kTypeStruct, name)
    {
    }

    int DynamicNodeStruct::from_bytes(const char* data, size_t len)
    {
        return 0;
    }

    std::string DynamicNodeStruct::get_value()
    {
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (auto& i : nodes_) {
            if (first) {
                first = false;
            }else{
                oss << ",";
            }
            oss << i->to_json();
        }
        oss << "]";

        return std::move(oss.str());
    }

    uint32_t DynamicNodeStruct::serialize(ProtoIf* proto)
    {
        return SoceProto::write_struct(proto,
                                       required_,
                                       setted_,
                                       false,
                                       nodes_.size(),
                                       [&, this]()->uint32_t{
                                           uint32_t size = 0;
                                           for (auto& i : nodes_) {
                                               SoceDataType type = i->get_type();
                                               if (!i->get_setted()) {
                                                   type = SoceDataType::kTypeNull;
                                               }

                                               if (proto->write_type(type) == 0) {
                                                   return 0;
                                               }

                                               if (i->get_type() == SoceDataType::kTypeNull) {
                                                   continue;
                                               }

                                               uint32_t len = i->serialize(proto);
                                               if (len == 0) {
                                                   return 0;
                                               }
                                               size += len;
                                           }

                                           return size + nodes_.size();
                                       });

    }

    uint32_t DynamicNodeStruct::deserialize(ProtoIf* proto, bool with_schema)
    {
        uint32_t size = 0;
        if (with_schema) {
            size = deserialize_with_schema(proto);
        }
        else{
            size = deserialize_without_schema(proto);
        }

        return size;
    }

    void DynamicNodeStruct::reset()
    {
        setted_ = false;
        for (auto& i : nodes_) {
            i->reset();
        }
    }

    std::shared_ptr<DynamicNode> DynamicNodeStruct::clone()
    {
        auto node = std::make_shared<DynamicNodeStruct>(name_);
        node->set_required(required_);
        node->set_setted(setted_);

        for (auto& i : nodes_) {
            node->add(i->clone());
        }

        return node;
    }

    int DynamicNodeStruct::add(std::shared_ptr<DynamicNode> node)
    {
        if (!node) {
            return -1;
        }

        bool auto_index = false;
        if (node->get_name().empty()) {
            node->set_name(std::to_string(auto_index_));
            auto_index = true;
        }

        const std::string& name = node->get_name();
        if (names_.find(name) != names_.end()){
            return -1;
        }

        nodes_.push_back(node);
        names_.insert(name);
        if (auto_index) {
            ++auto_index_;
        }

        return 0;
    }

    int DynamicNodeStruct::replace(std::shared_ptr<DynamicNode> node)
    {
        if (!node || node->get_name().empty()) {
            return -1;
        }

        const std::string& name = node->get_name();
        if (names_.find(name) == names_.end()){
            return -1;
        }

        int replaced = -1;
        for (auto& i : nodes_) {
            if (i->get_name() == name) {
                i = node;
                replaced = 0;
                break;
            }
        }

        return replaced;
    }

    int DynamicNodeStruct::insert(const std::string& node_name,
                                  std::shared_ptr<DynamicNode> node)
    {
        if (!node) {
            return -1;
        }

        bool auto_index = false;
        if (node->get_name().empty()) {
            node->set_name(std::to_string(auto_index_));
            auto_index = true;
        }

        int inserted = -1;
        const std::string& name = node->get_name();
        if (names_.find(name) == names_.end()){
            return -1;
        }

        for (auto iter=nodes_.begin(); iter!=nodes_.end(); ++iter) {
            if ((*iter)->get_name() == node_name) {
                nodes_.insert(iter, node);
                names_.insert(name);
                inserted = 0;

                if (auto_index) {
                    ++auto_index;
                }

                break;
            }
        }

        return inserted;
    }

    std::shared_ptr<DynamicNode> DynamicNodeStruct::get(const std::string& path)
    {
        std::vector<std::string> pathes;
        if (soce::utils::StrHelper::split(path, ".", pathes)) {
            return std::shared_ptr<DynamicNode>();
        }

        return get(pathes, 0);
    }

    std::shared_ptr<DynamicNode> DynamicNodeStruct::get(const std::vector<std::string>& pathes, size_t index)
    {
        std::shared_ptr<DynamicNode> node;
        if (index >= pathes.size()) {
            return node;
        }

        for (auto& i : nodes_) {
            if (i->get_name() == pathes[index]) {
                if (index + 1 == pathes.size()) {
                    return i;
                }
                else if(i->get_type() != SoceDataType::kTypeStruct){
                    return node;
                }
                else{
                    DynamicNodeStruct* dns = static_cast<DynamicNodeStruct*>(i.get());
                    return dns->get(pathes, index + 1);
                }
            }
        }

        return node;
    }

    std::vector<std::shared_ptr<DynamicNode>>& DynamicNodeStruct::get_nodes()
    {
        return nodes_;
    }

    uint32_t DynamicNodeStruct::deserialize_with_schema(ProtoIf* proto)
    {
         return SoceProto::read_struct(proto,
                                       required_,
                                       false,
                                       nodes_.size(),
                                       [this](uint32_t valid_num, uint32_t schema_elem_num){
                                           for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                                               if (nodes_[i]->get_required()) {
                                                   return false;
                                               }
                                           }
                                           return true;
                                       },
                                       [&proto, this](uint32_t valid_num)->uint32_t{
                                           uint32_t index = 0;
                                           uint32_t total_size = 0;
                                           uint32_t elem_size = 0;
                                           for (auto& i : nodes_) {
                                               if (index >= valid_num) {
                                                   break;
                                               }

                                               SoceDataType type;
                                               if (proto->read_type(type) == 0) {
                                                   return 0;
                                               }
                                               total_size += 1;

                                               if (type != i->get_type()) {
                                                   return 0;
                                               }

                                               if (type == SoceDataType::kTypeNull) {
                                                   continue;
                                               }

                                               elem_size = i->deserialize(proto, true);
                                               if (elem_size == 0){
                                                   return 0;
                                               }

                                               total_size += elem_size;
                                           }

                                           return total_size;
                                       });
    }

    uint32_t DynamicNodeStruct::deserialize_without_schema(ProtoIf* proto)
    {
        uint32_t remain_len = 0;
        uint32_t elem_num = 0;
        uint32_t total_size = 0;
        uint32_t elem_size = 0;

        elem_size = proto->read_len(remain_len);
        if (elem_size == 0){
            return 0;
        }
        total_size += elem_size;

        elem_size = proto->read_len(elem_num);
        if (elem_num == 0) {
            return 0;
        }
        total_size += elem_size;

        for (size_t i=0; i<elem_num; ++i) {
            SoceDataType type;
            if (proto->read_type(type) == 0) {
                return 0;
            }
            total_size += 1;

            auto n = DynamicNode::create(type);
            if (!n) {
                return 0;
            }

            elem_size = n->deserialize(proto, false);
            if (elem_size == 0) {
                return 0;
            }
            total_size += elem_size;

            add(n);
        }

        setted_ = true;
        return total_size;
    }

} // namespace soce
} // namespace soce
