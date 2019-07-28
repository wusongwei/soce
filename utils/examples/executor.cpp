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
#include "utils/executor.h"
#include "transport/transport-libevent.h"
#include "cort-engine/cort-engine.h"
#include "log4rel/logger.h"

using namespace soce::transport;
using namespace soce::utils;
using namespace std;

static int g_timer_index = 0;

void timer_entry(Executor& e)
{
    if (g_timer_index > 5) {
        return;
    }

    auto id = SCortEngine.create([&]{
            int timer_index = ++g_timer_index;

            SOCE_DEBUG << _S("before submit, timer_index", timer_index);
            int val = timer_index;
            e.submit([&val]{
                    sleep(3);
                    val += 1;
                });
            SOCE_DEBUG << _S("after submit, timer_index", timer_index) << _S("val", val);
        });
    SCortEngine.resume(id);
}

void executor()
{
    SOCE_DEBUG << _D("--------------- executor ---------------");

    TransportLibevent tle;
    tle.init();

    Executor e(1, 3, 2, -1, &tle);
    tle.add_timer(1000,
                  TransportIf::kTimerModePresist,
                  std::bind(timer_entry, e));

    tle.run();
}
