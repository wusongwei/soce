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

#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "soce-redis-impl.h"
#include "utils/factory.hpp"
#include "utils/singleton.hpp"
#include "utils/time-helper.h"
#include "utils/str-helper.h"
#include "destructor.hpp"

using std::list;
using std::string;
using std::unordered_map;
using std::vector;
using std::ostringstream;
using namespace soce::utils;
using namespace std;

namespace soce{
namespace redis{

    // from redis-3.0.6/src/crc16.c
    static const uint16_t crc16tab[256]= {
        0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
        0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
        0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
        0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
        0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
        0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
        0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
        0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
        0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
        0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
        0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
        0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
        0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
        0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
        0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
        0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
        0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
        0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
        0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
        0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
        0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
        0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
        0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
        0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
        0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
        0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
        0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
        0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
        0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
        0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
        0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
        0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
    };

    static uint16_t crc16(const char *buf, size_t len) {
        size_t counter = 0;
        uint16_t crc = 0;
        for (counter = 0; counter < len; counter++){
            crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *buf++)&0x00FF];
        }
        return crc;
    }

    RedisCmd::RedisCmd(const string& key, const string& cmd)
    {
        key_ = key;
        slot_ = calc_hash_slot(key);
        cmd_ = cmd;
    }

    RedisCmd::RedisCmd(string&& key, string&& cmd)
    {
        key_ = std::move(key);
        slot_ = calc_hash_slot(key_);
        cmd_ = std::move(cmd);
    }

    // from redis-3.0.6/src/cluster.c:keyHashSlot()
    uint RedisCmd::calc_hash_slot(const string& skey)
    {
        const char* key = skey.c_str();
        size_t keylen = skey.size();
        size_t s = 0;
        size_t e = 0;

        for (s = 0; s < keylen; s++){
            if (key[s] == '{'){
                break;
            }
        }

        if (s == keylen){
            return crc16(key,keylen) & 0x3FFF;
        }

        for (e = s+1; e < keylen; e++){
            if (key[e] == '}'){
                break;
            }
        }

        if (e == keylen || e == s+1){
            return crc16(key,keylen) & 0x3FFF;
        }

        return crc16(key+s+1,e-s-1) & 0x3FFF;
    }

    RedisReply::RedisReply(redisReply* reply, bool own)
    {
        if (reply){
            reply_.reset(reply, [=](redisReply* reply){
                    if (own){
                        freeReplyObject(reply);
                    }
                });
        }
    }

    int RedisReply::get_type(SoceRedisType& type)
    {
        if (!reply_){
            type = kSRTTimeout;
            return 0;
        }

        switch (reply_->type){
        case REDIS_REPLY_STRING:
            type = kSRTString;
            break;

        case REDIS_REPLY_ARRAY:
            type = kSRTArray;
            break;

        case REDIS_REPLY_INTEGER:
            type = kSRTInteger;
            break;

        case REDIS_REPLY_NIL:
            type = kSRTNil;
            break;

        case REDIS_REPLY_STATUS:
            type = kSRTStatus;
            break;

        case REDIS_REPLY_ERROR:
            type = kSRTError;
            break;

        default:
            return -1;
        }

        return 0;
    }

    int RedisReply::get_str(string& val)
    {
        SoceRedisType type;
        if (get_type(type)){
            return -1;
        }

        switch (type){
        case kSRTString:
            if (!reply_->str){
                return -1;
            }
            val.assign(reply_->str, reply_->len);
            break;

        case kSRTNil:
            break;

        default:
            return -1;
        }

        return 0;
    }

    int RedisReply::get_array_size(int& size)
    {
        SoceRedisType type;
        if (get_type(type)){
            return -1;
        }

        if (type != kSRTArray){
            return -1;
        }

        size = reply_->elements;
        return 0;
    }

    int RedisReply::get_array_begin(SoceRedisIterator& iter)
    {
        SoceRedisType type;
        if (get_type(type) || type != kSRTArray){
            return -1;
        }

        RedisArrayReply& rar = SoceSingleton<RedisArrayReply>::get_instance();
        rar.clear();

        int size = reply_->elements;
        for (int i=0; i<size; ++i){
            rar.push(reply_->element[i]);
        }

        std::shared_ptr<SoceRedisReplyImpl> impl = SoceFactory<SoceRedisReplyImpl>::create(rar.get_replies().begin(), rar.get_replies().end());
        std::shared_ptr<SoceRedisReply> reply = SoceFactory<SoceRedisReply>::create(impl);
        iter = SoceRedisIterator(reply);

        return 0;
    }

    int RedisReply::get_array_end(SoceRedisIterator& iter)
    {
        SoceRedisType type;
        if (get_type(type) || type != kSRTArray){
            return -1;
        }

        RedisArrayReply& rar = SoceSingleton<RedisArrayReply>::get_instance();
        std::shared_ptr<SoceRedisReplyImpl> impl = SoceFactory<SoceRedisReplyImpl>::create(rar.get_replies().end(), rar.get_replies().end());
        std::shared_ptr<SoceRedisReply> reply = SoceFactory<SoceRedisReply>::create(impl);
        iter = SoceRedisIterator(reply);

        return 0;
    }

    int RedisReply::get_int(int& val)
    {
        SoceRedisType type;
        if (get_type(type)){
            return -1;
        }

        if (type != kSRTInteger){
            return -1;
        }

        val = reply_->integer;
        return 0;
    }

    int RedisReply::get_status(string& val)
    {
        SoceRedisType type;
        if (get_type(type)){
            return -1;
        }

        if (type != kSRTStatus || !reply_->str){
            return -1;
        }

        val.assign(reply_->str, reply_->len);
        return 0;
    }

    int RedisReply::get_error(string& val)
    {
        SoceRedisType type;
        if (get_type(type)){
            return -1;
        }

        if (type != kSRTError || !reply_->str){
            return -1;
        }

        val.assign(reply_->str, reply_->len);
        return 0;
    }

    int BasicClt::init(unordered_map<string, string>& confs)
    {
        // pasword
        auto iter = confs.find("passwd");
        if (iter == confs.end()){
            return -1;
        }
        passwd_ = iter->second;

        // addr
        iter = confs.find("addr");
        if (iter == confs.end()){
            return -1;
        }
        addrs_ = iter->second;

        // master
        iter = confs.find("master");
        if (iter != confs.end()){
            master_name_ = iter->second;
        }

        // timeout
        iter = confs.find("timeout");
        if (iter != confs.end()){
            timeout_ = (uint32_t)atoi(confs["timeout"].c_str());
        }

        // datebase
        iter = confs.find("db");
        if (iter != confs.end()){
            db_ = atoi(confs["db"].c_str());
        }

        return this->init();
    }


