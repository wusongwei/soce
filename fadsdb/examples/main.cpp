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

#include <stdio.h>
#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <sys/time.h>
#include <unistd.h>
#include "fads-db.h"
#include "node.h"
#include "proto/binary-proto.h"
#include "utils/time-helper.h"
#include "plugin.h"
#include "node-primary.h"
#include "gen-cpp/test.h"
#include "dbcmd.h"
#include "dbcore.h"
#include "proto/dynamic-getter.h"
#include "proto/dynamic-parser.h"
#include "log4rel/console-sink.hpp"
#include "dbserver.h"
#include "dbclient.h"

using namespace std;
using namespace fads;
using namespace soce::fadsdb;
using namespace soce::proto;

/**
 *
 * 本示例主要展示：
 * （1）创建、添加、更新、查询数据库。
 * （2）为特定类型添加命令。
 */

int ModI32(const std::vector<std::string>& params, NodeIf* node)
{
    NodeInt32* p = (NodeInt32*)node;
    if (!p->has_set()){
        SOCE_INFO << _D("dummy node");
        return 0;
    }

    SOCE_INFO << _S("func", "ModI32")
              << _S("param", params.front())
              << _S("value", p->val_);
    if (params.size() != 1){
        return -1;
    }

    p->val_ %= atoi(params.front().c_str());

    return 0;
}

class ClientTest
{
public:
    ClientTest()
        {
            client_ = std::make_shared<FadsDbSyncClient>("127.0.0.1", 1234);
            thread_ = std::thread(&ClientTest::test, this);
        }

    ~ClientTest(){
        thread_.join();
    }

private:
    void test()
        {
            sleep(1);

            create_test();
            insert_test();
            update_test();
            select_test();
        }

    void create_test()
        {
            FadsDbResponse response;
            int rc = client_->create("struct Worker{string name; i32 age;} table company{string CEO; Worker worker;}", response);
            if (rc == 0) {
                SOCE_INFO << _S("create", response.get_status());
            }else{
                SOCE_ERROR << _S("create", rc);
            }
        }

    void insert_test()
        {
            Worker worker;
            worker.mutable_name() = "louis";
            worker.mutable_age() = 18;

            company scompany;
            scompany.set_CEO("jobs");
            scompany.mutable_worker() = worker;

            soce::proto::BinaryProto bp;
            scompany.serialize(&bp);
            FadsDbResponse response;
            int rc = client_->insert(table_, key_, std::string(bp.data(), bp.size()), response);

            if (rc == 0) {
                SOCE_INFO << _S("insert", response.get_status());
            }else{
                SOCE_ERROR << _S("insert", rc);
            }
        }

    void update_test()
        {
            std::string action = "mod(worker.age, 4)";
            std::string filter = "CEO == jobs";
            FadsDbResponse response;
            int rc = client_->update(table_, key_, action, filter, response);
            if (rc == 0) {
                SOCE_INFO << _S("update", response.get_status());
            }else{
                SOCE_ERROR << _S("update", rc);
            }
        }

    void select_test()
        {
            std::string field = "*";
            std::string filter = "CEO == jobs";
            FadsDbResponse response;
            int rc = client_->select(table_, key_, field, filter, response);
            if (rc == 0) {
                parse_with_dynamic_parser(response.get_response());
                parse_with_dynamic_node(response.get_response());
                parse_with_deserialize(response.get_response());
            }else{
                SOCE_ERROR << _S("select", rc);
            }

        }

    void parse_with_dynamic_parser(const std::string& response)
        {
            soce::proto::DynamicParser dp;
            auto node = dp.parse(response.c_str(), response.size());
            SOCE_INFO << _S("select_result", node->to_json());
        }

    void parse_with_dynamic_node(const std::string& response)
        {
            auto root = std::make_shared<DynamicNodeStruct>("Company");
            root->add(std::make_shared<DynamicNodeString>("CEO", ""));
            auto worker = std::make_shared<DynamicNodeStruct>("Worker");
            worker->add(std::make_shared<DynamicNodeString>("name", ""));
            worker->add(std::make_shared<DynamicNodeInt32>("age", 0));
            root->add(worker);
            DynamicParser dp;
            dp.parse(response.c_str(), response.size(), root);
            SOCE_INFO << _S("select_result", root->to_json());
        }
    void parse_with_deserialize(const std::string& response)
        {
            fads::company c;
            BinaryProto bp;
            bp.init(const_cast<char*>(response.c_str()), response.size());
            c.deserialize(&bp);
            SOCE_INFO << _S("cmd", "select")
                      << _S("CEO", c.get_CEO())
                      << _S("Worker.name", c.get_worker().get_name())
                      << _S("Worker.age", c.get_worker().get_age());

        }

private:
    std::shared_ptr<FadsDbSyncClient> client_;
    std::thread thread_;
    std::string table_ = "company";
    std::string key_ = "apple";
};

int main()
{
    SOCE_CUR_LOGGER->set_log_level(soce::log4rel::kLogLevelInfo);
    SOCE_CUR_LOGGER->reserve_field(soce::log4rel::kLogFieldPos, true);

    // 为kTypeInt32类型添加mod命令。
    SPlugin.register_update_func(soce::proto::SoceDataType::kTypeInt32,
                                 "mod",
                                 ModI32);

    ClientTest ct;

    FadsDbServer server;
    server.start("127.0.0.1:1234", 8);

    return 0;
}
