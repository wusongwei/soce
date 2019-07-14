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

#ifndef _SOCE_REDIS_PSTCacu7xn_H_
#define _SOCE_REDIS_PSTCacu7xn_H_

#include <memory>
#include <type_traits>
#include <iterator>
#include <functional>
#include "param.hpp"

#if defined(SOCE_USE_COROUTINE)
#include "cort-engine/cort-engine.h"
#include "transport/transport-if.h"
#endif

#define soce_redis_docmd(clt, cmd)              \
    clt.exec([&](Param& pa){                    \
            pa << cmd;                          \
        })

#define soce_redis_append(clt, cmd)             \
    clt.append([&](Param& pa){                  \
            pa << cmd;                          \
        })

namespace soce{
namespace redis{

    class SoceRedisReply;
    class SoceRedisReplyImpl;
    class SoceRedisImpl;

    typedef enum{
        kSRTString = 1,
        kSRTArray,
        kSRTInteger,
        kSRTNil,
        kSRTStatus,
        kSRTError,
        kSRTTimeout
    }SoceRedisType;

    class SoceRedisIterator : public std::iterator<std::forward_iterator_tag, SoceRedisReply>
    {
    public:
        SoceRedisIterator() = default;
        SoceRedisIterator(std::shared_ptr<SoceRedisReply> reply);
        SoceRedisIterator(const SoceRedisIterator& rhs);
        SoceRedisIterator& operator=(const SoceRedisIterator& rhs);
        SoceRedisIterator& operator++();
        SoceRedisIterator operator++(int);
        bool operator==(const SoceRedisIterator& rhs);
        bool operator!=(const SoceRedisIterator& rhs);
        SoceRedisReply& operator*();
        SoceRedisReply* operator->();

    private:
        std::shared_ptr<SoceRedisReply> reply_;
    };

    class SoceRedisReply
    {
    public:
        SoceRedisReply(std::shared_ptr<SoceRedisReplyImpl> impl);
        SoceRedisReply(const SoceRedisReply& rhs);
        SoceRedisReply& operator=(const SoceRedisReply& rhs);
        SoceRedisReply& operator++();
        SoceRedisReply operator++(int);
        bool operator==(const SoceRedisReply& rhs);
        bool operator!=(const SoceRedisReply& rhs);

        int get_type(SoceRedisType& type);
        int get_str(std::string& val);
        int get_array_size(int& size);
        int get_array_begin(SoceRedisIterator& iter);
        int get_array_end(SoceRedisIterator& iter);
        int get_int(int& val);
        int get_status(std::string& val);
        int get_error(std::string& val);

    private:
        std::shared_ptr<SoceRedisReplyImpl> impl_;
    };

    class SoceRedis
    {
    public:
        using iterator = SoceRedisIterator;
        using Fmt = std::function<void(Param&)>;

        SoceRedis() = default;
        ~SoceRedis() = default;

#if defined(SOCE_USE_COROUTINE)
        using CortEnv = struct{
            std::function<void(int, std::function<void(int)>)> watch;
            std::function<void(int)> unwatch;
        };
        /*
         * Init client using coroutine.
         *
         * @param conf Support three modes. timeout is optional, not timeout by default.
         *             Signle mode : passwd@ip:port?timeout=1000
         *             Sentinel mode : passwd@ip:port,ip:port?master=[master-name]&timeout=1000
         *             Cluster mode : passwd@ip:port,ip:port?cluster=true&timeout=1000
         *             passwd should be "none" if server does not apply password, like:
         *             none@127.0.0.1:6379?timeout=1000
         * @param env Helper functions.
         *
         * @return 0 success, -1 falied.
         */
        int init(const std::string& conf, const CortEnv& env);

        /*
         * Call it repeatly to check the state of connection.
         */
        void keepalive();
#else
        /*
         * Init client.
         *
         * @param conf Support three modes. timeout is optional, not timeout by default.
         *             Signle mode : passwd@ip:port?timeout=1000
         *             Sentinel mode : passwd@ip:port,ip:port?master=[master-name]&timeout=1000
         *             Cluster mode : passwd@ip:port,ip:port?cluster=true&timeout=1000
         *             passwd should be "none" if server does not apply password, like:
         *             none@127.0.0.1:6379?timeout=1000
         *
         * @return 0 success, -1 falied.
         */

        int init(const std::string& conf);
#endif

        /*
         * Execute a single command.
         *
         * @param fmt The parameters.
         *
         * @return 0 success, -1 failed.
         */
        int exec(Fmt fmt);

        /*
         * Execute multiple commands in pipeline mode.
         *
         * Step 1 : call clear() to clear commands in command buffer.
         * Step 2 : call append() repeatly to add more than one commands.
         * Step 3 : call pipeline() to execute the given commands.
         */
        void clear();
        int append(Fmt fmt);
        int pipeline();

        /*
         * Get response for both mode.
         * Only use begin() to get the response if there is only one command.
         */
        iterator begin();
        iterator end();

    private:
        std::shared_ptr<SoceRedisImpl> impl_;
    };

}} // end namespace

#endif
