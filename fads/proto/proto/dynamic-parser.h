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

#ifndef _SOCE_PROTO_DYNAMIC_PARSER_bwMxHL2nPK_H_
#define _SOCE_PROTO_DYNAMIC_PARSER_bwMxHL2nPK_H_

#include <string>
#include "dynamic-node.h"

namespace soce {
namespace proto {

    class DynamicParser
    {
    public:
        /**
         * create the dynamic node and parse the given data
         */
        std::shared_ptr<DynamicNodeStruct> parse(const char* data, size_t len);

        /**
         * parse the given data with given dynamic node.
         */
        int parse(const char* data, size_t len, std::shared_ptr<DynamicNode> node);
    };

} // namespace soce
} // namespace soce

#endif
