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

#ifndef _SOCE_REDIS_IMPL_D9Dt82BspZ_H_
#define _SOCE_REDIS_IMPL_D9Dt82BspZ_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <hiredis/hiredis.h>
#include <cassert>
#include "soce-redis/soce-redis.h"
#include "soce-redis/param.hpp"

#if defined(SOCE_USE_COROUTINE)
#include "cort/cort-trigger.h"
#include "transport/transport-if.h"
#endif

namespace soce{
namespace redis{

    class RedisCmd
    {
    public:
        RedisCmd(const std::string& key, const std::string& cmd);
        RedisCmd(std::string&& key, std::string&& cmd);
        ~RedisCmd() = default;

        inline const std::string& get_key()
        {
            return key_;
        }

        inline uint32_t get_slot()
        {
            return slot_;
        }

        inline const char* get_cmd()
        {
            return cmd_.c_str();
        }

        inline size_t size()
        {
            return cmd_.size();
        }

    private:
        uint32_t calc_hash_slot(const std::string& key);

    private:
        std::string key_;
        std::string cmd_;
        uint32_t slot_ = 0;
    };

    class RedisReply
    {
    public:
        RedisReply(redisReply* reply, bool own);
        ~RedisReply() = default;

        /*
         * see hiredis/read.h REDIS_REPLY_XXX
         */
        int get_type(SoceRedisType& type);
        int get_str(std::string& val);
        int get_array_size(int& size);
        int get_array_begin(SoceRedisIterator& iter);
        int get_array_end(SoceRedisIterator& iter);
        int get_int(int& val);
        int get_status(std::string& val);
        int get_error(std::string& val);

    public:
        std::shared_ptr<redisReply> reply_;
    };

    class RedisArrayReply
    {
    public:
        inline void push(redisReply* reply){
            replies_.emplace_back(reply, false);
        }

        inline std::list<RedisReply>& get_replies(){
            return replies_;
        }

        inline void clear(){
            replies_.clear();
        }

        size_t size(){
            return replies_.size();
        }
    private:
        std::list<RedisReply> replies_;
    };

    class RedisCltIf
    {
    public:
        virtual int init(std::unordered_map<std::string, std::string>& confs) = 0;
        virtual int group(std::list<RedisCmd>& cmds) = 0;
        virtual void save_cmds(std::list<RedisCmd>& cmds) = 0;
        virtual int exec(uint32_t cmd_size,
                         std::list<RedisReply>& out) = 0;
        virtual int reset() = 0;
        virtual bool connected() = 0;

#if defined(SOCE_USE_COROUTINE)
        virtual void keepalive() = 0;
        virtual void set_cort_env(SoceRedis::CortEnv env) = 0;
#endif
    };

    class BasicClt : public RedisCltIf
    {
    public:
        using RedisCtx = std::shared_ptr<redisContext>;

    public:
        virtual int init() = 0;
        virtual int reset() = 0;

    public:
        int init(std::unordered_map<std::string, std::string>& confs);
        inline bool connected()
        {
            return connected_;
        }

        inline void save_cmds(std::list<RedisCmd>& cmds)
        {
            cmds_.splice(cmds_.end(), cmds, cmds.begin(), cmds.end());
        }


#if defined(SOCE_USE_COROUTINE)
    public:
        virtual int buffer_read(int fd) = 0;
        virtual bool has_reply(int fd) = 0;
        virtual int flush_cmds() = 0;
        void wakeup(int fd);
        void keepalive();
        inline void set_cort_env(SoceRedis::CortEnv env)
        {
            cort_env_ = env;
        }

    protected:
        std::shared_ptr<CortTrigger> trigger_{new CortTrigger};
        SoceRedis::CortEnv cort_env_;
#endif

    protected:
        /*
         * Only connect
         */
        redisContext* connect(const std::string& ip, int port);

        /*
         * Connect & Auth & SelectDb
         */
        RedisCtx make_conn(const std::string& ip, int port);

    private:
        int auth(redisContext* ctx);
        int select_db(redisContext* ctx);

    protected:
        std::string passwd_;
        std::string addrs_;
        std::string master_name_;
        uint32_t timeout_ = 0;
        int db_ = -1;
        bool connected_ = false;
        std::list<RedisCmd> cmds_;
        std::unordered_map<int, bool> more_reply_;
    };

