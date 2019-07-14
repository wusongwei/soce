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

#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <cassert>
#include <event2/bufferevent_ssl.h>
#include "transport/log.h"
#include "transport/transport-libevent.h"
#include "utils/str-helper.h"

using std::string;
using namespace soce::utils;

namespace soce{
namespace transport{

    static void on_accept(struct evconnlistener *,
                         evutil_socket_t,
                         struct sockaddr *,
                         int socklen,
                         void *);
    static void on_read(struct bufferevent *, void *);
    static void on_watch(int, short, void*);
    static void on_event(struct bufferevent *, short, void *);
    static void on_signal(evutil_socket_t, short, void *);
    static void on_timer(int fd,short event,void *args);

    int TransportLibevent::init(struct event_base* base)
    {
        TRANSPORT_DEBUG << _S("Thread id", std::this_thread::get_id());

        if (base){
            base_.reset(base, [](struct event_base*){});
        }
        else{
            base = event_base_new();
            if (!base){
                SOCE_ERROR << _S("Error", "Failed to create event_base");
                return -1;
            }
            base_.reset(base, [](struct event_base* base){
                    event_base_free(base);
                });
        }

        return 0;
    }

    int TransportLibevent::listen(const std::string& addr,
                                  std::shared_ptr<ProcessorIf> processor,
                                  int* sock)
    {
        string ip;
        string port;
        if (StrHelper::parse_addr(addr, ip, port)){
            SOCE_ERROR << _S("Invalid addr", addr);
            return -1;
        }

        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(ip.c_str());
        sin.sin_port = htons(atoi(port.c_str()));

        struct evconnlistener* listener = evconnlistener_new_bind(base_.get(),
                                                                  on_accept,
                                                                  (void *)this,
                                                                  LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
                                                                  -1,
                                                                  (struct sockaddr*)&sin,
                                                                  sizeof(sin));
        if (!listener) {
            SOCE_ERROR << _S("Failed to create listener for addr", addr);
            return -1;
        }

        std::shared_ptr<struct evconnlistener> evls(listener, [](struct evconnlistener* listener){
                evconnlistener_free(listener);
            });
        listener_resobj_.push_back(evls);

        if (sock){
            *sock = (int)evconnlistener_get_fd(listener);
        }
        listeners_[listener] = processor;

        TRANSPORT_DEBUG << _S("Listen on", addr);
        return 0;
    }

    int TransportLibevent::listen(const SSLCert& cert,
                                  const std::string& addr,
                                  std::shared_ptr<ProcessorIf> processor,
                                  int* sock)
    {
        SSL_CTX* ssl_ctx = nullptr;
        std::shared_ptr<SSL_CTX> ssl_ctx_ptr;

        SSL_library_init();
        ERR_load_crypto_strings();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();

        ssl_ctx = SSL_CTX_new(SSLv23_method());
        if (cert.ca_file.empty()){
            SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, 0);
        }
        else{
            SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, 0);
        }

        ssl_ctx_ptr.reset(ssl_ctx, [](SSL_CTX* ctx){
                SSL_CTX_free(ctx);
            });

        if (!cert.ca_file.empty()){
            if (SSL_CTX_load_verify_locations(ssl_ctx, cert.ca_file.c_str(), 0) != 1) {
                SOCE_ERROR << _S("load ca_file", cert.ca_file);
                return -1;
            }
        }

