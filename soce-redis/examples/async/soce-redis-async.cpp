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

#include "soce-redis-async.h"

SoceRedisAsync::SoceRedisAsync()
{
    thread_ = std::thread(&SoceRedisAsync::thread_entry, this);

}

SoceRedisAsync::~SoceRedisAsync()
{
    if (thread_.joinable()){
        thread_.join();
    }
}

int SoceRedisAsync::init(std::string conf)
{
    pool_ = std::shared_ptr<CortConnPool<std::shared_ptr<SoceRedis>>>(new CortConnPool<std::shared_ptr<SoceRedis>>(5, 100, [=](){
                std::shared_ptr<SoceRedis> soce_redis(new SoceRedis);
                soce_redis->init(conf);
                return soce_redis;
            }));
    return 0;
}

void SoceRedisAsync::exec_async(SoceRedis::Fmt fmt, Callback callback)
{
    queue_.produce(fmt, callback);
}

void SoceRedisAsync::thread_entry()
{
    CortEngine::set_stack_size(64 * 1024);
    SOCE_CUR_LOGGER->set_log_level(kLogLevelInfo);

    TransportLibevent tle;
    tle.init();

    std::shared_ptr<ProcessorIf> processor(new BypassProcessor([&](int fd){
                tle.unwatch(fd);
                SCortEngine.wakeup(fd);
            }));
    SCortEngine.set_hook_callback([&](int fd){
            tle.watch(fd, processor);
        });


    tle.watch(queue_.get_consumer_fd(0),
              std::shared_ptr<BypassProcessor>(new BypassProcessor(
                                                   std::bind(&SoceRedisAsync::handle_request,
                                                             this,
                                                             std::placeholders::_1))));
    tle.run();
}

void SoceRedisAsync::handle_request(int fd)
{
    (void) fd;

    soce::utils::FQVector<AsyncRequest> requests;
    if (queue_.try_consume(0, requests)){
        return;
    }

    for (auto& i : requests){
        CortEngine::CortId id = SCortEngine.create([=]{
                SCortEngine.enable_hook(0);
                std::shared_ptr<SoceRedis> soce_redis;
                pool_->get(soce_redis);
                soce_redis->exec(i.fmt);
                pool_->put(soce_redis);
                i.callback(soce_redis);
            });
        SCortEngine.resume(id);
    }
}
