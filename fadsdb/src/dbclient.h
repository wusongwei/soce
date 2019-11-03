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

#ifndef _SOCE_FADSDB_DBCLIENT_e4c08GmM1M_H_
#define _SOCE_FADSDB_DBCLIENT_e4c08GmM1M_H_

#include "dbrpc.h"
#include "dbrpc-service.h"
#include "dbcmd.h"

namespace soce{
namespace fadsdb{

    class FadsDbSyncClient
    {
    public:
        FadsDbSyncClient(const std::string& ip, int port);
        int create(const std::string& schema,
                   FadsDbResponse& response);
        int insert(const std::string& table,
                   const std::string& key,
                   const std::string& data,
                   FadsDbResponse& response);
        int update(const std::string& table,
                   const std::string& key,
                   const std::string& action,
                   const std::string& filter,
                   FadsDbResponse& response);
        int select(const std::string& table,
                   const std::string& key,
                   const std::string& field,
                   const std::string& filter,
                   FadsDbResponse& response);

    private:
        int dorpc(const std::string& req, FadsDbResponse& response);

    private:
        FadsDbServiceSyncClient client_;
        int64_t req_id_ = 0;
    };

} //namespace fadsdb
} // namespace soce

#endif
