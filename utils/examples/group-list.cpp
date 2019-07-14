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

#include "log4rel/logger.h"
#include "utils/group-list.hpp"

using std::string;
using namespace soce::utils;

class ExampleGroupList
{
public:
    void start()
        {
            SOCE_DEBUG << _D("--------------- sample_group_list ---------------");

            GroupList<string, int>::gid_t gid = 0;
            int val = 0;
            group_list_.append("a", val, gid);
            group_list_.append("b", val, gid);

            int* pval = nullptr;
            int rc = group_list_.ack("a", &pval);
            *pval = 1;
            SOCE_DEBUG << _S("key", "a") << _S("rc", rc);
            rc = group_list_.ack("b", &pval);
            *pval = 2;
            SOCE_DEBUG << _S("key", "a") << _S("rc", rc);

            GroupList<string, int>::group_list* gl = nullptr;
            group_list_.get(gid, &gl);
            for (auto& i : *gl){
                SOCE_DEBUG << _S("key", i.first) << _S("val", i.second);
            }
        }

private:
    GroupList<string, int> group_list_;
};

void group_list()
{
    ExampleGroupList sgl;
    sgl.start();
}
