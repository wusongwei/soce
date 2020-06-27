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

#include <unordered_map>
#include "prototype.h"
#include "node.h"
#include "log4rel/logger.h"

using std::string;
using std::vector;
using std::unordered_map;

namespace soce{
namespace fadsdb{

    Prototype& Prototype::operator=(const Prototype& rhs)
    {
        if (this == &rhs){
            return *this;
        }

        index_ = rhs.index_;
        if (rhs.node_){
            node_ = rhs.node_->clone();
        }

        for (auto& iter : rhs.children_){
            std::shared_ptr<Prototype> p(new Prototype);
            *p = *iter;
            children_.push_back(p);
        }

        return *this;
    }

    int Prototype::add_elem(const std::string& type, const std::string& name)
    {
        std::shared_ptr<Prototype> ptype(new Prototype);
        if (ptype->add_node(type, index_, name)){
            return -1;
        }
        children_.push_back(ptype);
        return 0;
    }

    int Prototype::add_elem(const Prototype& proto, const std::string& name)
    {
        string index = format_index(index_, name);
        std::shared_ptr<Prototype> ptype = proto.copy(index);
        children_.push_back(ptype);
        return 0;
    }

    void Prototype::set_index(const std::string& path, const std::string& name)
    {
        index_ = format_index(path, name);
    }

    int Prototype::add_node(const std::string& type, const std::string& index, const std::string& name)
    {
        set_index(index, name);
        node_ = create_node(type);

        return node_ ? 0 : -1;
    }

    std::shared_ptr<NodeIf> Prototype::create_node(const std::string& type)
    {
        std::shared_ptr<NodeIf> node;
        if (type == "bool"){
            node.reset(new NodeBool);
        }
        if (type == "i32"){
            node.reset(new NodeInt32);
        }
        if (type == "i64"){
            node.reset(new NodeInt64);
        }
        if (type == "float"){
            node.reset(new NodeFloat);
        }
        if (type == "double"){
            node.reset(new NodeDouble);
        }
        else if(type == "string"){
            node.reset(new NodeString);
        }
        else if (type == "any") {
            node.reset(new NodeAny);
        }

        return node;
    }

    std::shared_ptr<Prototype> Prototype::copy(const std::string& index) const
    {
        std::shared_ptr<Prototype> ptype(new Prototype);
        ptype->set_index(index, index_);

        if (node_){
            std::shared_ptr<NodeIf> node = node_->clone();
            ptype->set_node(node);
        }
        else{
            for (auto& i : children_){
                std::shared_ptr<Prototype> proto = i->copy(index);
                ptype->add_child(proto);
            }
        }

        return ptype;
    }

    std::shared_ptr<NodeIf> Prototype::clone(std::unordered_map<std::string, std::shared_ptr<NodeIf>>& indexes) const
    {
        std::shared_ptr<NodeIf> node;
        if (node_){
            node = node_->clone();
        }
        else{
            std::shared_ptr<NodeStruct> ns = std::shared_ptr<NodeStruct>(new NodeStruct);
            for (auto& i : children_){
                std::shared_ptr<NodeIf> child = i->clone(indexes);
                ns->add_elem(child);
            }
            node = ns;
        }

        indexes[index_] = node;
        return node;
    }

    std::string Prototype::format_index(const std::string& path, const std::string& name)
    {
        string index;
        if (!path.empty() && !name.empty()){
            index = path + "." + name;
        }
        else if (!path.empty()){
            index = path;
        }
        else if (!name.empty()){
            index = name;
        }

        return std::move(index);
    }

} // namespace fadsdb
} // namespace soce
