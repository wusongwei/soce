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

#ifndef _SOCE_TRANSPORT_IF_vMZr9yKqID_H_
#define _SOCE_TRANSPORT_IF_vMZr9yKqID_H_

#include <string>
#include <functional>
#include <memory>
#include <stdint.h>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/**
 * 当打开此宏时，需要同时修改CMakeLists.txt，添加websockets库
 * 后续通过CMAKE的option来控制。
 */
// #define WITH_LIBWEBSOCKETS

#if defined WITH_LIBWEBSOCKETS
#include <libwebsockets.h>
#endif

#include "utils/nocopyable.hpp"
#include "processor-if.h"

namespace soce{
namespace transport{

    class TransportIf : public soce::utils::NoCopyable
    {
    public:
        using SignalHandler = std::function<void(int)>;
        using tmid_t = uint32_t;
        using TimerCallback = std::function<void()>;
        using TimerMode = enum {
            kTimerModeOnceAndDel,
            kTimerModeOnceAndRetain,
            kTimerModePresist
        };
        using SSLCert = struct {
            std::string ca_file;
            std::string cert_file;
            std::string key_file;
        };

    public:
        /*
         * Listen on an address, and the data received will be processed by the processor.
         * The format of addr like IP:Port.
         * If the port is zero, it will use a random port.
         * The true address can be resolved from the sock if it is not null.
         * It is usefull when we want to register the address to zookeeper, etcd and etc.
         *
         * @param addr The address
         * @param processor The data processor
         * @param sock The socket of the server
         *
         * @return 0 success, -1 failed
         */
        virtual int listen(const std::string& addr,
                           std::shared_ptr<ProcessorIf> processor,
                           int* sock = nullptr) = 0;

        /*
         * TLS
         */
        virtual int listen(const SSLCert& cert,
                           const std::string& addr,
                           std::shared_ptr<ProcessorIf> processor,
                           int* sock = nullptr) = 0;

#if defined WITH_LIBWEBSOCKETS
        /*
         * Websocket
         */
        virtual int listen(const std::string& addr,
                           struct lws_protocols* protocols,
                           struct lws_extension* exts,
                           int* sock = nullptr) = 0;

        /* virtual int listen(const SSLCert& cert, */
        /*                    const std::string& addr, */
        /*                    struct lws_protocols* protocols, */
        /*                    struct lws_extension* exts, */
        /*                    int* sock = nullptr) = 0; */
#endif

        /*
         * Conenct to a server.
         * The connection is not ready
         * before `processor->on_connected()` has been called.
         *
         * @param addr The address
         * @param processor The data processor
         * @param cli_id The client id
         *
         * @return 0 success, -1 failed
         */
        virtual int connect(const std::string& addr,
                            std::shared_ptr<ProcessorIf> processor,
                            uint64_t& cli_id) = 0;

        virtual int connect(const SSLCert& cert,
                            const std::string& addr,
                            std::shared_ptr<ProcessorIf> processor,
                            uint64_t& cli_id) = 0;
        virtual bool is_ready(uint64_t cli_id) = 0;

        /*
         * Watch a existed fd, call the WatcherCallback when data coming.
         *
         * @param fd The fd watched, it will be passed to 'processor->process()'
         * @param processor The processor for the given fd
         *
         * @return 0 success, -1 failed
         */
        virtual int watch(int fd, std::shared_ptr<ProcessorIf> processor) = 0;

        /*
         * Unwatch a fd.
         *
         * @param fd The fd unwatched.
         *
         * @return 0 success, -1 failed
         */
        virtual int unwatch(int fd) = 0;

        /*
         * Send data.
         *
         * @param conn_id The id of the connection
         * @param data The data.
         * @param len  The length of the data.
         *
         * @return 0 success, -1 failed
         */
        virtual int send(uint64_t conn_id, const char* data, size_t len) = 0;

        /*
         * Set the handler for the signal.
         *
         * @param signum The number of the signal
         * @param handler The handler for the given signal
         *
         * @return 0 success, -1 failed
         */
        virtual int signal(int signum, SignalHandler handler) = 0;

        /*
         * Close a connection.
         *
         * @param conn_id The id of the connection.
         *
         * @return 0 success, -1 connection not found.
         */
        virtual int close(uint64_t conn_id) = 0;

        /*
         * Disable/Resume read for a connection.
         *
         * @param conn_id The id of the connection.
         *
         * @return 0 success, -1 connection not found.
         */
        virtual int enable_read(uint64_t conn_id) = 0;
        virtual int disable_read(uint64_t conn_id) = 0;

        /*
         * Set timeout for a connection
         *
         * @param conn_id The id of the connection
         * @param timeout_ms The time in millsecond
         *
         * @return 0 success, -1 connection not found
         */
        virtual int set_timeout(uint64_t conn_id, uint32_t timeout_ms) = 0;

        /*
         * Add a timer, timer mode can be:
         *
         * @param gap_ms The gap in millsec.
         * @param mode Time mode
         *             kTimerModeOnceAndDel : The timer will be deleted after triggered.
         *             kTimerModeOnceAndRetain : The timer will not be deleted after triggered.
         *                                       You can restart it by 'restart_timer()'.
         *             kTimerModePresist : Timer timer will be triggered periodicity。
         * @parma tc Timer callback
         *
         * @return tmid_t The timer id
         */
        virtual tmid_t add_timer(uint32_t gap_ms,
                                 TimerMode mode,
                                 TimerCallback tc) = 0;

        /*
         * Restart a timer which is created with mode 'kTimerModeOnceAndRetain'
         * with a new gap.
         *
         * @param tid Timer id
         * @param gap The gap in millsec
         *
         * @return 0 success, -1 timer not found or can not be restarted.
         */
        virtual int restart_timer(tmid_t tid, uint32_t gap) = 0;

        /*
         * Delete a timer which is created with mode 'kTimerModeOnceAndRetain'
         *
         * @param tid Timer id
         * @param gap The gap in millsec
         *
         * @return 0 success, -1 timer not found
         */
        virtual int del_timer(tmid_t tid) = 0;

        /*
         * Loop forever
         */
        virtual void run() = 0;

        /*
         * Stop it from another thread
         */
        virtual void stop() = 0;
    };

} // namespace transport
} // namespace soce
#endif
