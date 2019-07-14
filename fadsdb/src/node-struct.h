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

#ifndef _SOCE_NODE_STRUCT_msBfkgAwme_H_
#define _SOCE_NODE_STRUCT_msBfkgAwme_H_

#include <vector>
#include "node-if.h"

namespace soce{
namespace fadsdb{

    class NodeStruct : public NodeIf
    {
    public:
        virtual std::shared_ptr<NodeIf> clone();
        virtual void reset();
        virtual SoceDataType get_type();
        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type);
        virtual uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type);
        void add_elem(std::shared_ptr<NodeIf> elem);

    private:
        std::vector<std::shared_ptr<NodeIf>> elems_;
    };

} // namespace fadsdb
} // namespace soce

#endif