        if (SSL_CTX_use_certificate_file(ssl_ctx, cert.cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            SOCE_ERROR << _S("load cert_file", cert.cert_file);
            return -1;
        }

        if (SSL_CTX_use_PrivateKey_file(ssl_ctx, cert.key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            SOCE_ERROR << _S("load key_file", cert.key_file);
            return -1;
        }

        if (!SSL_CTX_check_private_key(ssl_ctx)) {
            SOCE_ERROR << _D("check private key");
            return -1;
        }

        SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY);

        int fd = 0;
        if (listen(addr, processor, &fd)){
            return -1;
        }

        if (sock){
            *sock = fd;
        }
        ssl_ctx_[fd] = ssl_ctx_ptr;

        TRANSPORT_DEBUG << _S("listen on", fd)
                        << _S("server.ca", cert.ca_file)
                        << _S("server.cert_file", cert.cert_file)
                        << _S("server.key_file", cert.key_file);
        return 0;
    }

#if defined WITH_LIBWEBSOCKETS
    int TransportLibevent::listen(const std::string& addr,
                                  struct lws_protocols* protocols,
                                  struct lws_extension* exts,
                                  int* sock)
    {
        SSLCert cert;
        return listen(cert, addr, protocols, exts, sock);
    }

    int TransportLibevent::listen(const SSLCert& cert,
                                  const std::string& addr,
                                  struct lws_protocols* protocols,
                                  struct lws_extension* exts,
                                  int* sock)
    {
        // I don't know how to get the socket right now.
        (void) sock;

        string host;
        string port;

        if (StrHelper::parse_addr(addr, host, port)
            || host.empty() || port.empty()){
            SOCE_ERROR << _S("Invalid addr", addr);
            return -1;
        }

        struct lws_context_creation_info info;
        memset(&info, 0, sizeof(info));

        info.options |= LWS_SERVER_OPTION_LIBEVENT;
        info.iface = host.c_str();
        info.port = atoi(port.c_str());
        info.protocols = protocols;
        info.extensions = exts;
        info.gid = -1;
        info.uid = -1;
        info.max_http_header_pool = 1;

        if (cert.ca_file.empty() || cert.cert_file.empty() || cert.key_file.empty()){
            info.ssl_ca_filepath = NULL;
            info.ssl_cert_filepath = NULL;
            info.ssl_private_key_filepath = NULL;
        }
        else{
            info.ssl_ca_filepath = cert.ca_file.c_str();
            info.ssl_cert_filepath = cert.cert_file.c_str();
            info.ssl_private_key_filepath = cert.key_file.c_str();
            info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        }

        struct lws_context* lws_ctx = lws_create_context(&info);
        if (!lws_ctx){
            SOCE_ERROR << _D("Failed to create lws_context");
            return -1;
        }
        lws_ctx_.reset(lws_ctx, [](struct lws_context* ctx){
                lws_context_destroy(ctx);
            });
        lws_event_sigint_cfg(lws_ctx, 0, NULL);
        lws_event_initloop(lws_ctx, base_.get(), 0);

        return 0;
    }
#endif

    int TransportLibevent::connect(const std::string& addr,
                                   std::shared_ptr<ProcessorIf> processor,
                                   uint64_t& conn_id)
    {
        string ip;
        string port;
        if (StrHelper::parse_addr(addr, ip, port)) {
            SOCE_ERROR << _S("Connect invalid addr", addr);
            return -1;
        }

        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr=inet_addr(ip.c_str());
        sin.sin_port = htons(atoi(port.c_str()));

        struct bufferevent* bev = bufferevent_socket_new(base_.get(),
                                                         -1,
                                                         BEV_OPT_CLOSE_ON_FREE);

        if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0){
            SOCE_ERROR << _S("Connect failed", addr);
            return -1;
        }

        std::shared_ptr<LeConn> conn = std::make_shared<LeConn>(this, bev, processor, false);
        conn_id = conn->get_conn_id();
        conns_[conn_id] = conn;

