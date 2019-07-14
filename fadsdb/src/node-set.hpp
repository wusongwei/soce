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

#ifndef _SOCE_NODE_SET_cuIRsJo3co_H_
#define _SOCE_NODE_SET_cuIRsJo3co_H_

#include <unordered_set>
#include "node-if.h"
#include "proto/soce-proto.h"
#include "log4rel/logger.h"

using soce::proto::SoceProto;

namespace soce{
namespace fadsdb{

    template <class T, SoceDataType val_type>
    class NodeSet : public NodeIf
    {
    public:
        NodeSet<T, val_type>()
            {
                update_funcs_["push_back"] = std::bind(&NodeSet<T,val_type>::update_insert,
                                                       this,
                                                       std::placeholders::_1);
                select_funcs_["size"] = std::bind(&NodeSet<T,val_type>::select_size, this);
                select_funcs_["max"] = std::bind(&NodeSet<T,val_type>::select_max, this);
                select_funcs_["min"] = std::bind(&NodeSet<T,val_type>::select_min, this);
                select_funcs_["avg"] = std::bind(&NodeSet<T,val_type>::select_avg, this);
                select_funcs_["sum"] = std::bind(&NodeSet<T,val_type>::select_sum, this);
            }

        virtual SoceDataType get_type()
            {
                return SoceDataType::kTypeSet;
            }

        void set_val(const std::unordered_set<T>& val)
            {
                val_ = val;
            }

        void show()
            {
                for (auto& i : val_){
                    SOCE_INFO << _S("value", i);
                }
            }

        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                SoceProto sproto;
                return sproto.write_set(proto,
                                        required_,
                                        has_set_,
                                        with_type,
                                        val_.size(),
                                        val_type,
                                        [&, this]()->uint32_t{
                                            uint32_t total = 0;
                                            for (auto& i : val_){
                                                uint32_t size = this->write(proto, val_type, false, i);
                                                if (size == 0){
                                                    return 0;
                                                }
                                                total += size;
                                            }
                                            return total;
                                        });
            }

        virtual uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                val_.clear();

                SoceProto sproto;
                uint32_t size = sproto.read_set(proto,
                                                required_,
                                                with_type,
                                                val_type,
                                                [&, this]()->uint32_t{
                                                    T val;
                                                    uint32_t size = this->read(proto, val_type, false, val);
                                                    if (size == 0){
                                                        return 0;
                                                    }
                                                    val_.insert(val);
                                                    return size;
                                                });

                uint32_t type_size = with_type ? 1 : 0;
                has_set_ = (size > type_size) ? true : false;
                return size;
            }

        int update_insert(const std::vector<std::string>& params)
            {
                if (params.size() != 1 || !has_set_){
                    return -1;
                }

                return _update_insert(params);
            }

        virtual int _update_insert(const std::vector<std::string>& params) = 0;

        std::shared_ptr<NodeIf> select_size()
            {
                std::shared_ptr<NodeInt32> node(new NodeInt32);
                node->set_val(val_.size());
                return node;
            }

        virtual std::shared_ptr<NodeIf> select_max()
            {
                return std::shared_ptr<NodeIf>();
            }

        virtual std::shared_ptr<NodeIf> select_min()
            {
                return std::shared_ptr<NodeIf>();
            }

        virtual std::shared_ptr<NodeIf> select_avg()
            {
                return std::shared_ptr<NodeIf>();
            }

        virtual std::shared_ptr<NodeIf> select_sum()
            {
                return std::shared_ptr<NodeIf>();
            }

