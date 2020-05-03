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

#include <unistd.h>
#include <thread>
#include "utils/actor-executor.h"
#include "transport/transport-libevent.h"
#include "cort-engine/cort-engine.h"
#include "log4rel/logger.h"

using namespace soce::transport;
using namespace soce::utils;
using namespace std;

class CounterActor : public Actor
{
public:
    void add(int adder, int& counted)
        {
            counter_ += adder;
            counted = counter_;
        }

private:
    int counter_ = 0;
};

void timer_entry(ActorExecutor& ae,
                 std::shared_ptr<CounterActor> actor)
{
    int counted = 0;
    auto id = SCortEngine.create([&]{
            ae.submit(actor,
                      [&counted](Actor::ActorPtr actor){
                          // sleep(1);
                          CounterActor* ca = (CounterActor*)actor.get();
                          ca->add(1, counted);
                          SOCE_DEBUG << _S("counted", counted);
                      });
        });
    SCortEngine.resume(id);
}

void thread_entry(std::shared_ptr<CounterActor> actor)
{
    TransportLibevent tle;
    tle.init();

    ActorExecutor ae(1, 3, 2, -1, &tle);
    tle.add_timer(1000,
                  TransportIf::kTimerModeOnceAndDel,
                  std::bind(timer_entry, ae, actor));

    tle.run();
}

#define N 4

void actor_executor()
{
    SOCE_DEBUG << _D("--------------- actor_executor ---------------");

    std::shared_ptr<CounterActor> actor = std::make_shared<CounterActor>();

    std::thread t[N];
    for (int i=0; i<N; ++i){
        t[i] = std::thread(thread_entry, actor);
    }

    for (int i=0; i<N; ++i){
        if (t[i].joinable()){
            t[i].join();
        }
    }
}
