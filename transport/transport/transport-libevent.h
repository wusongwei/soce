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

#ifndef _SOCE_TRANSPORT_LIBEVENT_pYZcRtJPmr_H_
#define _SOCE_TRANSPORT_LIBEVENT_pYZcRtJPmr_H_

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "transport-if.h"
namespace soce{
namespace transport{

    class TransportLibevent : public TransportIf
    {
    public:
        class LeConn;

        using OnTimerArgs = struct{
            TransportLibevent* tle;
            tmid_t id;
        };

    public:
        int init(struct event_base* base = nullptr);
        virtual int listen(const std::string& addr,
                           std::shared_ptr<ProcessorIf> processor,
                           int* sock = nullptr);
        virtual int listen(const SSLCert& cert,
                           const std::string& addr,
                           std::shared_ptr<ProcessorIf> processor,
                           int* sock = nullptr);
#if defined WITH_LIBWEBSOCKETS
        virtual int listen(const std::string& addr,
                           struct lws_protocols* protocols,
                           struct lws_extension* exts,
                           int* sock = nullptr);
        virtual int listen(const SSLCert& cert,
                           const std::string& addr,
                           struct lws_protocols* protocols,
                           struct lws_extension* exts,
                           int* sock = nullptr);
#endif
        virtual int connect(const std::string& addr,
                            std::shared_ptr<ProcessorIf> processor,
                            uint64_t& cli_id);
        virtual int connect(const SSLCert& cert,
                            const std::string& addr,
                            std::shared_ptr<ProcessorIf> processor,
                            uint64_t& cli_id);
        virtual bool is_ready(uint64_t conn_id);
        virtual int watch(int fd, std::shared_ptr<ProcessorIf> processor);
        virtual int unwatch(int fd);
        virtual int send(uint64_t conn_id, const char* data, size_t len);
        virtual int signal(int signum, SignalHandler handler);
        virtual int close(uint64_t conn_id);
        virtual int enable_read(uint64_t conn_id);
        virtual int disable_read(uint64_t conn_id);
        virtual int set_timeout(uint64_t conn_id, uint32_t timeout_ms);
        virtual tmid_t add_timer(uint32_t gap_ms,
                                 TimerMode mode,
                                 TimerCallback tc);
        virtual int restart_timer(tmid_t tid, uint32_t gap);
        virtual int del_timer(tmid_t tid);
        virtual void run();
        virtual void stop();

        void do_accept(struct evconnlistener *listener,
                       evutil_socket_t fd,
                       struct sockaddr *sa,
                       int socklen);
        void do_signal(evutil_socket_t sig, short events);
        void timer_entry(TransportIf::tmid_t timer_id);

        inline struct event_base* get_event_base() {return base_.get();}

    private:
        using Timer = struct {
            TransportIf::TimerCallback tc;
            TransportIf::TimerMode mode;
            std::shared_ptr<struct event> evt;
            std::shared_ptr<OnTimerArgs> on_timer_args;
        };

        using Signal = struct{
            std::shared_ptr<struct event> event;
            SignalHandler handler;
        };

        std::shared_ptr<struct event_base> base_;
        std::unordered_map<int, Signal> signals_;
        std::unordered_map<int, std::shared_ptr<SSL_CTX>> ssl_ctx_;

        std::unordered_map<evconnlistener*, std::shared_ptr<ProcessorIf>> listeners_;
        std::vector<std::shared_ptr<struct evconnlistener>> listener_resobj_;

        std::unordered_map<uint64_t, std::shared_ptr<LeConn>> conns_;
        std::unordered_map<tmid_t, Timer> timers_;
        tmid_t timer_id_ = 0;
        bool own_base_ = false;

        // websocket
        std::shared_ptr<struct lws_context> lws_ctx_;
    };

    class TransportLibevent::LeConn
    {
    public:
        LeConn(TransportLibevent* tl,
               int fd,
               std::shared_ptr<ProcessorIf> processor);
        LeConn(TransportLibevent* tl,
               struct bufferevent* bev,
               std::shared_ptr<ProcessorIf> processor,
               bool ready);
        ~LeConn();

        inline void set_ssl_ctx(std::shared_ptr<SSL_CTX> ctx) {ssl_ctx_ = ctx;}
        inline void set_event(std::shared_ptr<struct event> evt) {evt_ = evt;}

        bool is_ready();
        void do_read();
        void do_event(short events);
        int send(const char* data, size_t len);
        int enable_read();
        int disable_read();
        int set_timeout(uint32_t timeout_ms);
        void close();

        inline uint64_t get_conn_id() {return conn_id_;};

    private:
        using Status = enum{
            kStatusEnable,
            kStatusDisable,
            kStatusClosed
        };

    private:
        uint64_t conn_id_ = 0;
        bool is_ready_ = true;
        TransportLibevent* tl_;
        std::shared_ptr<struct bufferevent> bev_;
        std::shared_ptr<struct event> evt_;
        std::shared_ptr<ProcessorIf> processor_;
        std::shared_ptr<SSL_CTX> ssl_ctx_;

        size_t buffer_size_ = 0;
        std::shared_ptr<char> buffer_;
        Status status_ = kStatusEnable;
    };

} // namespace transport
} // namespace soce
#endif
