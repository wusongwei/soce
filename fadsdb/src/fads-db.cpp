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

#include "fads-db.h"
#include "fads-table.h"
#include "log4rel/logger.h"

namespace soce{
namespace fadsdb{

    void FadsDb::add_tables(const std::vector<std::shared_ptr<FadsTable>>& tables)
    {
        for (auto& i : tables){
            tables_[i->get_name()] = i->copy();
        }
    }

    Status FadsDb::insert(const std::string& table,
                          const std::string& key,
                          const char* data,
                          size_t len)
    {
        auto iter = tables_.find(table);
        if (iter == tables_.end()){
            return kErrTableNotExist;
        }

        return iter->second->insert(key, data, len);
    }

    Status FadsDb::remove(const std::string& table, const std::string& key)
    {
        auto iter = tables_.find(table);
        if (iter == tables_.end()){
            return kErrTableNotExist;
        }

        return iter->second->remove(key);
    }

    Status FadsDb::update(const std::string& table,
                          const std::string& key,
                          const std::string& cmd,
                          const std::string& filters)
    {
        auto iter = tables_.find(table);
        if (iter == tables_.end()){
            return kErrTableNotExist;
        }

        return iter->second->update(key, cmd, filters);
    }

    Status FadsDb::select(const std::string& table,
                          const std::string& key,
                          const std::string& fields,
                          const std::string& filters,
                          std::string& out)
    {
        auto iter = tables_.find(table);
        if (iter == tables_.end()){
            return kErrTableNotExist;
        }

        return iter->second->select(key, fields, filters, out);
    }

    Status FadsDb::selup(const std::string& table,
                         const std::string& key,
                         const std::string& fields,
                         const std::string& uncond_cmds,
                         const std::string& precmds,
                         const std::string& subcmds,
                         const std::string& filters,
                         std::string& out)
    {
        auto iter = tables_.find(table);
        if (iter == tables_.end()){
            return kErrTableNotExist;
        }

        return iter->second->selup(key, fields, uncond_cmds, precmds, subcmds, filters, out);
    }

} // namespace fadsdb
} // namespace soce
