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
 * Access the server without coroutine.
 */

#include <iostream>
#include <unistd.h>
#include "soce-redis/soce-redis.h"
using namespace std;
using namespace soce::redis;

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

            // clear first
            rc = soce_redis_docmd(clt_, "del" << key1_);
            CHECK_RC(rc, "SampleSingleCmd del key1");

            // do set
            rc = soce_redis_docmd(clt_, "set" << key1_ << value1_);
            CHECK_RC(rc, "SampleSingleCmd set");
            string val;
            rc = clt_.begin()->get_status(val);
            CHECK_RC(rc, "get_status");
            cout << "SET status = " << val << endl;

            // do get
            rc = soce_redis_docmd(clt_, "get" << key1_);
            CHECK_RC(rc, "SampleSingleCmd get key1");
            rc = clt_.begin()->get_str(val);
            CHECK_RC(rc, "get_str");

            cout << "key = " << key1_ << ", value = " << val << endl << endl;

            // clear
            rc = soce_redis_docmd(clt_, "del" << key1_);
            CHECK_RC(rc, "SampleSingleCmd del");
            return 0;
        }

    int32_t sample_multi_cmd()
        {
            cout << "----------SampleMultiCmd----------" << endl;
            SoceRedisIterator iter;
            int rc = 0;

            // clear first
            rc = soce_redis_docmd(clt_, "del" << key1_);
            CHECK_RC(rc, "SamplePipeline del key1");

            rc = soce_redis_docmd(clt_, "del" << key2_);
            CHECK_RC(rc, "SamplePipeline del key2");

            rc = soce_redis_docmd(clt_, "del" << key3_);
            CHECK_RC(rc, "SamplePipeline del key3");

            // do mset
            rc = soce_redis_docmd(clt_, "mset" << key1_ << value1_ << key2_ << value2_);
            CHECK_RC(rc, "SampleMultiCmd mset");

            // do mget
            rc = soce_redis_docmd(clt_, "mget" << key1_ << key2_ << key3_);
            CHECK_RC(rc, "SampleMultiCmd mget");

            // parse the result
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
            rc = soce_redis_docmd(clt_, "del" << key1_);
            CHECK_RC(rc, "SamplePipeline del key1");

            rc = soce_redis_docmd(clt_, "del" << key2_);
            CHECK_RC(rc, "SamplePipeline del key2");

            rc = soce_redis_docmd(clt_, "del" << key3_);
            CHECK_RC(rc, "SamplePipeline del key3");

            // do set
            clt_.clear();
            rc = soce_redis_append(clt_, "set" << key1_ << value1_);
            CHECK_RC(rc, "SamplePipeline set key1");

            rc = soce_redis_append(clt_, "set" << key2_ << value2_);
            CHECK_RC(rc, "SamplePipeline set key2");

            rc = clt_.pipeline();
            CHECK_RC(rc, "pipeline");

            for (auto& i : clt_){
                string val;
                rc = i.get_status(val);
                CHECK_RC(rc, "get_status");

                cout << "SET Status = " << val << endl;

            }

            clt_.clear();
            rc = soce_redis_append(clt_, "get" << key1_);
            CHECK_RC(rc, "SamplePipeline get key1");

            rc = soce_redis_append(clt_, "get" << key2_);
            CHECK_RC(rc, "SamplePipeline get key2");

            rc = soce_redis_append(clt_, "get" << key3_);
            CHECK_RC(rc, "SamplePipeline get key3");

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
    const std::string key1_ = "a";
    const std::string value1_ = "b";
    const std::string key2_ = "c";
    const std::string value2_ = "d";
    const std::string key3_ = "e";
};

int main()
{
    int rc = 0;
    string conf = "none@127.0.0.1:6379";
    // none : no passwd
    //string conf = "none@127.0.0.1:30001,127.0.0.1:30002,127.0.0.1:30006?cluster=true";

    RedisSample sample;
    rc = sample.init(conf);
    CHECK_RC(rc, "init");

    sample.sample_single_cmd();
    sample.sample_multi_cmd();
    sample.sample_pipeline_cmd();

    return 0;
}
