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

#include "node-struct.h"
#include "proto/soce-proto.h"

using soce::proto::SoceProto;

namespace soce{
namespace fadsdb{

    std::shared_ptr<NodeIf> NodeStruct::clone()
    {
        std::shared_ptr<NodeStruct> node(new NodeStruct);
        for (auto& i : elems_){
            node->add_elem(i->clone());
        }

        return node;
    }

    void NodeStruct::reset()
    {
        NodeIf::reset();
        for (auto e : elems_){
            e->reset();
        }
    }

    SoceDataType NodeStruct::get_type()
    {
        return SoceDataType::kTypeStruct;
    }

    uint32_t NodeStruct::serialize(ProtoIf* proto, bool with_type)
    {
        SoceProto sproto;
        return sproto.write_struct(proto,
                                   required_,
                                   has_set_,
                                   with_type,
                                   elems_.size(),
                                   [&, this]()->uint32_t{
                                       uint32_t total = 0;
                                       for (auto& i : elems_){
                                           uint32_t size = i->serialize(proto, true);
                                           if (size == 0){
                                               return 0;
                                           }
                                           total += size;
                                       }
                                       return total;
                                   });
    }

    uint32_t NodeStruct::deserialize(ProtoIf* proto, bool with_type)
    {
        SoceProto sproto;
        uint32_t size = sproto.read_struct(proto,
                                           required_,
                                           with_type,
                                           elems_.size(),
                                           [this](uint32_t valid_num, uint32_t schema_elem_num){
                                               for (uint32_t i=valid_num; i<schema_elem_num; ++i){
                                                   if (elems_[i]->required()){
                                                       return false;
                                                   }
                                               }
                                               return true;
                                           },
                                           [&, this](uint32_t valid_num)->uint32_t{
                                               uint32_t len = 0;
                                               uint32_t elem_len = 0;
                                               uint32_t index = 0;
                                               for (auto& i : elems_){
                                                   if (index >= valid_num){
                                                       break;
                                                   }
                                                   elem_len = i->deserialize(proto, true);
                                                   if (elem_len == 0){
                                                       return 0;
                                                   }
                                                   len += elem_len;
                                                   ++index;
                                               }

                                               return len;
                                           });
        uint32_t type_size = with_type ? 1 : 0;
        has_set_ = (size > type_size) ? true : false;
        return size;
    }

    void NodeStruct::add_elem(std::shared_ptr<NodeIf> elem)
    {
        elems_.push_back(elem);
    }

} // namespace fadsdb
} // namespace soce
