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

#ifndef _SOCE_NODE_PRIMARY_EWlSod0MEO_H_
#define _SOCE_NODE_PRIMARY_EWlSod0MEO_H_

#include <string>
#include "node-if.h"
#include "log4rel/logger.h"

namespace soce{
namespace fadsdb{

#define DEF_NODE(N, T)                                                  \
    class Node##N : public NodeIf                                       \
    {                                                                   \
    public:                                                             \
        Node##N();                                                      \
        void set_val(T val){                                            \
            val_ = val;                                                 \
            has_set_ = true;                                            \
        }                                                               \
        SoceDataType get_type(){                                        \
            return SoceDataType::kType##N;                              \
        }                                                               \
        virtual std::shared_ptr<NodeIf> clone();                        \
        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type); \
        virtual uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type); \
        void show(){SOCE_INFO << _S("value", val_);}                    \
                                                                        \
        bool eq(const std::string& val);                                \
        bool neq(const std::string& val);                               \
        bool gt(const std::string& val);                                \
        bool egt(const std::string& val);                               \
        bool lt(const std::string& val);                                \
        bool elt(const std::string& val);                               \
                                                                        \
        int set(const std::vector<std::string>& params);                \
        int unset(const std::vector<std::string>& params);              \
                                                                        \
        T val_;

#define END_DEF_NODE                            \
    };

    DEF_NODE(Bool, bool)
    END_DEF_NODE

    DEF_NODE(Int32, int32_t)
    END_DEF_NODE

    DEF_NODE(Int64, int64_t)
    END_DEF_NODE

    DEF_NODE(Float, float)
    END_DEF_NODE

    DEF_NODE(Double, double)
    END_DEF_NODE

    DEF_NODE(String, std::string)
        bool match(const std::string& str);
        std::shared_ptr<NodeIf> select_size(const std::vector<std::string>& params);
    END_DEF_NODE

#undef DEF_NODE
#undef END_DEF_NODE

} // namespace fadsdb
} // namespace soce

#endif
