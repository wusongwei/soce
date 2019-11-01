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

#include "proto/dynamic-parser.h"

namespace soce {
namespace proto{

    std::shared_ptr<DynamicNodeStruct> DynamicParser::parse(const char* data, size_t len)
    {
        BinaryProto bp;
        bp.init(const_cast<char*>(data), len, false);
        auto node = std::make_shared<DynamicNodeStruct>("0");
        if (node->deserialize(&bp, false) == 0) {
            node.reset();
        }
        return node;
    }


    int DynamicParser::parse(const char* data, size_t len, std::shared_ptr<DynamicNode> node)
    {
        if (node->get_type() != SoceDataType::kTypeStruct) {
            return -1;
        }

        BinaryProto bp;
        bp.init(const_cast<char*>(data), len, false);
        return node->deserialize(&bp, true) == 0 ? -1 : 0;
    }

} // namespace proto
} // namespace soce