#if defined(SOCE_USE_COROUTINE)
    void BasicClt::wakeup(int fd)
    {
        if (!connected_ || buffer_read(fd)){
            connected_ = false;
            return keepalive();
        }

        more_reply_[fd] = true;
        do{
            int wait = trigger_->get_wait_fd();
            if (!connected_ || wait == 0 || !has_reply(wait)){
                break;
            }

            trigger_->wakeup_top();
        }while (1);
    }

    void BasicClt::keepalive()
    {
        if (connected_){
            return;
        }

        if (reset() || group(cmds_) || flush_cmds()){
            connected_ = false;
            return;
        }

        trigger_->reset();
    }
#endif

    BasicClt::RedisCtx BasicClt::make_conn(const string& ip, int port)
    {
        std::shared_ptr<redisContext> rc;
        redisContext* raw_ctx = connect(ip, port);

        if (!raw_ctx || raw_ctx->err || auth(raw_ctx) || select_db(raw_ctx)){
            return rc;
        }
        rc.reset(raw_ctx, free_redis_contex);

        return rc;
    }

    redisContext* BasicClt::connect(const string& ip, int port)
    {
        redisContext* ctx = NULL;

        // Ignore broken pipe signal
        signal(SIGPIPE, SIG_IGN);

        // {0, 0} for never timeout
        struct timeval timeout;
        timeout.tv_sec = timeout_ / 1000;
        timeout.tv_usec = timeout_ % 1000 * 1000;

        if (timeout.tv_sec == 0 && timeout.tv_usec == 0){
            ctx = redisConnect(ip.c_str(), port);
        }
        else {
            ctx = redisConnectWithTimeout(ip.c_str(), port, timeout);
            if (ctx){
                redisSetTimeout(ctx, timeout);
            }
        }

        return ctx;
    }

    int BasicClt::auth(redisContext* ctx)
    {
        if (passwd_ == "none"){
            return 0;
        }
        redisReply* reply = (redisReply*)redisCommand(ctx, "AUTH %s", passwd_.c_str());
        std::shared_ptr<redisReply> res(reply, free_redis_reply);

        if ((reply == NULL) || (reply->type == REDIS_REPLY_ERROR)){
            return -1;
        }

        return 0;
    }

    int BasicClt::select_db(redisContext* ctx)
    {
        if (db_ == -1){
            return 0;
        }

        redisReply* reply = (redisReply*)redisCommand(ctx, "SELECT %d", db_);
        std::shared_ptr<redisReply> res(reply, free_redis_reply);

        if ((reply == NULL) || (reply->type == REDIS_REPLY_ERROR)){
            return -1;
        }

        return 0;
    }

