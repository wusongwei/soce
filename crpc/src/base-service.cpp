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

#include <stack>
#include "crpc/base-service.h"
#include "crpc/log.h"
#include "token-getter.h"
#include "gen-cpp/crpc-header.h"
#include "proto/binary-proto.h"
#include "proto/dynamic-getter.h"
#include "utils/cond-expr.h"

using std::string;
using std::stack;

namespace soce{
namespace crpc{

    string BaseService::process(const string& data)
    {
        string rc;
        string method;
        soce::proto::DynamicGetter dg;
        dg.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);

        // "0.2" for header.method
        if (dg.get_string(method_index_, method)){
            SOCE_WARN << _S("ProtocolError", "Failed to get method");
            /*
             * Protocol error, nothing to do
             */
            return std::string();
        }

        auto iter = handlers_.find(method);
        if (iter == handlers_.end()){
            int64_t req_id = 0;
            if (dg.get_int64(reqid_index_, req_id)){
                CrpcErrorResp resp;
                resp.mutable_header().set_req_id(req_id);
                resp.mutable_header().set_status(kRpcStatusProtoError);

                soce::proto::BinaryProto bp;
                if (resp.serialize((soce::proto::ProtoIf*)&bp) != 0){
                    rc.assign(bp.data(), bp.size());
                }
            }
            return std::move(rc);
        }

        rc = iter->second(data);
        return std::move(rc);
    }

    int BaseService::add_filter(soce::proto::TypeTree* type_tree,
                                const string& service,
                                const string& method,
                                const string& filter)
    {
        CRPC_DEBUG << _S("Service", service) << _S("Method", method) << _S("Filter", filter);

        string name = service + method + "Req";
        soce::crpc::attr::MethodFilters method_filter;

        TokenGetter tg;
        vector<string> expr;
        tg.set_data(filter.c_str(), filter.size());

        /*
         * 0 : epoch
         * 1 : expr beg
         * 2 : expr end
         */
        int status = 0;
        stack<int> status_frame;
        while (!tg.end()){
            string token = tg.get_token();
            if (token == "("){
                if (status != 0 && status != 1 ){
                    return -1;
                }
                expr.push_back(token);
                status_frame.push(status);
                status = 1;
            }
            else if (token == ")"){
                if (status != 2){
                    return -1;
                }

                expr.push_back(token);
            }
            else if(token == "&&" || token == "||"){
                if (status != 2){
                    return -1;
                }

                expr.push_back(token);
                status = 1;
            }
            else{
                if (status != 0 && status != 1){
                    return -1;
                }

                string path;
                int val_type;
                if (type_tree->transfer(name, token, path, val_type)){
                    return -1;
                }
                expr.push_back(path);
                method_filter.mutable_val_type()[path] = val_type;

                token = tg.get_token();
                if (token.empty()){
                    return -1;
                }
                expr.push_back(token);

                token = tg.get_token();
                if (token.empty()){
                    return -1;
                }
                expr.push_back(token);

                status = 2;
            }

            ostringstream oss;
            for (auto& i : expr){
                oss << i << " ";
            }
            method_filter.set_filter(std::move(oss.str()));
            attrs_.mutable_methods()[method] = method_filter;

            CRPC_DEBUG << _S("Filter", oss.str());
        }

        return 0;
    }

    string BaseService::get_service_info()
    {
        string rc;
        soce::proto::BinaryProto bp;
        size_t len = attrs_.serialize((soce::proto::ProtoIf*)&bp);
        if (len != 0){
            rc.assign(bp.data(), bp.size());
        }

        CRPC_DEBUG << _S("ServiceInfo", rc);

        return std::move(rc);
    }

} // namespace crpc
} // namespace soce
