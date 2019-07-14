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

#ifndef _SOCE_NODE_LIST_hNR90tAqOn_H_
#define _SOCE_NODE_LIST_hNR90tAqOn_H_

#include <vector>
#include <algorithm>
#include "node-primary.h"
#include "proto/soce-proto.h"
#include "proto/binary-proto.h"
#include "log4rel/logger.h"

using soce::proto::SoceProto;

namespace soce{
namespace fadsdb{

    template <class T, SoceDataType val_type>
    class NodeList : public NodeIf
    {
    public:
        NodeList<T, val_type>()
            {
                update_funcs_["set"] = std::bind(&NodeList<T,val_type>::update_set,
                                                 this,
                                                 std::placeholders::_1);
                update_funcs_["push_back"] = std::bind(&NodeList<T,val_type>::update_push_back,
                                                       this,
                                                       std::placeholders::_1);
                update_funcs_["pop_back"] = std::bind(&NodeList<T,val_type>::update_pop_back,
                                                      this,
                                                      std::placeholders::_1);
                update_funcs_["clear"] = std::bind(&NodeList<T,val_type>::update_clear,
                                                   this,
                                                   std::placeholders::_1);
                update_funcs_["shuffle"] = std::bind(&NodeList<T,val_type>::update_shuffle,
                                                     this,
                                                     std::placeholders::_1);

                select_funcs_["size"] = std::bind(&NodeList<T,val_type>::select_size, this);
                select_funcs_["max"] = std::bind(&NodeList<T,val_type>::select_max, this);
                select_funcs_["min"] = std::bind(&NodeList<T,val_type>::select_min, this);
                select_funcs_["avg"] = std::bind(&NodeList<T,val_type>::select_avg, this);
                select_funcs_["sum"] = std::bind(&NodeList<T,val_type>::select_sum, this);
            }

        virtual SoceDataType get_type()
            {
                return SoceDataType::kTypeList;
            }

        void set_val(const std::vector<T>& val)
            {
                val_ = val;
                has_set_ = true;
            }

        void show()
            {
                for (auto i : val_){
                    SOCE_INFO << _S("value", i);
                }
            }

        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                SoceProto sproto;
                return sproto.write_list(proto,
                                         required_,
                                         has_set_,
                                         with_type,
                                         val_.size(),
                                         val_type,
                                         [&, this](uint32_t index)->uint32_t{
                                             return this->write(proto, val_type, false, val_[index]);
                                         });
            }

        virtual uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                val_.clear();
                SoceProto sproto;
                uint32_t size = sproto.read_list(proto,
                                                 required_,
                                                 with_type,
                                                 val_type,
                                                 [this](uint32_t size){
                                                     val_.resize(size);
                                                 },
                                                 [&, this](uint32_t index)->uint32_t{
                                                     //TODO:
                                                     T val;
                                                     uint32_t rc = this->read(proto, val_type, false, val);
                                                     if (rc){
                                                         val_[index] = val;
                                                     }
                                                     return rc;
                                                 });

                uint32_t type_size = with_type ? 1 : 0;
                has_set_ = (size > type_size) ? true : false;
                return size;
            }

        int update_set(const std::vector<std::string>& params)
            {
                if (params.size() != 1){
                    return -1;
                }

                soce::proto::BinaryProto bp;
                bp.init(const_cast<char*>(params.front().c_str()), params.front().size(), false);
                if (deserialize(&bp, true) == 0){
                    return -1;
                }

                has_set_ = true;
                return 0;
            }

        int update_push_back(const std::vector<std::string>& params)
            {
                if (params.size() != 1 || !has_set_){
                    return -1;
                }

                return _update_push_back(params);
            }

        virtual int _update_push_back(const std::vector<std::string>& params) = 0;

        int update_pop_back(const std::vector<std::string>& params)
            {
                if (!params.empty() || !has_set_){
                    return -1;
                }

                val_.pop_back();
                return 0;
            }

        int update_clear(const std::vector<std::string>& params)
            {
                if (!params.empty() || !has_set_){
                    return -1;
                }

                val_.clear();
                return 0;
            }