#if defined(SOCE_USE_COROUTINE)
    int NoClusterClt::buffer_read(int fd)
    {
        redisContext* ctx = ctx_.get();

        if (!ctx || ctx->fd != fd || redisBufferRead(ctx) != REDIS_OK){
            return -1;
        }

        return 0;
    }

    bool NoClusterClt::has_reply(int fd)
    {
        auto iter = more_reply_.find(fd);
        if (iter == more_reply_.end() || !iter->second){
            return false;
        }

        return true;
    }


    int NoClusterClt::flush_cmds()
    {
        redisContext* ctx = ctx_.get();
        if (!ctx){
            return -1;
        }

        int done = 0;
        while (done != 1){
            if (REDIS_OK != redisBufferWrite(ctx, &done)){
                return -1;
            }
        }

        return 0;
    }

#endif

    int NoClusterClt::group(list<RedisCmd>& cmds)
    {
        redisContext* ctx = ctx_.get();
        if (!ctx){
            return -1;
        }

        for (auto& i : cmds){
            if (redisAppendFormattedCommand(ctx, i.get_cmd(), i.size())){
                return -1;
            }
        }

        return 0;
    }

#if defined(SOCE_USE_COROUTINE)
    int NoClusterClt::exec(uint32_t cmd_size, list<RedisReply>& out)
    {
        bool is_timeout = false;
        bool own = true;
        uint64_t start_tm = TimeHelper::get_time_ms();

        for (uint32_t i=0; i<cmd_size; ++i){
            redisReply* reply = NULL;

            while (1){
                if (is_timeout){
                    own = false;
                    break;
                }

                redisContext* ctx = ctx_.get();
                if (!connected_ || flush_cmds() || REDIS_OK != redisGetReply(ctx, (void**)&reply)){
                    connected_ = false;

                    if (timeout_ != 0){
                        uint64_t cur_tm = TimeHelper::get_time_ms();
                        if (cur_tm - start_tm > timeout_){
                            is_timeout = true;
                            break;
                        }
                    }
                }
                else if(reply){
                    break;
                }

                more_reply_[ctx->fd] = false;
                CortTrigger::CortState state = trigger_->yield(ctx->fd);
                if (state == CortTrigger::kCortReset){
                    continue;
                }

                if (state == CortTrigger::kCortTimeout){
                    connected_ = false;
                    is_timeout = true;
                }
            }

            out.emplace_back(reply, own);
            cmds_.pop_front();
            reply = NULL;
        }

        trigger_->pop();
        return 0;
    }

