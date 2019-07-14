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

#include <iostream>
#include "proto/type-tree.h"
#include "utils/str-helper.h"

using std::string;
using std::vector;
using std::ostringstream;
using std::cout;
using std::endl;

namespace soce{
namespace proto{

    int TypeTree::add(const string& father, const string& name, SoceDataType type)
    {
        if (type == SoceDataType::kTypeStruct){
            if (!nodes_.find(name)){
                return -1;
            }
        }

        if (!nodes_.find(father)){
            nodes_.push_back(father, TypeNode());
        }

        TypeNode* pnode = NULL;
        nodes_.get(father, &pnode);

        TypeTree::TypeNode::Node node;
        node.type = type;
        node.index = pnode->size();
        pnode->push_back(name, node);

        return 0;
    }

    int TypeTree::add(const string& father, const string& name, const string& type)
    {
        TypeTree::TypeNode* pnode = NULL;
        if (nodes_.get(type, &pnode)){
            return -1;
        }

        if (!nodes_.find(father)){
            nodes_.push_back(father, TypeNode());
        }

        pnode = NULL;
        nodes_.get(father, &pnode);

        TypeTree::TypeNode::Node node;
        node.type = SoceDataType::kTypeStruct;
        node.type_str = type;
        node.index = pnode->size();
        pnode->push_back(name, node);

        return 0;
    }

    void TypeTree::show()
    {
        for (auto iter = nodes_.begin();
             iter != nodes_.end(); ++iter){
            cout << "key = " << iter->first << endl;
            iter->second.show();
        }
    }

    int TypeTree::transfer(const string& struct_name, const string& path_str, string& path_index, int& val_type)
    {
        if (path_str == "_rand100") {
            path_index = "_rand100";
            val_type = (int)SoceDataType::kTypeInt32;
            return 0;
        }

        TypeTree::TypeNode* pnode = NULL;
        if (nodes_.get(struct_name, &pnode)){
            return -1;
        }

        ostringstream oss;
        vector<string> vec_path;
        if (soce::utils::StrHelper::split(path_str, ".", vec_path) || vec_path.empty()){
            return -1;
        }
        uint32_t count = vec_path.size();
        TypeTree::TypeNode::Node* pn = NULL;
        for (uint32_t i=0; i<count; ++i){
            if (pnode->get(vec_path[i], &pn)){
                return -1;
            }

            if (i != 0){
                oss << ".";
            }
            oss << pn->index;

            if (i != count - 1){
                if (pn->type != SoceDataType::kTypeStruct){
                    return -1;
                }

                if (nodes_.get(pn->type_str, &pnode)){
                    return -1;
                }
            }
            else{
                if (pn->type != SoceDataType::kTypeBool
                    && pn->type != SoceDataType::kTypeInt32
                    && pn->type != SoceDataType::kTypeInt64
                    && pn->type != SoceDataType::kTypeFloat
                    && pn->type != SoceDataType::kTypeDouble
                    && pn->type != SoceDataType::kTypeString
                    && pn->type != SoceDataType::kTypeEnum){
                    return -1;
                }
                val_type = (int)pn->type;
            }
        }

        path_index = oss.str();
        return 0;
    }

    void TypeTree::TypeNode::show()
    {
        for (auto iter = nodes_.begin(); iter != nodes_.end(); ++iter)
        {
            cout << "Node key = " << iter->first
                 << ", index = " << iter->second.index
                 << ", type = " << iter->second.type_str
                 << endl;
        }
    }

} // namespace proto
} // namespace soce
