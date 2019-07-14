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

#include <vector>
#include <unordered_set>
#include "fads-item.h"
#include "token-parser.h"
#include "proto/binary-proto.h"
#include "utils/hash.h"
#include "utils/cond-expr.h"
#include "node.h"

using std::string;
using std::vector;
using soce::utils::Hash;
using soce::utils::CondExpr;
using soce::proto::BinaryProto;

namespace soce{
namespace fadsdb{

    int FadsItem::init(std::unordered_map<std::string, std::shared_ptr<NodeIf>>&& indexes)
    {
        indexes_ = std::move(indexes);
        root_ = indexes_[string()];
        return 0;
    }

    int FadsItem::deserialize(const char* data, size_t len)
    {
        soce::proto::BinaryProto bp;
        bp.init(const_cast<char*>(data), len, false);
        uint64_t size = root_->deserialize(&bp, false);
        if (size == 0){
            return -1;
        }

        return 0;
    }

    Status FadsItem::update(const string& cmd, const string& filters)
    {
        if (!match(filters)){
            return kFilteredOut;
        }

        vector<OperationFields> cmds;
        if (get_update_cmds(cmd, cmds)){
            return kErrInvalidCmd;
        }

        NodeStruct node;
        BinaryProto bp;
        if (backup(node, bp, cmds)){
            return kErrInternal;
        }

        if (do_cmds(cmds)){
            restore(node, bp);
            return kErrDoCmd;
        }

        return kOk;
    }

    Status FadsItem::select(const string& fields,
                            const string& filters,
                            string& out)
    {
        return select(fields, filters, true, out);
    }

    Status FadsItem::selup(const string& fields,
                           const string& uncond_cmds,
                           const string& precmds,
                           const string& subcmds,
                           const string& filters,
                           string& out)
    {
        vector<OperationFields> vec_uncond_cmds;
        vector<OperationFields> vec_precmds;
        vector<OperationFields> vec_subcmds;

        if (get_update_cmds(uncond_cmds, vec_uncond_cmds)
            || get_update_cmds(precmds, vec_precmds)
            || get_update_cmds(subcmds, vec_subcmds)){
            return kErrInvalidCmd;
        }

        NodeStruct node;
        BinaryProto bp;
        if (backup(node, bp, vec_uncond_cmds, vec_precmds, vec_subcmds)){
            return kErrInternal;
        }

        bool need_restore = false;
        Status rc = kOk;
        do{
            if (do_cmds(vec_uncond_cmds)){
                rc = kErrDoCmd;
                need_restore = true;
                break;
            }

            if (!match(filters)){
                rc = kFilteredOut;
                break;
            }

            string out;
            if (do_cmds(vec_precmds)
                || select(fields, filters, false, out)
                || do_cmds(vec_subcmds)){
                rc = kErrDoCmd;
                need_restore = true;
                break;
            }
        } while(0);

        if (need_restore){
            restore(node, bp);
        }

        return rc;
    }

    void FadsItem::reset()
    {
        if (root_){
            root_->reset();
        }
    }

    void FadsItem::show()
    {
        for (auto& i : indexes_){
            i.second->show();
        }
    }

    Status FadsItem::select(const string& fields,
                            const string& filters,
                            bool check_match,
                            string& out)
    {
        if (check_match && !match(filters)){
            return kFilteredOut;
        }
        std::vector<OperationFields> select_fields;
        if (get_select_fields(fields, select_fields) || select_fields.empty()){
            return kErrInvalidField;
        }

        if (select_fields.size() == 1 && select_fields.front().index == "*"){
            BinaryProto bp;
            if (root_->serialize(&bp, false) == 0){
                return kErrInternal;
            }
            out.assign(bp.data(), bp.size());
        }
        else{
            NodeStruct node;
            for (auto& i : select_fields){
                const string& func = i.func;

                auto iter = indexes_.find(i.index);
                if (iter == indexes_.end()){
                    return kErrInvalidField;
                }

                if (func.empty()){
                    node.add_elem(iter->second);
                }
                else{
                    std::shared_ptr<NodeIf> n = iter->second->do_select(func, i.params);
                    if (!n){
                        return kErrInvalidField;
                    }
                    node.add_elem(n);
                }
            }

            BinaryProto bp;
            if (node.serialize(&bp, false) == 0){
                return kErrInternal;
            }
            out.assign(bp.data(), bp.size());
        }

        return kOk;
    }

