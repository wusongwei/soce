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

#ifndef _SOCE_PROTOTYPE_XjUyejQlr6_H_
#define _SOCE_PROTOTYPE_XjUyejQlr6_H_

#include <string>
#include <vector>
#include <memory>
#include <stdint.h>
#include "node-if.h"

namespace soce{
namespace fadsdb{

    class Prototype
    {
    public:
        Prototype& operator=(const Prototype& rhs);
        int add_elem(const std::string& type, const std::string& name);
        int add_elem(const Prototype& proto, const std::string& name);
        void set_index(const std::string& index, const std::string& name);
        inline const std::string& get_index() {return index_;}
        inline void set_node(std::shared_ptr<NodeIf> node) {node_ = node;}
        inline void add_child(std::shared_ptr<Prototype> proto) {children_.push_back(proto);}
        std::shared_ptr<NodeIf> clone(std::unordered_map<std::string, std::shared_ptr<NodeIf>>& indexes) const;

    private:
        int add_node(const std::string& type, const std::string& index, const std::string& name);
        std::shared_ptr<NodeIf> create_node(const std::string& type);
        std::shared_ptr<Prototype> copy(const std::string& index) const;
        std::string format_index(const std::string& path, const std::string& name);

    private:
        std::string index_;
        std::shared_ptr<NodeIf> node_;
        std::vector<std::shared_ptr<Prototype>> children_;
    };

} // namespace fadsdb
} // namespace soce

#endif