#else

    int NoClusterClt::exec(uint32_t cmd_size, list<RedisReply>& out)
    {
        bool is_timeout = false;
        bool own = true;
        uint64_t start_tm = TimeHelper::get_time_ms();

        for (uint32_t i = 0; i<cmd_size; ++i) {
            redisReply* reply = NULL;

            do{
                if (is_timeout){
                    own = false;
                    connected_ = false;
                    break;
                }

                redisContext* ctx = ctx_.get();
                if (!ctx || !connected_ || REDIS_OK != redisGetReply(ctx, (void**)&reply)){
                    connected_ = false;
                    int sleep_time = 1000;
                    do{
                        if (timeout_ != 0){
                            uint64_t cur_tm = TimeHelper::get_time_ms();
                            if (cur_tm - start_tm > timeout_){
                                is_timeout = true;
                                break;
                            }
                        }

                        usleep(sleep_time);
                        if (reset() == 0){
                            if (group(cmds_) == 0){
                                break;
                            }
                            connected_ = false;
                        }
                        sleep_time = min(1000000, sleep_time + 1000);
                    }while(1);
                }
                else{
                    break;
                }

            }while(1);

            out.emplace_back(reply, own);
            cmds_.pop_front();
            reply = NULL;
        }

        return 0;
    }
#endif

    int SingleClt::init()
    {
        vector<string> vec_addr;
        StrHelper::split(addrs_, ",", vec_addr);
        if (vec_addr.size() != 1){
            return -1;
        }

        vector<string> vec_ipport;
        StrHelper::split(vec_addr[0], ":", vec_ipport);
        if (vec_ipport.size() != 2){
            return -1;
        }

        ip_ = vec_ipport[0];
        port_ = atoi(vec_ipport[1].c_str());

        return reset();
    }

    int SingleClt::reset()
    {
#if defined(SOCE_USE_COROUTINE)
        if (ctx_){
            cort_env_.unwatch(ctx_->fd);
        }
#endif

        ctx_ = make_conn(ip_, port_);
        if (!ctx_){
            return -1;
        }

#if defined(SOCE_USE_COROUTINE)
        ctx_->flags &= ~REDIS_BLOCK;
        cort_env_.watch(ctx_->fd, std::bind(&BasicClt::wakeup,
                                            this,
                                            std::placeholders::_1));
#endif
        connected_ = true;
        return 0;
    }

    int SentinelClt::init()
    {
        master_port_ = 0;

        vector<string> vec_addr;
        StrHelper::split(addrs_, ",", vec_addr);
        if (vec_addr.empty()){
            return -1;
        }

        for (auto& i : vec_addr){
            vector<string> vec_ipport;
            StrHelper::split(i, ":", vec_ipport);
            if (vec_ipport.size() != 2){
                return -1;
            }

            sentinels_.push_back(make_pair(vec_ipport[0], atoi(vec_ipport[1].c_str())));
        }

        return reset();
    }

    int SentinelClt::reset()
    {
#if defined(SOCE_USE_COROUTINE)
        if (ctx_){
            cort_env_.unwatch(ctx_->fd);
        }
#endif

        for (auto& i : sentinels_){
            if (get_master(i.first, i.second)){
                continue;
            }
            ctx_ = make_conn(master_ip_, master_port_);
            if (ctx_){
                break;
            }
        }
#if defined(SOCE_USE_COROUTINE)
        if (ctx_){
            ctx_->flags &= ~REDIS_BLOCK;
            cort_env_.watch(ctx_->fd, std::bind(&BasicClt::wakeup,
                                                         this,
                                                        std::placeholders::_1));
        }
#endif
        int rc = -1;
        if (ctx_){
            rc = 0;
            connected_ = true;
        }
        return rc;
    }

    int SentinelClt::get_master(const string& ip, int port)
    {
        redisContext* ctx = connect(ip, port);
        std::shared_ptr<redisContext> res_ctx(ctx, free_redis_contex);

        redisReply* reply = (redisReply*)redisCommand(ctx, "SENTINEL get-master-addr-by-name %s", master_name_.c_str());
        std::shared_ptr<redisReply> res_reply(reply, free_redis_reply);

        if (!reply || (reply->type != REDIS_REPLY_ARRAY) || (reply->elements != 2)){
            return -1;
        }

        master_ip_ = (*reply->element)->str;
        master_port_ = atoi((*(reply->element+1))->str);

        return 0;
    }

    int ClusterClt::init()
    {
        vector<string> masters;

        StrHelper::split(addrs_, ",", masters);

        for (auto& i : masters){
            vector<string> vec_ipport;
            StrHelper::split(i, ":", vec_ipport);
            if (vec_ipport.size() != 2){
                return -1;
            }

            string ip = vec_ipport[0];
            int port = atoi(vec_ipport[1].c_str());
            masters_.push_back(make_pair(ip, port));
        }

        return reset();
    }

