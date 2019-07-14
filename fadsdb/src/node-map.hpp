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

#ifndef _SOCE_NODE_MAP_xrOIW8TKdd_H_
#define _SOCE_NODE_MAP_xrOIW8TKdd_H_

#include <unordered_map>
#include <memory>
#include "node-if.h"
#include "proto/soce-proto.h"
#include "log4rel/logger.h"

namespace soce{
namespace fadsdb{

    template <class TKEY, class TVAL, SoceDataType key_type, SoceDataType val_type>
    class NodeMap : public NodeIf
    {
    public:
        NodeMap<TKEY, TVAL, key_type, val_type>()
            {
                select_funcs_["size"] = std::bind(&NodeMap<TKEY, TVAL, key_type, val_type>::select_size, this);
            }

        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeMap<TKEY, TVAL, key_type, val_type>());
            }

        virtual SoceDataType get_type()
            {
                return SoceDataType::kTypeMap;
            }

        void set_val(const std::unordered_map<TKEY, TVAL>& val)
            {
                val_ = val;
            }

        void show()
            {
                for (auto& i : val_){
                    SOCE_INFO << _S(i.first, i.second);
                }
            }

        uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                SoceProto sproto;
                return sproto.write_map(proto,
                                        required_,
                                        has_set_,
                                        with_type,
                                        val_.size(),
                                        key_type,
                                        val_type,
                                        [&, this]()->uint32_t{
                                            uint32_t total = 0;
                                            for (auto& i : val_){
                                                uint32_t size = 0;
                                                size = this->write(proto, key_type, false, i.first);
                                                if (size == 0){
                                                    return 0;
                                                }
                                                total += size;

                                                size = this->write(proto, key_type, false, i.second);
                                                if (size == 0){
                                                    return 0;
                                                }
                                                total += size;
                                            }
                                            return total;
                                        });
            }

        uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                val_.clear();

                SoceProto sproto;
                uint32_t size = sproto.read_map(proto,
                                                required_,
                                                with_type,
                                                key_type,
                                                val_type,
                                                [&, this]()->uint32_t{
                                                    uint32_t total = 0;
                                                    uint32_t size = 0;
                                                    TKEY key;
                                                    size = this->read(proto, key_type, false, key);
                                                    if (size == 0){
                                                        return 0;
                                                    }
                                                    total += size;

                                                    TVAL val;
                                                    size = this->read(proto, val_type, false, val);
                                                    if (size == 0){
                                                        return 0;
                                                    }
                                                    val_[key] = val;
                                                    total += size;
                                                    return total;
                                                });

                uint32_t type_size = with_type ? 1 : 0;
                has_set_ = (size > type_size) ? true : false;
                return size;
            }

        std::shared_ptr<NodeIf> select_size()
            {
                std::shared_ptr<NodeInt32> node(new NodeInt32);
                node->set_val(val_.size());
                return node;
            }

        std::unordered_map<TKEY, TVAL> val_;
    };

    using NodeMapBoolBool = NodeMap<bool, bool, SoceDataType::kTypeBool, SoceDataType::kTypeBool>;
    using NodeMapBoolInt32 = NodeMap<bool, int32_t, SoceDataType::kTypeBool, SoceDataType::kTypeInt32>;
    using NodeMapBoolInt64 = NodeMap<bool, int64_t, SoceDataType::kTypeBool, SoceDataType::kTypeInt64>;
    using NodeMapBoolFloat = NodeMap<bool, float, SoceDataType::kTypeBool, SoceDataType::kTypeFloat>;
    using NodeMapBoolDouble = NodeMap<bool, double, SoceDataType::kTypeBool, SoceDataType::kTypeDouble>;
    using NodeMapBoolString = NodeMap<bool, std::string, SoceDataType::kTypeBool, SoceDataType::kTypeString>;

    using NodeMapInt32Bool = NodeMap<int32_t, bool, SoceDataType::kTypeInt32, SoceDataType::kTypeBool>;
    using NodeMapInt32Int32 = NodeMap<int32_t, int32_t, SoceDataType::kTypeInt32, SoceDataType::kTypeInt32>;
    using NodeMapInt32Int64 = NodeMap<int32_t, int64_t, SoceDataType::kTypeInt32, SoceDataType::kTypeInt64>;
    using NodeMapInt32Float = NodeMap<int32_t, float, SoceDataType::kTypeInt32, SoceDataType::kTypeFloat>;
    using NodeMapInt32Double = NodeMap<int32_t, double, SoceDataType::kTypeInt32, SoceDataType::kTypeDouble>;
    using NodeMapInt32String = NodeMap<int32_t, std::string, SoceDataType::kTypeInt32, SoceDataType::kTypeString>;

    using NodeMapInt64Bool = NodeMap<int64_t, bool, SoceDataType::kTypeInt64, SoceDataType::kTypeBool>;
    using NodeMapInt64Int32 = NodeMap<int64_t, int32_t, SoceDataType::kTypeInt64, SoceDataType::kTypeInt32>;
    using NodeMapInt64Int64 = NodeMap<int64_t, int64_t, SoceDataType::kTypeInt64, SoceDataType::kTypeInt64>;
    using NodeMapInt64Float = NodeMap<int64_t, float, SoceDataType::kTypeInt64, SoceDataType::kTypeFloat>;
    using NodeMapInt64Double = NodeMap<int64_t, double, SoceDataType::kTypeInt64, SoceDataType::kTypeDouble>;
    using NodeMapInt64String = NodeMap<int64_t, std::string, SoceDataType::kTypeInt64, SoceDataType::kTypeString>;

    using NodeMapFloatBool = NodeMap<float, bool, SoceDataType::kTypeFloat, SoceDataType::kTypeBool>;
    using NodeMapFloatInt32 = NodeMap<float, int32_t, SoceDataType::kTypeFloat, SoceDataType::kTypeInt32>;
    using NodeMapFloatInt64 = NodeMap<float, int64_t, SoceDataType::kTypeFloat, SoceDataType::kTypeInt64>;
    using NodeMapFloatFloat = NodeMap<float, float, SoceDataType::kTypeFloat, SoceDataType::kTypeFloat>;
    using NodeMapFloatDouble = NodeMap<float, double, SoceDataType::kTypeFloat, SoceDataType::kTypeDouble>;
    using NodeMapFloatString = NodeMap<float, std::string, SoceDataType::kTypeFloat, SoceDataType::kTypeString>;

    using NodeMapDoubleBool = NodeMap<double, bool, SoceDataType::kTypeDouble, SoceDataType::kTypeBool>;
    using NodeMapDoubleInt32 = NodeMap<double, int32_t, SoceDataType::kTypeDouble, SoceDataType::kTypeInt32>;
    using NodeMapDoubleInt64 = NodeMap<double, int64_t, SoceDataType::kTypeDouble, SoceDataType::kTypeInt64>;
    using NodeMapDoubleFloat = NodeMap<double, float, SoceDataType::kTypeDouble, SoceDataType::kTypeFloat>;
    using NodeMapDoubleDouble = NodeMap<double, double, SoceDataType::kTypeDouble, SoceDataType::kTypeDouble>;
    using NodeMapDoubleString = NodeMap<double, std::string, SoceDataType::kTypeDouble, SoceDataType::kTypeString>;

    using NodeMapStringBool = NodeMap<double, bool, SoceDataType::kTypeString, SoceDataType::kTypeBool>;
    using NodeMapStringInt32 = NodeMap<double, int32_t, SoceDataType::kTypeString, SoceDataType::kTypeInt32>;
    using NodeMapStringInt64 = NodeMap<double, int64_t, SoceDataType::kTypeString, SoceDataType::kTypeInt64>;
    using NodeMapStringFloat = NodeMap<double, float, SoceDataType::kTypeString, SoceDataType::kTypeFloat>;
    using NodeMapStringDouble = NodeMap<double, double, SoceDataType::kTypeString, SoceDataType::kTypeDouble>;
    using NodeMapStringString = NodeMap<double, std::string, SoceDataType::kTypeString, SoceDataType::kTypeString>;

    template <class T, SoceDataType key_type>
    class NodeMapCustom : public NodeIf
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                std::shared_ptr<NodeMapCustom<T, key_type>> node(new NodeMapCustom<T, key_type>());
                node->set_prototype(prototype_->clone());
                return node;
            }

        virtual SoceDataType get_type()
            {
                return SoceDataType::kTypeMap;
            }

        void set_prototype(std::shared_ptr<NodeIf> prototype)
            {
                prototype_ = prototype;
            }

        void set_val(const std::unordered_map<T, std::shared_ptr<NodeIf>>& val)
            {
                has_set_ = true;
                val_ = val;
            }

        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                SoceProto sproto;
                return sproto.write_map(proto,
                                        required_,
                                        has_set_,
                                        with_type,
                                        val_.size(),
                                        key_type,
                                        prototype_->get_type(),
                                        [&, this]()->uint32_t{
                                            uint32_t total = 0;
                                            for (auto& i : val_){
                                                uint32_t size = 0;
                                                size = this->write(proto, key_type, false, i.first);
                                                if (size == 0){
                                                    return 0;
                                                }
                                                total += size;

                                                size = i.second->serialize(proto, false);
                                                if (size == 0){
                                                    return 0;
                                                }
                                                total += size;
                                            }
                                            return total;
                                        });
            }

        uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                val_.clear();

                SoceProto sproto;
                uint32_t size = sproto.read_map(proto,
                                                required_,
                                                with_type,
                                                key_type,
                                                prototype_->get_type(),
                                                [&, this]()->uint32_t{
                                                    uint32_t total = 0;
                                                    uint32_t size = 0;
                                                    T key;
                                                    size = this->read(proto, key_type, false, key);
                                                    if (size == 0){
                                                        return 0;
                                                    }
                                                    total += size;

                                                    std::shared_ptr<NodeIf> node = prototype_->clone();
                                                    size = node->deserialize(proto, false);
                                                    if (size == 0){
                                                        return 0;
                                                    }
                                                    val_[key] = node;
                                                    total += size;
                                                    return total;
                                                });

                uint32_t type_size = with_type ? 1 : 0;
                has_set_ = (size > type_size) ? true : false;
                return size;
            }

        std::unordered_map<T, std::shared_ptr<NodeIf>> val_;
        std::shared_ptr<NodeIf> prototype_;
    };

    using NodeMapCustomBool = NodeMapCustom<bool, SoceDataType::kTypeBool>;
    using NodeMapCustomInt32 = NodeMapCustom<int32_t, SoceDataType::kTypeInt32>;
    using NodeMapCustomInt64 = NodeMapCustom<int64_t, SoceDataType::kTypeInt64>;
    using NodeMapCustomFloat = NodeMapCustom<float, SoceDataType::kTypeFloat>;
    using NodeMapCustomDouble = NodeMapCustom<double, SoceDataType::kTypeDouble>;
    using NodeMapCustomString = NodeMapCustom<std::string, SoceDataType::kTypeString>;

} // namespace fadsdb
} // namespace soce

#endif
