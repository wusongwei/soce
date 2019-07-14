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
 * Access the server in coroutine with the same connection.
 * All the coroutines share the same SoceRedis instance.
 */

#include <iostream>
#include <unistd.h>
#include "soce-redis/soce-redis.h"
#include "transport/transport-libevent.h"
#include "transport/bypass-processor.h"
#include "utils/time-helper.h"

using namespace std;
using namespace soce::redis;
using namespace soce::transport;
using namespace soce::cortengine;
using namespace soce::utils;

#define CHECK_RC(rc, msg)                           \
    if (rc) {                                       \
        cout << "error:" << msg << endl;            \
        return -1;                                  \
}

class RedisSample
{
public:
    RedisSample() {}
    ~RedisSample() {}

    void watch(int fd, std::function<void(int)> handler)
        {
            transport_->watch(fd, std::shared_ptr<BypassProcessor>(new BypassProcessor(handler)));
        }

    void unwatch(int fd)
        {
            transport_->unwatch(fd);
        }

    int init(const std::string& conf)
        {
            TransportLibevent* le = new TransportLibevent;
            le->init();
            transport_.reset(le);
            SoceRedis::CortEnv env;
            env.watch = std::bind(&RedisSample::watch,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2);
            env.unwatch = std::bind(&RedisSample::unwatch,
                                  this,
                                  std::placeholders::_1);

            if (clt_.init(conf, env)){
                return -1;
            }

            transport_->add_timer(10,
                                  TransportIf::kTimerModePresist,
                                  std::bind(&SoceRedis::keepalive, &clt_));
            return 0;
        }

    void do_sample()
        {
            sample();
            transport_->run();
        }

private:
        void sample()
        {
            cout << "Begin : " << TimeHelper::get_time_ms() << endl;
            for (size_t i = 0; i<5; ++i){
                CortEngine::CortId id = SCortEngine.create([this]{
                        SoceRedisIterator iter;
                        if (soce_redis_docmd(clt_, "incr" << "a")){
                            cout << "failed" << endl;
                            return;
                        }
                        else{
                            SoceRedisType type;
                            if (clt_.begin()->get_type(type)){
                                cout << "Get Type failed" << endl;
                                return;
                            }
                            if (type == kSRTError){
                                cout << "id = " << SCortEngine.get_cur_cort_id()
                                     << ", type = " << type << endl;
                                return;
                            }
                            else{
                                int result;
                                clt_.begin()->get_int(result);
                                cout << "id = " << SCortEngine.get_cur_cort_id()
                                     << ", result = " << result << endl;
                            }
                        }
                    },
                    5000);
                SCortEngine.resume(id);
            }

            CortEngine::CortId id = SCortEngine.create([this]{
                    SoceRedisIterator iter;
                    if (soce_redis_docmd(clt_, "get" << "a")){
                        cout << "Get error" << endl;
                    }
                    else{
                        string result;
                        clt_.begin()->get_str(result);
                        cout << "cmd get id = " << SCortEngine.get_cur_cort_id()
                             << ", result = " << result << endl;
                        cout << "End   : " << TimeHelper::get_time_ms() << endl;
                    }
                },
                5000);
            SCortEngine.resume(id);
        }

private:
    SoceRedis clt_;
    std::shared_ptr<TransportIf> transport_;
};

int main()
{
    int rc = 0;
    string conf = "none@127.0.0.1:6379";
    // none : no passwd
    //string conf = "none@127.0.0.1:30001,127.0.0.1:30002,127.0.0.1:30006?cluster=true";

    RedisSample sample;
    rc = sample.init(conf);
    CHECK_RC(rc, "Init");
    sample.do_sample();

    return 0;
}