#if defined(SOCE_USE_COROUTINE)
    int ClusterClt::buffer_read(int fd)
    {
        redisContext* ctx = NULL;
        for (auto& i : ctxs_){
            if (i.ctx.get()->fd == fd){
                ctx = i.ctx.get();
                break;
            }
        }

        if (!ctx || REDIS_OK != redisBufferRead(ctx)){
            return -1;
        }
        return 0;
    }

    bool ClusterClt::has_reply(int fd)
    {
        auto iter = more_reply_.find(fd);
        if (iter == more_reply_.end() || !iter->second){
            return false;
        }

        return true;
    }

    int ClusterClt::flush_cmds()
    {
        for (auto& i : ctxs_){
            redisContext* ctx = i.ctx.get();
            if (!ctx){
                return -1;
            }

            int done = 0;
            while (done != 1){
                if (REDIS_OK != redisBufferWrite(ctx, &done)){
                    return -1;
                }
            }
        }

        return 0;
    }
#endif

    int ClusterClt::reset()
    {
#if defined(SOCE_USE_COROUTINE)
         for (auto& i : ctxs_){
             cort_env_.unwatch(i.ctx->fd);
         }
#endif

        bool ok = false;
        if (get_masters()){
            return -1;
        }

        for (auto& i : masters_){
            BasicClt::RedisCtx ctx = make_conn(i.first, i.second);
            if (!ctx){
                continue;
            }

            redisReply* reply = (redisReply*)redisCommand(ctx.get(), "CLUSTER SLOTS");
            std::shared_ptr<redisReply> res(reply, free_redis_reply);
            if ((reply == NULL) || (reply->type != REDIS_REPLY_ARRAY)){
                continue;
            }

            bool is_ok = true;
            vector<std::pair<string, int> > masters;
            vector<Node> ctxs;
            for (size_t i=0; i<reply->elements; ++i){
                if (reply->element[i]->type != REDIS_REPLY_ARRAY ||
                    reply->element[i]->elements < 3){
                    is_ok = false;
                    break;
                }

                Node nd;
                string ip;
                int port = 0;
                redisReply* rp = reply->element[i]->element[0];
                if (rp->type != REDIS_REPLY_INTEGER){
                    is_ok = false;
                    break;
                }
                nd.beg = rp->integer;

                rp = reply->element[i]->element[1];
                if (rp->type != REDIS_REPLY_INTEGER){
                    is_ok = false;
                    break;
                }
                nd.end = rp->integer;

                rp = reply->element[i]->element[2];
                if (rp->type != REDIS_REPLY_ARRAY ||
                    rp->elements != 2){
                    is_ok = false;
                    break;
                }

                redisReply* rp_ipport = rp->element[0];
                if (rp_ipport->type != REDIS_REPLY_STRING){
                    is_ok = false;
                    break;
                }
                ip.assign(rp_ipport->str, rp_ipport->len);

                rp_ipport = rp->element[1];
                if (rp_ipport->type != REDIS_REPLY_INTEGER){
                    is_ok = false;
                    break;
                }
                port = rp_ipport->integer;

                masters.push_back(make_pair(ip, port));
                nd.ctx = make_conn(ip, port);
                if (!nd.ctx){
                    is_ok = false;
                    break;
                }
#if defined(SOCE_USE_COROUTINE)
                nd.ctx->flags &= ~REDIS_BLOCK;
                cort_env_.watch(nd.ctx->fd,
                                 std::bind(&BasicClt::wakeup,
                                           this,
                                           std::placeholders::_1));
#endif
                ctxs.push_back(nd);
            }

            if (is_ok){
                masters_ = masters;
                ctxs_ = ctxs;
                ok = true;
                break;
            }
        }

        int rc = -1;
        if (ok){
            rc = 0;
            connected_ = true;
        }
        return rc;
    }

    int ClusterClt::group(list<RedisCmd>& cmds)
    {
        for (auto& i : cmds){
            redisContext* ctx = get_ctx_from_slot(i.get_slot());
            if (!ctx){
                return -1;
            }
            if (redisAppendFormattedCommand(ctx, i.get_cmd(), i.size())){
                return -1;
            }
        }

        return 0;
    }

