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

#include "dbclient.h"

namespace soce{
namespace fadsdb{

    FadsDbSyncClient::FadsDbSyncClient(const std::string& host, int port)
    {
        client_.init(host, port);
    }

    int FadsDbSyncClient::create(const std::string& schema, FadsDbResponse& response)
    {
        FadsDbCreate crt;
        crt.mutable_header().set_type(kCmdCreate);
        crt.mutable_header().set_id(++req_id_);
        crt.set_schema(schema);

        soce::proto::BinaryProto bp;
        crt.serialize(&bp);

        return dorpc(std::string(bp.data(), bp.size()), response);
    }

    int FadsDbSyncClient::insert(const std::string& table,
                                 const std::string& key,
                                 const std::string& data,
                                 FadsDbResponse& response)
    {
        FadsDbInsert ist;
        ist.mutable_header().set_type(kCmdInsert);
        ist.mutable_header().set_id(++req_id_);
        ist.set_table(table);
        ist.set_key(key);
        ist.set_data(data);

        soce::proto::BinaryProto bp;
        ist.serialize(&bp);

        return dorpc(std::string(bp.data(), bp.size()), response);
    }

    int FadsDbSyncClient::update(const std::string& table,
                                 const std::string& key,
                                 const std::string& action,
                                 const std::string& filter,
                                 FadsDbResponse& response)
    {
        FadsDbUpdate up;
        up.mutable_header().set_type(kCmdUpdate);
        up.mutable_header().set_id(++req_id_);
        up.set_table(table);
        up.set_key(key);
        up.set_cmd(action);
        up.set_filters(filter);

        soce::proto::BinaryProto bp;
        up.serialize(&bp);

        return dorpc(std::string(bp.data(), bp.size()), response);
    }

    int FadsDbSyncClient::select(const std::string& table,
                                 const std::string& key,
                                 const std::string& field,
                                 const std::string& filter,
                                 FadsDbResponse& response)
    {
        FadsDbSelect sel;
        sel.mutable_header().set_type(kCmdSelect);
        sel.mutable_header().set_id(++req_id_);
        sel.set_table(table);
        sel.set_key(key);
        sel.set_fields(field);
        sel.set_filters(filter);

        soce::proto::BinaryProto bp;
        sel.serialize(&bp);

        return dorpc(std::string(bp.data(), bp.size()), response);
    }


    int FadsDbSyncClient::dorpc(const std::string& req, FadsDbResponse& response)
    {
        soce::proto::BinaryProto bp;
        string resp;
        soce::crpc::RpcStatus status = client_.exec(resp, req_id_, req);
        if (status != soce::crpc::RpcStatus::kRpcStatusOK) {
            return -1;
        }

        bp.init(const_cast<char*>(resp.data()), resp.size());
        return (response.deserialize(&bp) == 0) ? -1 : 0;
    }
} //namespace fadsdb
} // namespace soce
