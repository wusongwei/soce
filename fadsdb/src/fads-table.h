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

#ifndef _SOCE_FADS_TABLE_pBDEy8oMan_H_
#define _SOCE_FADS_TABLE_pBDEy8oMan_H_

#include "fads-item.h"
#include "prototype.h"
#include "dbcmd.h"

namespace soce{
namespace fadsdb{

    class FadsTable
    {
    public:
        inline void set_name(const std::string& name)
        {
            name_ = name;
        }

        inline const std::string& get_name() const
        {
            return name_;
        }

        int add_elem(const std::string& type, const std::string& name);
        int add_elem(const Prototype& proto, const std::string& name);

        FadsDbRespStatus insert(const std::string& key, const char* data, size_t len);
        FadsDbRespStatus remove(const std::string& key);
        FadsDbRespStatus update(const std::string& key,
                                const std::string& cmd,
                                const std::string& filters);
        FadsDbRespStatus select(const std::string& key,
                                const std::string& fields,
                                const std::string& filters,
                                std::string& out);
        FadsDbRespStatus selup(const std::string& key,
                               const std::string& fields,
                               const std::string& uncond_cmds,
                               const std::string& precmds,
                               const std::string& subcmds,
                               const std::string& filters,
                               std::string& out);

        void add_schema(const std::string& schema, int indent);
        inline const std::string& get_schema() const
        {
            return schema_;
        }

        std::shared_ptr<FadsTable> copy() const;

    private:
        std::shared_ptr<FadsItem> create_item();

    private:
        std::string schema_;
        std::string name_;
        std::unordered_map<std::string, std::shared_ptr<FadsItem>> items_;
        Prototype prototype_;
    };

} // namespace fadsdb
} // namespace soce
#endif