        TRANSPORT_DEBUG << _S("Connect addr ", addr) << _S("conn_id", conn_id);
        return 0;
    }

    int TransportLibevent::connect(const SSLCert& cert,
                                   const std::string& addr,
                                   std::shared_ptr<ProcessorIf> processor,
                                   uint64_t& conn_id)
    {
        string ip;
        string port;
        if (StrHelper::parse_addr(addr, ip, port)) {
            SOCE_ERROR << _S("Connect invalid addr", addr);
            return -1;
        }

        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr=inet_addr(ip.c_str());
        sin.sin_port = htons(atoi(port.c_str()));

        SSL_CTX* ssl_ctx = nullptr;
        std::shared_ptr<SSL_CTX> ssl_ctx_ptr;

        SSL_library_init();
        ERR_load_crypto_strings();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();

        ssl_ctx = SSL_CTX_new(SSLv23_method());
        if (cert.ca_file.empty()){
            SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, 0);
        }
        else{
            SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, 0);
        }

        ssl_ctx_ptr.reset(ssl_ctx, [](SSL_CTX* ctx){
                SSL_CTX_free(ctx);
            });

        if (!cert.ca_file.empty()){
            if (SSL_CTX_load_verify_locations(ssl_ctx, cert.ca_file.c_str(), 0) != 1) {
                SOCE_ERROR << _S("load ca_file", cert.ca_file);
                return -1;
            }
        }

        if (!cert.cert_file.empty() && !cert.key_file.empty()){
            if (SSL_CTX_use_certificate_file(ssl_ctx, cert.cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
                SOCE_ERROR << _S("load cert_file", cert.cert_file);
                return -1;
            }

            if (SSL_CTX_use_PrivateKey_file(ssl_ctx, cert.key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
                SOCE_ERROR << _S("load key_file", cert.key_file);
                return -1;
            }

            if (!SSL_CTX_check_private_key(ssl_ctx)) {
                SOCE_ERROR << _D("check private key");
                return -1;
            }
        }

        SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY);

        SSL *ssl = SSL_new(ssl_ctx);
        struct bufferevent* bev = bufferevent_openssl_socket_new(base_.get(),
                                                                 -1,
                                                                 ssl,
                                                                 BUFFEREVENT_SSL_CONNECTING,
                                                                 BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

        if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0){
            SOCE_ERROR << _S("Connect failed", addr);
            return -1;
        }

        std::shared_ptr<LeConn> conn = std::make_shared<LeConn>(this, bev, processor, false);
        conn->set_ssl_ctx(ssl_ctx_ptr);
        conn_id = conn->get_conn_id();
        conns_[conn_id] = conn;

        TRANSPORT_DEBUG << _S("Connect addr", addr) << _S("conn_id", conn_id);
        return 0;
    }

    bool TransportLibevent::is_ready(uint64_t conn_id)
    {
        auto iter = conns_.find(conn_id);
        return (iter == conns_.end()) ? false : iter->second->is_ready();
    }

    int TransportLibevent::watch(int fd, std::shared_ptr<ProcessorIf> processor)
    {
        if (processor->get_max_header_size() == 0){
            std::shared_ptr<LeConn> conn = std::make_shared<LeConn>(this, fd, processor);
            conns_[fd] = conn;
            struct event *evt = event_new(base_.get(), fd, EV_READ | EV_PERSIST, on_watch, conn.get());
            conn->set_event(std::shared_ptr<struct event>(evt, [](struct event* e){
                        event_free(e);
                    }));
            event_add(evt, NULL);
        }
        else{
            // don't close the connection when unwatch
            struct bufferevent *bev = NULL;
            bev = bufferevent_socket_new(base_.get(), fd, 0);
            if (!bev) {
                return -1;
            }

            std::shared_ptr<LeConn> conn = std::make_shared<LeConn>(this, bev, processor, true);
            conns_[fd] = conn;
        }
        TRANSPORT_DEBUG << _S("watch ", fd);
        return 0;
    }

    int TransportLibevent::unwatch(int fd)
    {
        TRANSPORT_DEBUG << _S("unwatch ", fd);
        return close(fd);
    }

    int TransportLibevent::send(uint64_t conn_id, const char* data, size_t len)
    {
        auto iter = conns_.find(conn_id);
        if (iter == conns_.end()){
            TRANSPORT_DEBUG << _S("Send invalid conn_id", conn_id);
            return -1;
        }
        return iter->second->send(data, len);
    }

    int TransportLibevent::signal(int signum, SignalHandler handler)
    {
        struct event* signal_event = evsignal_new(base_.get(), signum, on_signal, (void *)this);

        if (!signal_event || event_add(signal_event, NULL) < 0) {
            SOCE_ERROR << _S("SignalFailed", signum);
            return -1;
        }
        Signal sig;
        sig.event.reset(signal_event, [](struct event* evt){
                event_free(evt);
            });
        sig.handler = handler;
        signals_[signum] = sig;
        return 0;
    }

    int TransportLibevent::close(uint64_t conn_id)
    {
        auto iter = conns_.find(conn_id);
        if (iter == conns_.end()) {
            TRANSPORT_DEBUG << _S("Close invalid conn", conn_id);
            return -1;
        }

        TRANSPORT_DEBUG << _S("close", conn_id);
        conns_.erase(conn_id);

        return 0;
    }

    int TransportLibevent::enable_read(uint64_t conn_id)
    {
        auto iter = conns_.find(conn_id);
        if (iter == conns_.end()) {
            TRANSPORT_DEBUG << _S("enable_read invalid conn_id", conn_id);
            return -1;
        }

        TRANSPORT_DEBUG << _S("enable_read", conn_id);
        return iter->second->enable_read();
    }

    int TransportLibevent::disable_read(uint64_t conn_id)
    {
        auto iter = conns_.find(conn_id);
        if (iter == conns_.end()) {
            TRANSPORT_DEBUG << _S("disable_read invalid conn_id", conn_id);
            return -1;
        }

        TRANSPORT_DEBUG << _S("disable_read", conn_id);
        return iter->second->disable_read();
    }

    int TransportLibevent::set_timeout(uint64_t conn_id, uint32_t timeout_ms)
    {
        auto iter = conns_.find(conn_id);
        if (iter == conns_.end()) {
            SOCE_INFO << _S("set_timeout invalid conn_id", conn_id);
            return -1;
        }

        TRANSPORT_DEBUG << _S("set_timeout", conn_id);
        return iter->second->set_timeout(timeout_ms);
    }

    TransportIf::tmid_t
    TransportLibevent::add_timer(uint32_t gap,
                                 TimerMode mode,
                                 TransportIf::TimerCallback tc)

    {
        ++timer_id_;
        Timer& timer = timers_[timer_id_];
        timer.on_timer_args.reset(new OnTimerArgs);
        timer.on_timer_args->tle = this;
        timer.on_timer_args->id = timer_id_;

        int flag = (mode == kTimerModePresist) ? EV_PERSIST : 0;
        struct event* evt = event_new(base_.get(), -1, flag, on_timer, (void*)timer.on_timer_args.get());

        timer.tc = tc;
        timer.mode = mode;
        timer.evt.reset(evt, [](struct event* evt){
                event_free(evt);
            });

        struct timeval tv;
        tv.tv_sec = gap / 1000;
        tv.tv_usec = (gap % 1000) * 1000;
        evtimer_add(evt, &tv);

        TRANSPORT_DEBUG << _S("Add timer_id", timer_id_) << _S("mode", mode) << _S("gap", gap);
        return timer_id_;
    }

    int TransportLibevent::restart_timer(TransportIf::tmid_t timer_id, uint32_t gap)
    {
        auto iter = timers_.find(timer_id);
        if (iter == timers_.end() || iter->second.mode != kTimerModeOnceAndRetain) {
            SOCE_INFO << _S("Failed to restart timer_id", timer_id);
            return -1;
        }

        struct timeval tv;
        tv.tv_sec = gap / 1000;
        tv.tv_usec = (gap % 1000) * 1000;

        evtimer_del(iter->second.evt.get());
        evtimer_add(iter->second.evt.get(), &tv);

        TRANSPORT_DEBUG << _S("Restart timer_id", timer_id) << _S("gap", gap);
        return 0;
    }

    int TransportLibevent::del_timer(TransportIf::tmid_t timer_id)
    {
        auto iter = timers_.find(timer_id);
        if (iter == timers_.end()){
            return -1;
        }

        evtimer_del(iter->second.evt.get());
        timers_.erase(timer_id);
        TRANSPORT_DEBUG << _S("DelTimer timer_id", timer_id);
        return 0;
    }

    void TransportLibevent::run()
    {
        event_base_dispatch(base_.get());
    }

    void TransportLibevent::stop()
    {
        event_base_loopexit(base_.get(), NULL);
        listener_resobj_.clear();
        conns_.clear();
    }

    void TransportLibevent::timer_entry(TransportIf::tmid_t id)
    {
        auto iter = timers_.find(id);
        if (iter != timers_.end()) {
            iter->second.tc();
            if (iter->second.mode == kTimerModeOnceAndDel){
                timers_.erase(iter);
            }
        }
    }

    void TransportLibevent::do_accept(struct evconnlistener *listener,
                                      evutil_socket_t fd,
                                      struct sockaddr *sa,
                                      int socklen)
    {
        (void) sa;
        (void) socklen;

        auto iter = listeners_.find(listener);
        if (iter == listeners_.end()){
            return;
        }

        struct bufferevent *bev = NULL;
        bev = bufferevent_socket_new(base_.get(), fd, BEV_OPT_CLOSE_ON_FREE);
        if (!bev) {
            return;
        }

        evutil_socket_t server_fd = evconnlistener_get_fd(listener);
        auto iter_ctx = ssl_ctx_.find((int)server_fd);
        if (iter_ctx != ssl_ctx_.end()){
            struct bufferevent* b_ssl = nullptr;
            SSL* ssl = SSL_new(iter_ctx->second.get());
            b_ssl = bufferevent_openssl_filter_new(base_.get(),
                                                   bev,
                                                   ssl,
                                                   BUFFEREVENT_SSL_ACCEPTING,
                                                   BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
            if (!b_ssl){
                SSL_free(ssl);
                bufferevent_free(bev);
                return;
            }
            bev = b_ssl;
        }

        std::shared_ptr<LeConn> conn = std::make_shared<LeConn>(this, bev, iter->second, true);
        uint64_t conn_id = conn->get_conn_id();
        conns_[conn_id] = conn;
    }

    void TransportLibevent::do_signal(evutil_socket_t sig, short events)
    {
        (void) events;

        auto iter = signals_.find((int)sig);
        if (iter == signals_.end()){
            return;
        }

        iter->second.handler((int)sig);
    }

    TransportLibevent::LeConn::LeConn(TransportLibevent* tl,
                                      int fd,
                                      std::shared_ptr<ProcessorIf> processor)
        : conn_id_(fd), tl_(tl), processor_(processor)
    {
        processor->on_accept(fd);
    }

    TransportLibevent::LeConn::LeConn(TransportLibevent* tl,
                                      struct bufferevent* bev,
                                      std::shared_ptr<ProcessorIf> processor,
                                      bool ready)
        : is_ready_(ready),
          tl_(tl),
          processor_(processor),
          buffer_size_(processor->get_max_header_size()),
          buffer_(new char[buffer_size_])
    {
        int fd = bufferevent_getfd(bev);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64_t t = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        conn_id_ = ((uint64_t)fd << 32) | (t & 0xFFFFFFFF);

        bufferevent_setcb(bev, on_read, NULL, on_event, this);
        bufferevent_enable(bev, EV_READ | EV_PERSIST);
        bev_ = std::shared_ptr<struct bufferevent>(bev, [](struct bufferevent* event){
                bufferevent_free(event);
            });
        processor->on_accept(conn_id_);
    }

    TransportLibevent::LeConn::~LeConn()
    {
    }

    void TransportLibevent::LeConn::do_read()
    {
        processor_->on_read(conn_id_);

        if (buffer_size_ == 0){
            processor_->process(conn_id_, std::string());
            return;
        }

        struct evbuffer *input = bufferevent_get_input(bev_.get());
        size_t buffer_len = evbuffer_get_length(input);
        size_t pkt_size = 0;

        while (true){
            int header_size = evbuffer_copyout(input, buffer_.get(), buffer_size_);
            if (header_size <= 0){
                break;
            }

            pkt_size = processor_->get_full_pkt_size(buffer_.get(), header_size);
            if (pkt_size == IMCOMPLETED_PACKET_SIZE || pkt_size > buffer_len){
                break;
            }

            string pkt;
            pkt.resize(pkt_size);
            evbuffer_remove(input, const_cast<char*>(pkt.c_str()), pkt_size);
            buffer_len -= pkt_size;
            processor_->process(conn_id_, std::move(pkt));

            if (buffer_len == 0){
                break;
            }
        }

        if (!processor_->auto_read()){
            disable_read();
        }
    }

    bool TransportLibevent::LeConn::is_ready()
    {
        return is_ready_;
    }

    void TransportLibevent::LeConn::do_event(short events)
    {
        TRANSPORT_DEBUG <<  _S("event", events);

        if (events & BEV_EVENT_CONNECTED){
            is_ready_ = true;
            processor_->on_connected(conn_id_);
        }
        else if ((events & BEV_EVENT_EOF) || (events & BEV_EVENT_ERROR)){
            processor_->on_error(conn_id_);
            close();
        }
    }

    int TransportLibevent::LeConn::send(const char* data, size_t len)
    {
        return bufferevent_write(bev_.get(), data, len);
    }

    int TransportLibevent::LeConn::enable_read()
    {
        int rc = 0;
        if (buffer_size_ == 0){
            rc = (status_ == kStatusEnable) ? 0 : event_add(evt_.get(), NULL);
        }else{
            rc = (status_ == kStatusEnable) ? 0 : bufferevent_enable(bev_.get(), EV_READ);
        }

        if (rc == 0){
            status_ = kStatusEnable;
        }

        return rc;
    }

    int TransportLibevent::LeConn::disable_read()
    {
        int rc = 0;

        if (buffer_size_ == 0){
            rc = (status_ == kStatusDisable) ? 0 : event_del(evt_.get());
        }else{
            rc = (status_ == kStatusDisable) ? 0 : bufferevent_disable(bev_.get(), EV_READ);
        }

        if (!rc){
            status_ = kStatusDisable;
        }

        return rc;
    }

    int TransportLibevent::LeConn::set_timeout(uint32_t timeout_ms)
    {
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        return bufferevent_set_timeouts(bev_.get(), &tv, NULL);
    }

    void TransportLibevent::LeConn::close()
    {
        status_ = kStatusClosed;
        tl_->close(conn_id_);
    }

    static void on_accept(struct evconnlistener *listener,
                         evutil_socket_t fd,
                         struct sockaddr *sa,
                         int socklen,
                         void *user_data)
    {
        TransportLibevent* tle = (TransportLibevent*)user_data;
        tle->do_accept(listener, fd, sa, socklen);
    }

    static void on_read(struct bufferevent *bev, void *user_data)
    {
        (void) bev;

        TransportLibevent::LeConn* conn = (TransportLibevent::LeConn*)user_data;
        conn->do_read();
    }

    static void on_watch(int fd, short events, void* user_data)
    {
        (void) fd;
        if (events == EV_READ){
            TransportLibevent::LeConn* conn = (TransportLibevent::LeConn*)user_data;
            conn->do_read();
        }
    }

    static void on_event(struct bufferevent *bev, short events, void *user_data)
    {
        (void) bev;

        TransportLibevent::LeConn* conn = (TransportLibevent::LeConn*)user_data;
        conn->do_event(events);
    }

    static void on_signal(evutil_socket_t sig, short events, void *user_data)
    {
        TransportLibevent* tle = (TransportLibevent*)user_data;
        tle->do_signal(sig, events);
    }

    static void on_timer(int fd,short event,void *args)
    {
        TransportLibevent::OnTimerArgs* ota = (TransportLibevent::OnTimerArgs*)args;
        ota->tle->timer_entry(ota->id);
    }
}} // end namespace