#if defined(SOCE_USE_COROUTINE)
    int ClusterClt::exec(uint32_t cmd_size, list<RedisReply>& out)
    {
        bool is_timeout = false;
        bool own = true;
        redisContext* ctx = NULL;
        uint64_t start_tm = TimeHelper::get_time_ms();

        for (uint32_t i=0; i<cmd_size; ++i){
            redisReply* reply = NULL;
            do{
                if (is_timeout){
                    own = false;
                    break;
                }

                ctx = get_ctx_from_slot(cmds_.front().get_slot());
                if (!connected_ || !ctx || flush_cmds() || REDIS_OK != redisGetReply(ctx, (void**)&reply)){
                    connected_ = false;

                    if (timeout_ != 0){
                        uint64_t cur_tm = TimeHelper::get_time_ms();
                        if (cur_tm - start_tm > timeout_){
                            is_timeout = true;
                            break;
                        }
                    }
                }
                else if (reply){
                    break;
                }

                more_reply_[ctx->fd] = false;
                CortTrigger::CortState state = trigger_->yield(ctx->fd);
                if (state == CortTrigger::kCortReset){
                    continue;
                }

                if (state == CortTrigger::kCortTimeout){
                    connected_ = false;
                    is_timeout = true;
                }
            }while(1);

            out.emplace_back(reply, own);
            cmds_.pop_front();
            reply = NULL;
        }

        trigger_->pop();
        return 0;
    }
#else
    int ClusterClt::exec(uint32_t cmd_size, list<RedisReply>& out)
    {
        bool is_timeout = false;
        bool own = true;
        uint64_t start_tm = TimeHelper::get_time_ms();
        redisContext* ctx = NULL;

        for (uint32_t i=0; i<cmd_size; ++i){
            redisReply* reply = NULL;
            do{
                if (is_timeout){
                    own = false;
                    connected_ = false;
                    break;
                }

                ctx = get_ctx_from_slot(cmds_.front().get_slot());
                if (!connected_
                    || !ctx
                    || REDIS_OK != redisGetReply(ctx, (void**)&reply)
                    || (reply->type == REDIS_REPLY_ERROR
                        && strncmp(reply->str, "MOVED", 5) == 0)){
                    connected_ = false;
                    int sleep_time = 1000;
                    do{
                        if (timeout_ != 0){
                            uint64_t cur_tm = TimeHelper::get_time_ms();
                            if (cur_tm - start_tm > timeout_){
                                is_timeout = true;
                                break;
                            }
                        }

                        usleep(sleep_time);
                        if (reset() == 0){
                            if (group(cmds_) == 0){
                                break;
                            }
                            connected_ = false;
                        }
                        sleep_time = min(1000000, sleep_time * 2);
                    }while(1);
                }
                else{
                    break;
                }
            }while(1);

            out.emplace_back(reply, own);
            cmds_.pop_front();
            reply = NULL;
        }

        return 0;
    }