    bool FadsItem::match(const string& filters)
    {
        if (filters.empty()){
            return true;
        }

        CondExpr cond_expr;
        if (cond_expr.parse(filters.c_str(), filters.size())){
            return false;
        }

        return cond_expr.calc(std::bind([this](const string& lvalue,
                                               const string& oper,
                                               const string& rvalue){
                                            auto iter = indexes_.find(lvalue);
                                            if (iter == indexes_.end()
                                                || !iter->second->has_set()){
                                                return false;
                                            }

                                            return iter->second->bool_func(oper, rvalue);
                                        },
                                        std::placeholders::_1,
                                        std::placeholders::_2,
                                        std::placeholders::_3
                                  ));
    }

    int FadsItem::get_select_fields(const string& fields,
                                    std::vector<OperationFields>& out)
    {
        TokenParser tp(fields.c_str(), fields.size());

        while (!tp.end()){
            string func;
            string token;
            string param;
            vector<string> params;

            if (tp.get_token("(,", token)){
                return -1;
            }

            if (tp.skip_sep('(') == 0){
                func = std::move(token);
                if (tp.get_token(",)", token)){
                    return -1;
                }

                bool finish = false;
                while (!tp.end()){
                    if (tp.skip_sep(')') == 0){
                        finish = true;
                        break;
                    }

                    if (tp.skip_sep(',')){
                        return -1;
                    }

                    if (tp.skip_sep('\'') == 0){
                        if (tp.get_str(param)){
                            return -1;
                        }
                    }
                    else{
                        if (tp.get_token(",)", param)){
                            return -1;
                        }
                    }

                    params.emplace_back(std::move(param));
                }
                if (!finish){
                    return -1;
                }
            }

            out.emplace_back(std::move(func),
                             std::move(token),
                             std::move(params));

            if (tp.skip_sep(',')){
                break;
            }
        }

        return 0;
    }

    int FadsItem::get_update_cmds(const string& cmds,
                                  std::vector<OperationFields>& out)
    {
        TokenParser tp(cmds.c_str(), cmds.size());
        bool finish = true;
        out.clear();

        while (!tp.end()){
            string func;
            string token;
            string param;
            vector<string> params;

            if (tp.get_token("(", func)
                || tp.skip_sep('(')
                || tp.get_token(",)", token)){
                return -1;
            }

            while (!tp.end()){
                if (tp.skip_sep(')') == 0){
                    finish = true;
                    break;
                }
                else if (tp.skip_sep(',') == 0){
                    finish = false;
                    if (tp.skip_sep('\'') == 0){
                        if (tp.get_str(param)){
                            return -1;
                        }
                    }
                    else{
                        if (tp.get_token(",)", param)){
                            return -1;
                        }
                    }
                    params.emplace_back(std::move(param));
                }
                else{
                    return -1;
                }
            }

            if (!finish){
                return -1;
            }

            out.emplace_back(std::move(func),
                             std::move(token),
                             std::move(params));

            if (tp.skip_sep(',')){
                break;
            }
        }

        return 0;
    }

    int FadsItem::backup(NodeStruct& node,
                         soce::proto::BinaryProto& bp,
                         const std::vector<OperationFields>& cmds)
    {
        for (auto& i : cmds){
            auto iter = indexes_.find(i.index);
            if (iter == indexes_.end()){
                return -1;
            }

            node.add_elem(iter->second);
        }

        return (node.serialize(&bp, false) == 0) ? -1 : 0;
    }

    int FadsItem::backup(NodeStruct& node,
                         soce::proto::BinaryProto& bp,
                         const std::vector<OperationFields>& uncond_cmds,
                         const std::vector<OperationFields>& precmds,
                         const std::vector<OperationFields>& subcmds)
    {
        std::unordered_set<std::string> indexes;
        for (auto& i : uncond_cmds){
            indexes.insert(i.index);
        }

        for (auto& i : precmds){
            indexes.insert(i.index);
        }

        for (auto& i : subcmds){
            indexes.insert(i.index);
        }

        for (auto& i : indexes){
            auto iter = indexes_.find(i);
            if (iter == indexes_.end()){
                return -1;
            }

            node.add_elem(iter->second);
        }

        return (node.serialize(&bp, false) == 0) ? -1 : 0;
    }

    int FadsItem::restore(NodeStruct& node, soce::proto::BinaryProto& bp)
    {
        BinaryProto proto;
        proto.init(bp.data(), bp.size(), false);

        return (node.deserialize(&proto, false) == 0) ? -1 : 0;
    }

    int FadsItem::do_cmds(const std::vector<OperationFields>& cmds)
    {
        for (auto& i : cmds){
            auto iter = indexes_.find(i.index);
            if (iter == indexes_.end()){
                return -1;
            }

            if (iter->second->do_update(i.func, i.params)){
                return -1;
            }
        }

        return 0;
    }

} // namespace fadsdb
} // namespace soce
