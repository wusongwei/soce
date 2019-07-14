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

#include "fads-table.h"

using std::string;

namespace soce{
namespace fadsdb{

    int FadsTable::add_elem(const string& type, const string& name)
    {
        return prototype_.add_elem(type, name);
    }

    int FadsTable::add_elem(const Prototype& proto, const string& name)
    {
        return prototype_.add_elem(proto, name);
    }

    Status FadsTable::insert(const string& key, const char* data, size_t len)
    {
        Status rc = kOk;
        std::shared_ptr<FadsItem> item;
        auto iter = items_.find(key);
        if (iter != items_.end()){
            item = iter->second;
            item->reset();
            rc = kDupKey;
        }else{
            item = create_item();
        }

        if (item->deserialize(data, len)){
            items_.erase(key);
            return kErrProto;
        }

        items_[key] = item;
        return rc;
    }

    Status FadsTable::remove(const string& key)
    {
        auto iter = items_.find(key);
        if (iter == items_.end()){
            return kErrKeyNotExist;
        }

        items_.erase(iter);
        return kOk;
    }

    Status FadsTable::update(const string& key,
                             const string& cmd,
                             const string& filters)
    {
        auto iter = items_.find(key);
        if (iter == items_.end()){
            return kErrKeyNotExist;
        }

        return iter->second->update(cmd, filters);
    }

    Status FadsTable::select(const string& key,
                             const string& fields,
                             const string& filters,
                             string& out)
    {
        auto iter = items_.find(key);
        if (iter == items_.end()){
            return kErrKeyNotExist;
        }

        return iter->second->select(fields, filters, out);
    }

    Status FadsTable::selup(const string& key,
                            const string& fields,
                            const string& uncond_cmds,
                            const string& precmds,
                            const string& subcmds,
                            const string& filters,
                            string& out)
    {
        auto iter = items_.find(key);
        if (iter == items_.end()){
            return kErrKeyNotExist;
        }

        return iter->second->selup(fields, uncond_cmds, precmds, subcmds, filters, out);
    }

    void FadsTable::add_schema(const string& schema, int indent)
    {
        schema_ += string(indent, ' ') + schema;
    }

    std::shared_ptr<FadsTable> FadsTable::copy() const
    {
        std::shared_ptr<FadsTable> table(new FadsTable);
        table->schema_ = schema_;
        table->name_ = name_;
        table->prototype_ = prototype_;
        return table;
    }

    std::shared_ptr<FadsItem> FadsTable::create_item()
    {
        std::unordered_map<std::string, std::shared_ptr<NodeIf>> indexes;
        prototype_.clone(indexes);

        std::shared_ptr<FadsItem> item(new FadsItem);
        item->init(std::move(indexes));

        return item;
    }

} // namespace fadsdb
} // namespace soce