#endif

    int ClusterClt::get_masters()
    {
        std::vector<std::pair<std::string, int> > masters;
        for (auto& i : masters_){
            redisContext* ctx = connect(i.first, i.second);
            std::shared_ptr<redisContext> res_ctx(ctx, free_redis_contex);

            redisReply* reply = (redisReply*)redisCommand(ctx, "CLUSTER NODES");
            std::shared_ptr<redisReply> res_reply(reply, free_redis_reply);

            if (!reply || (reply->type != REDIS_REPLY_STRING)){
                continue;
            }

            std::vector<string> nodes;
            StrHelper::split(reply->str, "\n", nodes);

            for (auto& i : nodes){
                std::vector<string> node_info;
                StrHelper::split(i, " ", node_info);
                if (node_info.size() < 3){
                    continue;
                }
                const char* c = node_info[2].c_str() + node_info[2].size() - 6;
                if (strncmp("master", c, 6) == 0){
                    std::vector<string> addr;
                    StrHelper::split(node_info[1], ":", addr);
                    if (addr.size() != 2){
                        continue;
                    }
                    masters.push_back(std::make_pair(addr[0], (int)atoi(addr[1].c_str())));
                }
            }
            break;
        }

        if (!masters.empty()){
            masters_.swap(masters);
        }

        return 0;
    }

    redisContext* ClusterClt::get_ctx_from_slot(uint slot)
    {
        for (auto& i : ctxs_){
            if (slot >= i.beg && slot < i.end){
                return i.ctx.get();
            }
        }

        return NULL;
    }


    SoceRedisReplyImpl::SoceRedisReplyImpl(std::list<RedisReply>::iterator iter,
                                           std::list<RedisReply>::iterator end)
        : iter_(iter), end_(end)
    {
    }

    SoceRedisReplyImpl::SoceRedisReplyImpl(const SoceRedisReplyImpl& rhs)
        : iter_(rhs.iter_), end_(rhs.end_)
    {
    }

    SoceRedisReplyImpl& SoceRedisReplyImpl::operator=(const SoceRedisReplyImpl& rhs)
    {
        if (this != &rhs){
            iter_ = rhs.iter_;
            end_ = rhs.end_;
        }
        return *this;
    }

    SoceRedisReplyImpl& SoceRedisReplyImpl::operator++()
    {
        ++iter_;
        return *this;
    }

    SoceRedisReplyImpl SoceRedisReplyImpl::operator++(int)
    {
        SoceRedisReplyImpl tmp(*this);
        ++iter_;
        return tmp;
    }

    bool SoceRedisReplyImpl::operator==(const SoceRedisReplyImpl& rhs)
    {
        return (iter_ == rhs.iter_) ? true : false;
    }

    bool SoceRedisReplyImpl::operator!=(const SoceRedisReplyImpl& rhs)
    {
        return (iter_ != rhs.iter_) ? true : false;
    }

    int SoceRedisReplyImpl::get_type(SoceRedisType& type)
    {
        return get([this, &type]{
                return iter_->get_type(type);
            });
    }

    int SoceRedisReplyImpl::get_str(string& val)
    {
        return get([this, &val]{
                return iter_->get_str(val);
            });
    }

    int SoceRedisReplyImpl::get_array_size(int& size)
    {
        return get([this, &size]{
                return iter_->get_array_size(size);
            });
    }

    int SoceRedisReplyImpl::get_array_begin(SoceRedisIterator& iter)
    {
        return get([this, &iter]{
                return iter_->get_array_begin(iter);
            });
    }

    int SoceRedisReplyImpl::get_array_end(SoceRedisIterator& iter)
    {
        return get([this, &iter]{
                return iter_->get_array_end(iter);
            });
    }

    int SoceRedisReplyImpl::get_int(int& val)
    {
        return get([this, &val]{
                return iter_->get_int(val);
            });
    }

    int SoceRedisReplyImpl::get_status(string& val)
    {
        return get([this, &val]{
                return iter_->get_status(val);
            });
    }

    int SoceRedisReplyImpl::get_error(string& val)
    {
        return get([this, &val]{
                return iter_->get_error(val);
            });
    }

    int SoceRedisReplyImpl::get(std::function<int()> getter)
    {
        if (iter_ == end_){
            return -1;
        }

        return getter();
    }

#if defined(SOCE_USE_COROUTINE)
    int SoceRedisImpl::init(const string& conf, const SoceRedis::CortEnv& env){
        assert(env.watch && env.unwatch);
        cort_env_ = env;
        return init(conf);
    }

    void SoceRedisImpl::keepalive()
    {
        clt_->keepalive();
    }
#endif