        std::unordered_set<T> val_;
    };

    template <class T, SoceDataType val_type>
    class NodeSetNum : public NodeSet<T, val_type>
    {
    public:
        std::shared_ptr<NodeIf> select_funcs(std::function<T()> func)
            {
                std::shared_ptr<NodeIf> rc;

                if (val_type != SoceDataType::kTypeInt32
                    && val_type != SoceDataType::kTypeInt64
                    && val_type != SoceDataType::kTypeFloat
                    && val_type != SoceDataType::kTypeDouble){
                    return rc;
                }

                T val = func();

                if (val_type == SoceDataType::kTypeInt32){
                    std::shared_ptr<NodeInt32> node(new NodeInt32);
                    if (!this->val_.empty()){
                        node->set_val(val);
                    }
                    rc = node;
                }
                else if (val_type == SoceDataType::kTypeInt64){
                    std::shared_ptr<NodeInt64> node(new NodeInt64);
                    if (!this->val_.empty()){
                        node->set_val(val);
                    }
                    rc = node;
                }
                else if (val_type == SoceDataType::kTypeFloat){
                    std::shared_ptr<NodeFloat> node(new NodeFloat);
                    if (!this->val_.empty()){
                        node->set_val(val);
                    }
                    rc = node;
                }
                else if (val_type == SoceDataType::kTypeDouble){
                    std::shared_ptr<NodeDouble> node(new NodeDouble);
                    if (!this->val_.empty()){
                        node->set_val(val);
                    }
                    rc = node;
                }

                return rc;
            }

        std::shared_ptr<NodeIf> select_max()
            {
                return select_funcs([this]()->T{
                        T max = 0;
                        for (auto& i : this->val_){
                            if (max < i){
                                max = i;
                            }
                        }

                        return max;
                    });
            }

        std::shared_ptr<NodeIf> select_min()
            {
                return select_funcs([this]()->T{
                        T min = 0;
                        for (auto& i : this->val_){
                            if (min > i){
                                min = i;
                            }
                        }

                        return min;
                    });
            }

        std::shared_ptr<NodeIf> select_avg()
            {
                return select_funcs([this]()->T{
                        T avg = 0;
                        for (auto& i : this->val_){
                            avg += i;
                        }
                        if (!this->val_.empty()){
                            avg /= this->val_.size();
                        }

                        return avg;
                    });
            }

        std::shared_ptr<NodeIf> select_sum()
            {
                return select_funcs([this]()->T{
                        T sum = 0;
                        for (auto& i : this->val_){
                            sum += i;
                        }
                        return sum;
                    });
            }
    };

    class NodeSetBool : public NodeSet<bool, SoceDataType::kTypeBool>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeSetBool);
            }

        virtual int _update_insert(const std::vector<std::string>& params)
            {
                int rc = 0;
                if (params.front() == "true"){
                    val_.insert(true);
                }
                else if (params.front() == "false"){
                    val_.insert(false);
                }
                else{
                    rc = -1;
                }

                return rc;
            }
    };

    class NodeSetInt32 : public NodeSetNum<int32_t, SoceDataType::kTypeInt32>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeSetInt32);
            }

        virtual int _update_insert(const std::vector<std::string>& params)
            {
                val_.insert(std::stoi(params.front()));
                return 0;
            }
    };

    class NodeSetInt64 : public NodeSetNum<int64_t, SoceDataType::kTypeInt64>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeSetInt64);
            }

        virtual int _update_insert(const std::vector<std::string>& params)
            {
                val_.insert(std::stoll(params.front()));
                return 0;
            }
    };

    class NodeSetFloat : public NodeSetNum<float, SoceDataType::kTypeFloat>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeSetFloat);
            }

        virtual int _update_insert(const std::vector<std::string>& params)
            {
                val_.insert(std::stof(params.front()));
                return 0;
            }
    };

    class NodeSetDouble : public NodeSetNum<double, SoceDataType::kTypeDouble>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeSetDouble);
            }

        virtual int _update_insert(const std::vector<std::string>& params)
            {
                val_.insert(std::stod(params.front()));
                return 0;
            }
    };

    class NodeSetString : public NodeSet<std::string, SoceDataType::kTypeString>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeSetString);
            }

        virtual int _update_insert(const std::vector<std::string>& params)
            {
                val_.insert(params.front());
                return 0;
            }
    };

    class NodeSetCustom : public NodeIf
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                std::shared_ptr<NodeSetCustom> node(new NodeSetCustom);
                node->set_prototype(prototype_->clone());
                return node;
            }

        virtual SoceDataType get_type()
            {
                return SoceDataType::kTypeSet;
            }

        void set_prototype(std::shared_ptr<NodeIf> prototype)
            {
                prototype_ = prototype;
            }

        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type){
            SoceProto sproto;
            return sproto.write_set(proto,
                                    required_,
                                    has_set_,
                                    with_type,
                                    val_.size(),
                                    prototype_->get_type(),
                                    [&, this]()->uint32_t{
                                        uint32_t total = 0;
                                        for (auto& i : val_){
                                            uint32_t size = i->serialize(proto, false);
                                            if (size == 0){
                                                return 0;
                                            }
                                            total += size;
                                        }
                                        return total;
                                    });
        }

        virtual uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type){
            val_.clear();

            SoceProto sproto;
            uint32_t size = sproto.read_set(proto,
                                            required_,
                                            with_type,
                                            prototype_->get_type(),
                                            [&, this]()->uint32_t{
                                                std::shared_ptr<NodeIf> node = prototype_->clone();
                                                uint32_t size = node->deserialize(proto, false);
                                                if (size == 0){
                                                    return 0;
                                                }
                                                val_.insert(node);
                                                return size;
                                            });

            uint32_t type_size = with_type ? 1 : 0;
            has_set_ = (size > type_size) ? true : false;
            return size;
        }

        std::unordered_set<std::shared_ptr<NodeIf>> val_;
        std::shared_ptr<NodeIf> prototype_;
    };

} // namespace fadsdb
} // namespace soce

#endif