    class NoClusterClt : public BasicClt
    {
    public:
#if defined(SOCE_USE_COROUTINE)
        virtual int buffer_read(int fd);
        virtual bool has_reply(int fd);
        virtual int flush_cmds();
#endif
        virtual int group(std::list<RedisCmd>& cmds);
        virtual int exec(uint32_t cmd_size, std::list<RedisReply>& out);

    protected:
        BasicClt::RedisCtx ctx_;
    };

    class SingleClt : public NoClusterClt
    {
    public:
        virtual int init();
        virtual int reset();

    private:
        std::string ip_;
        int port_;
    };

    class SentinelClt : public NoClusterClt
    {
    public:
        virtual int init();
        virtual int reset();

    private:
        int get_master(const std::string& ip, int port);

    private:
        std::vector<std::pair<std::string, int> > sentinels_;
        std::string master_ip_;
        int master_port_;
    };

    class ClusterClt : public BasicClt
    {
    public:
        virtual int init();
#if defined(SOCE_USE_COROUTINE)
        virtual int buffer_read(int fd);
        virtual bool has_reply(int fd);
        virtual int flush_cmds();
#endif
        virtual int reset();
        virtual int group(std::list<RedisCmd>& cmds);
        virtual int exec(uint32_t cmd_size, std::list<RedisReply>& out);

    private:
        using Node = struct {
            BasicClt::RedisCtx ctx;
            long long beg = 0;
            long long end = 0;
        };

    private:
        int get_masters();
        redisContext* get_ctx_from_slot(uint slot);

    private:
        std::vector<std::pair<std::string, int> > masters_;
        std::vector<Node> ctxs_;
    };

    class SoceRedisReplyImpl
    {
    public:
        SoceRedisReplyImpl(std::list<RedisReply>::iterator iter,
                           std::list<RedisReply>::iterator end);
        SoceRedisReplyImpl(const SoceRedisReplyImpl& rhs);
        SoceRedisReplyImpl& operator=(const SoceRedisReplyImpl& rhs);
        SoceRedisReplyImpl& operator++();
        SoceRedisReplyImpl operator++(int);
        bool operator==(const SoceRedisReplyImpl& rhs);
        bool operator!=(const SoceRedisReplyImpl& rhs);

        int get_type(SoceRedisType& type);
        int get_str(std::string& val);
        int get_array_size(int& size);
        int get_array_begin(SoceRedisIterator& iter);
        int get_array_end(SoceRedisIterator& iter);
        int get_int(int& val);
        int get_status(std::string& val);
        int get_error(std::string& val);

    private:
        int get(std::function<int()> getter);

    private:
        std::list<RedisReply>::iterator iter_;
        std::list<RedisReply>::iterator end_;
    };

    class SoceRedisImpl
    {
    public:
        SoceRedisImpl() = default;
        ~SoceRedisImpl() = default;
        SoceRedisImpl(const SoceRedisImpl&) = delete;
        SoceRedisImpl& operator=(const SoceRedisImpl&) = delete;

#if defined(SOCE_USE_COROUTINE)
        int init(const std::string& conf, const SoceRedis::CortEnv& env);
        void keepalive();
#endif
        int init(const std::string& conf);
        int exec(SoceRedis::Fmt fmt);
        int append(SoceRedis::Fmt fmt);
        int append(std::list<RedisCmd>& cmd, SoceRedis::Fmt fmt);
        int do_cmd();
        int do_cmd(std::list<RedisCmd>& cmd);

        void clear();
        SoceRedis::iterator begin();
        SoceRedis::iterator end();

    private:
        int parse_conf(const std::string& conf_str,
                      std::unordered_map<std::string, std::string>& confs);
        int create_clt(std::unordered_map<std::string, std::string>& confs);
        int append_cmd(const std::string& key, const std::vector<std::string>& cmd);

    private:
        std::list<RedisCmd> pipeline_cmd_;
        std::list<RedisReply> replies_;
        std::shared_ptr<RedisCltIf> clt_;
        uint64_t cmd_index_ = 0;
        uint64_t cmd_confirmed_ = 0;
        int timeout_ = 0;

#if defined(SOCE_USE_COROUTINE)
        SoceRedis::CortEnv cort_env_;
#endif
    };

}} // end namespace
#endif