/*
 * passwd@ip:port,ip:port,...,ip:port?db=0&timeout=10&master=mastername
 * passwd@ip:port,ip:port,...,ip:port?db=0&timeout=10&cluster=true
 */
    int SoceRedisImpl::init(const string& conf)
    {
        unordered_map<string, string> confs;
        if (parse_conf(conf, confs)){
            return -1;
        }

        return create_clt(confs);
    }

    int SoceRedisImpl::exec(SoceRedis::Fmt fmt)
    {
        std::list<RedisCmd> cmd;
        int rc = append(cmd, fmt);
        if (rc){
            return -1;
        }

        if (do_cmd(cmd)){
            return -1;
        }

        return 0;
    }

    int SoceRedisImpl::parse_conf(const string& conf_str,
                                             unordered_map<string, string>& confs)
    {
        string str = conf_str;
        StrHelper::trim(str);

        // get passwd
        vector<string> vec_pass;
        StrHelper::split(str, "@", vec_pass);
        if (vec_pass.size() != 2){
            return -1;
        }
        confs["passwd"] = vec_pass[0];

        if (vec_pass[1].find_first_of('?') == string::npos){
            confs["addr"] = vec_pass[1];
            return 0;
        }

        // get addr
        vector<string> vec_addr;
        StrHelper::split(vec_pass[1], "?", vec_addr);
        if (vec_addr.size() != 2){
            return -1;
        }
        confs["addr"] = vec_addr[0];

        //get other conf
        vector<string> vec_op;
        StrHelper::split(vec_addr[1], "&", vec_op);
        for (vector<string>::const_iterator iter = vec_op.begin();
             iter!=vec_op.end(); ++iter){
            vector<string> vec_other;
            StrHelper::split(*iter, "=", vec_other);
            if (vec_other.size() != 2){
                return -1;
            }
            confs[vec_other[0]] = vec_other[1];
        }

        return 0;
    }

    int SoceRedisImpl::create_clt(unordered_map<string, string>& confs)
    {
        unordered_map<string, string>::const_iterator iter;
        do{
            iter = confs.find("cluster");
            if (iter != confs.end()){
                clt_ = SoceFactory<ClusterClt>::create();
                break;
            }

            iter = confs.find("master");
            if (iter != confs.end()){
                clt_ = SoceFactory<SentinelClt>::create();
                break;
            }

            clt_ = SoceFactory<SingleClt>::create();
        }while(0);

#if defined(SOCE_USE_COROUTINE)
        if (cort_env_.watch && cort_env_.unwatch){
            clt_->set_cort_env(cort_env_);
        }
#endif
        return clt_->init(confs);
    }

    int SoceRedisImpl::append(SoceRedis::Fmt fmt)
    {
        return append(pipeline_cmd_, fmt);
    }

    int SoceRedisImpl::append(std::list<RedisCmd>& cmds, SoceRedis::Fmt fmt)
    {
        Param pa;
        fmt(pa);
        const vector<string>& params = pa.get_params();
        if (params.empty()){
            return -1;
        }

        ostringstream oss;
        oss << "*" << params.size() << "\r\n";
        for (auto& i : params){
            oss << "$" << i.size() << "\r\n" << i << "\r\n";
        }

        cmds.emplace_back(std::move(pa.get_key()), std::move(oss.str()));
        return 0;
    }

    int SoceRedisImpl::do_cmd()
    {
        return do_cmd(pipeline_cmd_);
    }

    int SoceRedisImpl::do_cmd(std::list<RedisCmd>& cmds)
    {
        uint64_t index = cmd_index_;
        auto size =  cmds.size();
        if (clt_->group(cmds)){
            return -1;
        }

        clt_->save_cmds(cmds);
        cmd_index_ += size;

        if (clt_->exec(size, replies_)){
            return -1;
        }

        for (uint64_t i = cmd_confirmed_; i < index; ++i){
            replies_.pop_front();
        }
        cmd_confirmed_ = index;

        return 0;
    }

    void SoceRedisImpl::clear()
    {
        pipeline_cmd_.clear();
    }

    SoceRedis::iterator SoceRedisImpl::begin()
    {
        std::shared_ptr<SoceRedisReplyImpl> impl = SoceFactory<SoceRedisReplyImpl>::create(replies_.begin(), replies_.end());
        std::shared_ptr<SoceRedisReply> reply = SoceFactory<SoceRedisReply>::create(impl);
        return SoceRedisIterator(reply);
    }

    SoceRedis::iterator SoceRedisImpl::end()
    {
        std::shared_ptr<SoceRedisReplyImpl> impl = SoceFactory<SoceRedisReplyImpl>::create(replies_.end(), replies_.end());
        std::shared_ptr<SoceRedisReply> reply = SoceFactory<SoceRedisReply>::create(impl);
        return SoceRedisIterator(reply);
    }

}} //end namespace
