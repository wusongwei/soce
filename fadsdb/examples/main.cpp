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
#include "log4rel/console-sink.hpp"

using namespace std;
using namespace fads;
using namespace soce::fadsdb;

/**
 * 目前还没有为fadsdb添加网络功能，
 * 因此程序的执行结果只能通过日志查看。
 * DbCore::thread_entry()用于从工作线程中收集执行结果，
 * 它仅仅是打印当前命令的类型等简单信息。
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

void RespHandler(const soce::fadsdb::DbCore::RespData& resp)
{
    string result = resp.result_ == 0 ? "ok" : "failed";
    switch (resp.req_id_){
    case 0:
        SOCE_INFO << _S("cmd", "create") << _S("result", result);
        break;

    case 1:
        SOCE_INFO << _S("cmd", "insert") << _S("result", result);
        break;

    case 2:
        SOCE_INFO << _S("cmd", "update") << _S("result", result);
        break;

    case 3:
        SOCE_INFO << _S("cmd", "select") << _S("result", result);
        if (resp.result_ == kOk){
            fads::company c;
            soce::proto::BinaryProto bp;
            bp.init(const_cast<char*>(resp.data_.c_str()), resp.data_.size());
            if (c.deserialize(&bp) == 0){
                SOCE_ERROR << _S("deserialize", "error");
            }else{
                SOCE_INFO << _S("cmd", "select")
                          << _S("CEO", c.get_CEO())
                          << _S("Worker.name", c.get_worker().get_name())
                          << _S("Worker.age", c.get_worker().get_age());
            }
        }
        break;
    }
}

int main()
{
    SOCE_CUR_LOGGER->set_log_level(soce::log4rel::kLogLevelInfo);

    int64_t req_id = 0;
    // 为kTypeInt32类型添加mod命令。
    SPlugin.register_update_func(soce::proto::SoceDataType::kTypeInt32,
                                 "mod",
                                 ModI32);
    DbCore dbcore;
    dbcore.init(4);
    dbcore.set_resp_handler(std::bind(RespHandler, std::placeholders::_1));

    // create
    FadsDbCreate crt;
    crt.mutable_header().set_type(kCmdCreate);
    crt.mutable_header().set_id(1);
    crt.set_schema("struct Worker{string name; i32 age;} table company{string CEO; Worker worker;}");
    soce::proto::BinaryProto bp;
    crt.serialize(&bp);
    dbcore.do_sql(req_id++, bp.data(), bp.size());

    // insert
    Worker worker;
    worker.mutable_name() = "louis";
    worker.mutable_age() = 18;

    company scompany;
    scompany.set_CEO("jobs");
    scompany.mutable_worker() = worker;
    bp.reset();
    scompany.serialize(&bp);

    FadsDbInsert ist;
    ist.mutable_header().set_type(kCmdInsert);
    ist.mutable_header().set_id(2);
    ist.set_table("company");
    ist.set_key("apple");
    ist.set_data(std::move(string(bp.data(), bp.size())));

    bp.reset();
    ist.serialize(&bp);
    dbcore.do_sql(req_id++, bp.data(), bp.size());

    // update
    FadsDbUpdate up;
    up.mutable_header().set_type(kCmdUpdate);
    up.mutable_header().set_id(3);
    up.set_table("company");
    up.set_key("apple");
    up.set_cmd("mod(worker.age, 4)");
    up.set_filters("CEO == jobs");
    bp.reset();
    up.serialize(&bp);
    dbcore.do_sql(req_id++, bp.data(), bp.size());

    // select
    FadsDbSelect sel;
    sel.mutable_header().set_type(kCmdSelect);
    sel.mutable_header().set_id(4);
    sel.set_table("company");
    sel.set_key("apple");
    sel.set_fields("*");
    sel.set_filters("CEO == jobs");
    bp.reset();
    sel.serialize(&bp);
    dbcore.do_sql(req_id++, bp.data(), bp.size());

    while (1){
        sleep(100);
    }

    return 0;
}