        int update_shuffle(const std::vector<std::string>& params)
            {
                if (!params.empty() || !has_set_){
                    return -1;
                }

                std::random_shuffle (val_.begin(), val_.end());
                return 0;
            }

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

        std::vector<T> val_;
    };

    template <class T, SoceDataType val_type>
    class NodeListNum : public NodeList<T, val_type>
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

        virtual std::shared_ptr<NodeIf> select_max()
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

        virtual std::shared_ptr<NodeIf> select_min()
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

        virtual std::shared_ptr<NodeIf> select_avg()
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

        virtual std::shared_ptr<NodeIf> select_sum()
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

    class NodeListBool : public NodeList<bool, SoceDataType::kTypeBool>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeListBool);
            }

        virtual int _update_push_back(const std::vector<std::string>& params)
            {
                int rc = 0;
                if (params.front() == "true"){
                    val_.push_back(true);
                }
                else if (params.front() == "false"){
                    val_.push_back(false);
                }
                else{
                    rc = -1;
                }

                return rc;
            }
    };

    class NodeListInt32 : public NodeListNum<int32_t, SoceDataType::kTypeInt32>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeListInt32);
            }

        virtual int _update_push_back(const std::vector<std::string>& params)
            {
                val_.push_back(std::stoi(params.front()));
                return 0;
            }
    };

    class NodeListInt64 : public NodeListNum<int64_t, SoceDataType::kTypeInt64>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeListInt64);
            }

        virtual int _update_push_back(const std::vector<std::string>& params)
            {
                val_.push_back(std::stoll(params.front()));
                return 0;
            }
    };

    class NodeListFloat : public NodeListNum<float, SoceDataType::kTypeFloat>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeListFloat);
            }

        virtual int _update_push_back(const std::vector<std::string>& params)
            {
                val_.push_back(std::stof(params.front()));
                return 0;
            }
    };

    class NodeListDouble : public NodeListNum<double, SoceDataType::kTypeDouble>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeListDouble);
            }

        virtual int _update_push_back(const std::vector<std::string>& params)
            {
                val_.push_back(std::stod(params.front()));
                return 0;
            }
    };

    class NodeListString : public NodeList<std::string, SoceDataType::kTypeString>
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                return std::shared_ptr<NodeIf>(new NodeListString);
            }

        virtual int _update_push_back(const std::vector<std::string>& params)
            {
                val_.push_back(params.front());
                return 0;
            }
    };

    class NodeListCustom : public NodeIf
    {
    public:
        virtual std::shared_ptr<NodeIf> clone()
            {
                std::shared_ptr<NodeListCustom> node(new NodeListCustom);
                node->set_prototype(prototype_->clone());
                return node;
            }

        virtual SoceDataType get_type()
            {
                return SoceDataType::kTypeList;
            }

        void set_prototype(std::shared_ptr<NodeIf> prototype)
            {
                prototype_ = prototype;
            }

        void push_back(std::shared_ptr<NodeIf> node)
            {
                val_.push_back(node);
                has_set_ = true;
            }

        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                SoceProto sproto;
                return sproto.write_list(proto,
                                         required_,
                                         has_set_,
                                         with_type,
                                         val_.size(),
                                         prototype_->get_type(),
                                         [&, this](uint32_t index)->uint32_t{
                                             return val_[index]->serialize(proto, false);
                                         });
            }

        virtual uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type)
            {
                val_.clear();

                SoceProto sproto;
                uint32_t size = sproto.read_list(proto,
                                                 required_,
                                                 with_type,
                                                 prototype_->get_type(),
                                                 [this](uint32_t size){
                                                     val_.resize(size);
                                                     for (uint32_t i=0; i<size; ++i){
                                                         val_[i] = prototype_->clone();
                                                     }
                                                 },
                                                 [&, this](uint32_t index)->uint32_t{
                                                     return val_[index]->deserialize(proto, false);
                                                 });

                uint32_t type_size = with_type ? 1 : 0;
                has_set_ = (size > type_size) ? true : false;
                return size;
            }

        void show()
            {
                SOCE_INFO << _S("size", val_.size());
            }

        std::vector<std::shared_ptr<NodeIf>> val_;
        std::shared_ptr<NodeIf> prototype_;
    };

} // namespace fadsdb
} // namespace soce

#endif
