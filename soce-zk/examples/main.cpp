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

#include <iostream>
#include <unistd.h>
#include <thread>
#include <atomic>
#include "soce-zk/soce-zk.h"
#include "log4rel/logger.h"

using namespace std;
using namespace soce::zookeeper;
using namespace soce::log4rel;

const std::string g_exist_path = "/test/exist";
const std::string g_get_path = "/test/get";
const std::string g_get_child_path = "/test/get_child";
const std::string g_get_child_context_path = "/test/get_child_context";

const std::string g_value = "test-value";

static std::atomic<bool> g_init{false};

static void zk_event(SoceZk* soce_zk, bool* run, std::shared_ptr<Logger> logger)
{
    SOCE_LOGGER_MGR.add_logger("soce_zk_watcher", logger);
    SOCE_LOGGER_MGR.switch_logger("soce_zk_watcher");

    bool exist = false;
    if (soce_zk->exist(g_exist_path, kWatchModeRepeat, exist) < 0){
        SOCE_ERROR << _D("init exist wathcer");
    }

    string value;
    if (soce_zk->get(g_get_path, kWatchModeRepeat, value) < 0){
        SOCE_ERROR << _D("init get wathcer");
    }

    vector<string> children;
    if (soce_zk->get_children(g_get_child_path, kWatchModeRepeat, children) < 0){
        SOCE_ERROR << _D("init get child wathcer");
    }

    vector<pair<string, string>> children_context;
    if (soce_zk->get_children_content(g_get_child_context_path, kWatchModeRepeat, children_context) < 0){
        SOCE_ERROR << _D("init get child context wathcer");
    }

    vector<NotifyInfo> notifies;
    fd_set readfds;
    int fd = soce_zk->get_notify_evtfd();

    g_init.store(true);

    while (*run){
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int rc = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (rc > 0){
            uint64_t ct = 0;
            (void)read(fd, (void*)&ct, sizeof(uint64_t));

            soce_zk->get_notifies(notifies);
            for (auto& info : notifies){
                SOCE_INFO << _S("type", info.type)
                                    << _S("path", info.path);

                switch (info.type){
                case kTypeExist:
                    SOCE_INFO << _S("exist", (info.exist ? "true" : "false"));
                    break;

                case kTypeGet:
                    SOCE_INFO << _S("value", info.value);
                    break;

                case kTypeGetCld:
                    for (auto& i : info.cld){
                        SOCE_INFO << _S("child", i);
                    }
                    break;

                case kTypeGetCldCnt:
                    for (auto& i : info.cldcnt){
                        SOCE_INFO << _S("child", i.first)
                                            << _S("content", i.second);
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }
}



int main()
{
//    SOCE_CUR_LOGGER->reserve_field(soce::log4rel::kLogFieldPos, true);
    SOCE_CUR_LOGGER->set_log_level(soce::log4rel::kLogLevelInfo);
    SoceZk soce_zk;
    uint32_t timeout = 4000;
    if (soce_zk.init("127.0.0.1:2181", timeout)){
        cout << "Init error" << endl;
        return -1;
    }

    bool run = true;
    std::thread t = std::thread(zk_event, &soce_zk, &run, SOCE_CUR_LOGGER);

    while (!g_init);

    SOCE_INFO << _S("create path", g_exist_path);
    soce_zk.create(g_exist_path, g_value, kCrtModeEphemeral);
    usleep(10000);

    SOCE_INFO << _S("create path", g_get_path);
    soce_zk.create(g_get_path, g_value, kCrtModeEphemeral);
    usleep(10000);

    SOCE_INFO << _S("change path", g_get_path);
    soce_zk.set(g_get_path, g_value + "-change");
    usleep(10000);

    SOCE_INFO << _S("add child", g_get_child_path);
    soce_zk.create(g_get_child_path + "/a", g_value, kCrtModeEphemeral);
    soce_zk.create(g_get_child_path + "/b", g_value, kCrtModeEphemeral);
    usleep(10000);

    SOCE_INFO << _S("add child context", g_get_child_context_path);
    soce_zk.create(g_get_child_context_path + "/a", g_value, kCrtModeEphemeral);
    soce_zk.create(g_get_child_context_path + "/b", g_value, kCrtModeEphemeral);
    usleep(10000);

    SOCE_INFO << _S("del child", g_get_child_path);
    soce_zk.del(g_get_child_path + "/b");
    usleep(10000);

    SOCE_INFO << _S("del child context", g_get_child_context_path);
    soce_zk.del(g_get_child_context_path + "/b");

    run = false;
    sleep(1);
    t.join();

    return 0;
}
