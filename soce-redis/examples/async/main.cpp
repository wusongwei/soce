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

/*
 * Async access.
 * Each request will be executed in a coroutine.
 */

#include <iostream>
#include "soce-redis-async.h"
using namespace std;

int main()
{
    SoceRedisAsync async_client;
    async_client.init("none@127.0.0.1:6379");

    for (int i=0; i<10; ++i){
        async_client.exec_async([](Param& pa){
                pa << "get" << "a";
            },
            [=](std::shared_ptr<SoceRedis> soce_redis){
                string val;
                soce_redis->begin()->get_str(val);
                cout << "val = " << val << endl;
            });

        cout << "add request, i = " << i << endl;
    }

    return 0;
}
