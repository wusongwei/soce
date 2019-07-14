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
 * Access the server in coroutine with connection pool.
 */

#include <iostream>
#include <unistd.h>
#include "soce-redis/soce-redis.h"
#include "transport/transport-libevent.h"
#include "transport/bypass-processor.h"
#include "cort-engine/cort-engine.h"
#include "log4rel/logger.h"

using namespace std;
using namespace soce::redis;
using namespace soce::transport;
using namespace soce::cortengine;
using namespace soce::log4rel;

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
    int32_t init(const std::string& conf)
        {
            return clt_.init(conf);
        }

    int32_t sample_single_cmd()
        {
            cout << "----------SampleSingleCmd----------" << endl;
            int rc = 0;

            // del first
            rc = soce_redis_docmd(clt_, "del" << "a");
            CHECK_RC(rc, "SampleSingleCmd del a");

            // do set
            rc = soce_redis_docmd(clt_, "set" << "a" << "b");
            CHECK_RC(rc, "SampleSingleCmd set a");
            string val;
            rc = clt_.begin()->get_status(val);
            CHECK_RC(rc, "get_status");
            cout << "SET status = " << val << endl;

            // do get
            rc = soce_redis_docmd(clt_, "get" << "a");
            CHECK_RC(rc, "SampleSingleCmd get a");
            rc = clt_.begin()->get_str(val);
            CHECK_RC(rc, "get_str");

            cout << "key = a, value = " << val << endl << endl;

            // clear
            rc = soce_redis_docmd(clt_, "del" << "a");
            CHECK_RC(rc, "SampleSingleCmd del");
            return 0;
        }

    int32_t sample_multi_cmd()
        {
            cout << "----------SampleMultiCmd----------" << endl;
            SoceRedisIterator iter;
            int rc = 0;

            rc = soce_redis_docmd(clt_, "mset" << "a" << "b" << "c" << "d");
            CHECK_RC(rc, "SampleMultiCmd mset a");

            rc = soce_redis_docmd(clt_, "mget" << "a" << "c" << "f");
            CHECK_RC(rc, "SampleMultiCmd mget");

            int size = 0;
            rc = clt_.begin()->get_array_size(size);
            CHECK_RC(rc, "SampleMultiCmd get_array_size");
            cout << "Array size = " << size << endl;

            SoceRedisIterator it;
            SoceRedisIterator it_end;

            rc = clt_.begin()->get_array_begin(it);
            CHECK_RC(rc, "SampleMultiCmd get_array_begin");

            rc = clt_.begin()->get_array_end(it_end);
            CHECK_RC(rc, "SampleMultiCmd get_array_end");

            for (; it != it_end; ++it){
                SoceRedisType type;
                rc = it->get_type(type);
                CHECK_RC(rc, "SampleMultiCmd get_type");

                cout << "type = " << type << " ";
                if (type != kSRTNil){
                    string str;
                    rc = it->get_str(str);
                    CHECK_RC(rc, "get_str");
                    cout << "value = " << str << endl;
                }
            }

            cout << endl << endl;
            return 0;
        }

    int32_t sample_pipeline_cmd()
        {
            cout << "----------SamplePipeline----------" << endl;

            int rc = 0;
            // clear fist
            SoceRedisIterator iter;
            rc = soce_redis_docmd(clt_, "del" << "a");
            CHECK_RC(rc, "SamplePipeline del a");

            rc = soce_redis_docmd(clt_, "del" << "c");
            CHECK_RC(rc, "SamplePipeline del c");

            rc = soce_redis_docmd(clt_, "del" << "e");
            CHECK_RC(rc, "SamplePipeline del c");

            // do set
            clt_.clear();
            rc = soce_redis_append(clt_, "set" << "a" << "b");
            CHECK_RC(rc, "SamplePipeline set a");

            rc = soce_redis_append(clt_, "set" << "c" << "d");
            CHECK_RC(rc, "SamplePipeline set c");

            rc = clt_.pipeline();
            CHECK_RC(rc, "pipeline");

            for (auto& i : clt_){
                string val;
                rc = i.get_status(val);
                CHECK_RC(rc, "get_status");

                cout << "SET Status = " << val << endl;

            }

            clt_.clear();
            rc = soce_redis_append(clt_, "get" << "a");
            CHECK_RC(rc, "SamplePipeline get a");

            rc = soce_redis_append(clt_, "get" << "c");
            CHECK_RC(rc, "SamplePipeline get c");

            rc = soce_redis_append(clt_, "get" << "e");
            CHECK_RC(rc, "SamplePipeline get e");

            rc = clt_.pipeline();
            CHECK_RC(rc, "SamplePipeline pipeline");

            for (auto& i : clt_){
                SoceRedisType type;
                rc = i.get_type(type);
                CHECK_RC(rc, "SamplePipeline get_type");

                cout << "type = " << type << " ";
                if (type != kSRTNil){
                    string str;
                    rc = i.get_str(str);
                    CHECK_RC(rc, "SamplePipeline get_str");
                    cout << "value = " << str << endl;
                }
            }

            cout << endl << endl;
            return 0;
        }

private:
    SoceRedis clt_;
};

int main()
{
    /*
     * The defaut stack size for coroutine is 8K,
     * but a local variable used by function redisBufferRead() in hiredis asked for 16K,
     * so we have to resize the stack size.
     */
    CortEngine::set_stack_size(64 * 1024);
    SOCE_CUR_LOGGER->set_log_level(kLogLevelInfo);

    TransportLibevent tle;
    tle.init();

    /*
     * Hook the read(),
     * add a watcher for the hooked fd,
     * and wake up the coroutine when data is comming.
     */
    std::shared_ptr<ProcessorIf> processor(new BypassProcessor([&](int fd){
                tle.unwatch(fd);
                SCortEngine.wakeup(fd);
            }));
    SCortEngine.set_hook_callback([&](int fd){
            tle.watch(fd, processor);
        });

    CortConnPool<std::shared_ptr<RedisSample>> pool(0, 1, [](){
            string conf = "none@127.0.0.1:6379";
            std::shared_ptr<RedisSample> sample(new RedisSample);
            sample->init(conf);
            return sample;
        });

    for (size_t i=0; i<2; ++i){
        CortEngine::CortId id = SCortEngine.create([&]{
                SCortEngine.enable_hook(0);
                std::shared_ptr<RedisSample> r;
                pool.get(r);
                r->sample_single_cmd();
                r->sample_multi_cmd();
                r->sample_pipeline_cmd();
                pool.put(r);
            });
        SCortEngine.resume(id);
    }

    tle.run();

    return 0;
}
