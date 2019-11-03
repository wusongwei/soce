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

#ifndef _SOCE_FADS_ITEM_J9wesqYHny_H_
#define _SOCE_FADS_ITEM_J9wesqYHny_H_

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include "dbcmd.h"
#include "node-struct.h"
#include "proto/binary-proto.h"

namespace soce{
namespace fadsdb{

    class FadsItem
    {
    public:
        int init(std::unordered_map<std::string, std::shared_ptr<NodeIf>>&& indexes);
        int deserialize(const char* data, size_t len);
        FadsDbRespStatus update(const std::string& cmd, const std::string& filters);
        FadsDbRespStatus select(const std::string& fields,
                                const std::string& filters,
                                std::string& out);
        FadsDbRespStatus selup(const std::string& fields,
                               const std::string& uncond_cmds,
                               const std::string& precmds,
                               const std::string& subcmds,
                               const std::string& filters,
                               std::string& out);
        void reset();
        void show();

    private:
        using OperationFields = struct OperationFields{
        OperationFields(std::string&& _func,
                        std::string&& _index,
                        std::vector<std::string>&& _params)
        : func(std::move(_func)),
                index(std::move(_index)),
                params(std::move(_params))
                {}

            std::string func;
            std::string index;
            std::vector<std::string> params;
        };

    private:
        FadsDbRespStatus select(const std::string& fields,
                                const std::string& filters,
                                bool check_match,
                                std::string& out);
        bool match(const std::string& filters);
        int get_select_fields(const std::string& fields,
                              std::vector<OperationFields>& out);
        int get_update_cmds(const std::string& cmds,
                            std::vector<OperationFields>& out);
        int backup(NodeStruct& node,
                   soce::proto::BinaryProto& bp,
                   const std::vector<OperationFields>& cmds);
        int backup(NodeStruct& node,
                   soce::proto::BinaryProto& bp,
                   const std::vector<OperationFields>& uncond_cmds,
                   const std::vector<OperationFields>& precmds,
                   const std::vector<OperationFields>& subcmds);
        int restore(NodeStruct& node, soce::proto::BinaryProto& bp);
        int do_cmds(const std::vector<OperationFields>& cmds);

    private:
        std::unordered_map<std::string, std::shared_ptr<NodeIf>> indexes_;
        std::shared_ptr<NodeIf> root_;
    };

} // namespace fadsdb
} // namespace soce

#endif
