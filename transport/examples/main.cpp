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

#include <thread>
#include "transport/transport-libevent.h"
#include "transport/processor-if.h"
#include "log4rel/logger.h"

using namespace soce::transport;
using namespace soce::log4rel;
using std::string;

class BaseProcessor : public ProcessorIf
{
public:
    BaseProcessor()
        {
        }

    virtual size_t get_max_header_size()
        {
            return 4;
        }

    virtual size_t get_full_pkt_size(const char* data, size_t len)
        {
            if (len != 4){
                return IMCOMPLETED_PACKET_SIZE;
            }

            return 4 + *(const int*)(data);
        }

    virtual void on_error(uint64_t conn_id)
        {
            SOCE_INFO << _S("on_error", conn_id);
        }
};

class ServerProcessor : public BaseProcessor
{
public:
    virtual void process(uint64_t conn_id, std::string&& data)
        {
            SOCE_INFO << _S("Server", "process") << _S("conn_id", conn_id) << _S("data", data);
            transport_->send(conn_id, data.c_str(), data.size());
        }
};

class ClientProcessor : public BaseProcessor
{
public:
    virtual void process(uint64_t conn_id, std::string&& data)
        {
            SOCE_INFO << _S("Client", "process") << _S("conn_id", conn_id) << _S("data", data);
        }
};

typedef struct
{
    TransportIf* transport;
    uint64_t conn_non_tls;
    uint64_t conn_tls;
}TimerSampleArg;

static void timer_entry(std::shared_ptr<TimerSampleArg> args)
{
    TimerSampleArg* tsa = (TimerSampleArg*)args.get();
    SOCE_INFO << _D("in timer")
              << _S("addr", args.get())
              << _S("conn_non_tls", tsa->conn_non_tls)
              << _S("conn_tls", tsa->conn_tls)
              << _N("refcount", args.use_count());
    tsa->transport->close(tsa->conn_non_tls);
    tsa->transport->close(tsa->conn_tls);
}
#if defined WITH_LIBWEBSOCKETS
/*************** Websocket ***************/
#define MAX_ECHO_PAYLOAD 1024
static bool g_client_recv = false;

struct per_session_data__echo {
    bool client_send;
};

static int callback_echo(struct lws *wsi,
                         enum lws_callback_reasons reason,
                         void *user,
                         void *in,
                         size_t len)
{
    struct per_session_data__echo *pss = (struct per_session_data__echo *)user;
    string send_msg = "hello";
    string msg;

    switch (reason) {
    case LWS_CALLBACK_ESTABLISHED:
        pss->client_send = false;
        g_client_recv = false;
        break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
        send_msg += ", client";
        SOCE_DEBUG << _S("server echo", send_msg);
        lws_write(wsi,
                  (unsigned char*)(const_cast<char*>(send_msg.c_str())),
                  send_msg.size(),
                  LWS_WRITE_TEXT);

        break;

    case LWS_CALLBACK_RECEIVE:
        msg = string((char*)in, len);
        SOCE_DEBUG << _S("server recv", msg);
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_CLOSED:
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        SOCE_DEBUG << _S("client recv", string((char*)in, len));
        g_client_recv = true;
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        send_msg += ", server";
        if (!pss->client_send){
            SOCE_DEBUG << _S("client send", send_msg);
            lws_write(wsi,
                      (unsigned char*)(const_cast<char*>(send_msg.c_str())),
                      send_msg.size(),
                      LWS_WRITE_TEXT);
            pss->client_send = true;
        }
        break;

    default:
        break;
    }

    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "ws-echo",    /* name */
        callback_echo,    /* callback */
        sizeof (struct per_session_data__echo),  /* per_session_data_size */
        MAX_ECHO_PAYLOAD,      /* max frame size / rx buffer */
    },

    { NULL, NULL, 0, 0 } /* terminator */
};

static void ws_client_entry()
{
    sleep(2);
    SOCE_DEBUG << _S("test", "websocket");
    char uri[256] = "/";
    struct lws_context *context;
    struct lws *wsi;
    struct lws_client_connect_info i;
    struct lws_context_creation_info info;

    memset(&i, 0, sizeof i);
    memset(&info, 0, sizeof info);

    info.port = CONTEXT_PORT_NO_LISTEN;;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    info.extensions = nullptr;
    info.options = LWS_SERVER_OPTION_VALIDATE_UTF8;
    info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    context = lws_create_context(&info);
    if (context == NULL) {
        SOCE_ERROR << _D("Failed to create lws context");
        return;
    }

    i.context = context;
    i.address = "127.0.0.1";
    i.port = 7681;
    i.ssl_connection = 0;
    i.path = uri;
    i.host = "127.0.0.1:7681";
    i.origin = "127.0.0.1:7681";
    i.protocol = "ws-echo";
    wsi = lws_client_connect_via_info(&i);
    if (!wsi){
        SOCE_ERROR << _D("Failed to create lws context");
        return;
    }

    while (!g_client_recv){
        lws_callback_on_writable_all_protocol(context, &protocols[0]);
        lws_service(context, 10);
    }
}


/*************** End Websocket ***************/
#endif

int main()
{
    TransportIf::SSLCert server_cert;
    server_cert.ca_file = "../../cert/ca/cacert.pem";
    server_cert.cert_file = "../../cert/server/cert.pem";
    server_cert.key_file = "../../cert/server/key.pem";

    TransportIf::SSLCert client_cert;
    client_cert.ca_file = "../../cert/ca/cacert.pem";
    client_cert.cert_file = "../../cert/client/cert.pem";
    client_cert.key_file = "../../cert/client/key.pem";

    TransportLibevent tle;
    tle.init();

    std::shared_ptr<BaseProcessor> svr_prc(new ServerProcessor);
    svr_prc->set_transport(&tle);
    tle.listen("127.0.0.1:8988", svr_prc);
    tle.listen(server_cert, "127.0.0.1:8989", svr_prc);

    uint64_t conn_non_tls = 0;
    uint64_t conn_tls = 0;
    tle.connect("127.0.0.1:8988",
                std::shared_ptr<ProcessorIf>(new ClientProcessor),
                conn_non_tls);
    tle.connect(client_cert,
                "127.0.0.1:8989",
                std::shared_ptr<ProcessorIf>(new ClientProcessor),
                conn_tls);

    string msg("hello, not tls on 8988!");
    int len = msg.size();
    tle.send(conn_non_tls, (char*)&len, 4);
    tle.send(conn_non_tls, msg.c_str(), len);

    msg = "hello, tls on 8989!";
    len = msg.size();
    tle.send(conn_tls, (char*)&len, 4);
    tle.send(conn_tls, msg.c_str(), len);

    std::shared_ptr<TimerSampleArg> pta = std::make_shared<TimerSampleArg>();
    pta->transport = &tle;
    pta->conn_non_tls = conn_non_tls;
    pta->conn_tls = conn_tls;

    tle.add_timer(100, TransportIf::kTimerModeOnceAndDel, std::bind(timer_entry, pta));
    tle.signal(SIGINT, [&](int sig){
            tle.stop();
        });

#if defined WITH_LIBWEBSOCKETS
    int conn = 0;
    if (tle.listen("127.0.0.1:7681", protocols, nullptr, &conn)){
        SOCE_ERROR << _S("listen failed", "localhost:7681");
    }

    std::thread t = std::thread(ws_client_entry);
    tle.run();

    t.join();
#else
    tle.run();
#endif

    return 0;
}
