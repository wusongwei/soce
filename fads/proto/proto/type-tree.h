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

#ifndef _SOCE_TYPE_TREE_PDmXbmqXow_H_
#define _SOCE_TYPE_TREE_PDmXbmqXow_H_

#include <stdint.h>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include "utils/index-list.hpp"
#include "proto-if.h"

namespace soce{
namespace proto{

    class TypeTree
    {
    public:
        class TypeNode;

    public:
        int add(const std::string& father, const std::string& name, SoceDataType type);
        int add(const std::string& father, const std::string& name, const std::string& type);
        int transfer(const std::string& struct_name, const std::string& path_str, std::string& path_index, int& val_type);
        void show();

    private:
        soce::utils::IndexList<std::string, TypeNode> nodes_;
    };

    class TypeTree::TypeNode
    {
    public:
        using Node = struct{
            SoceDataType type;
            std::string type_str;
            uint32_t index;
        };

        void push_back(const std::string& key, const Node& node){
            nodes_.push_back(key, node);
        }

        uint32_t size(){
            return nodes_.size();
        }

        int get(const std::string& key, Node** pnode)
        {
            return nodes_.get(key, pnode);
        }

        void show();

    private:
        soce::utils::IndexList<std::string, Node> nodes_;
    };

} // namespace proto
} // namespace soce

#endif
