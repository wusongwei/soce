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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <thread>
#include "log4rel/console-sink.hpp"
#include "crpc/server.h"
#include "transport/transport-libevent.h"
#include "proto/binary-proto.h"
#include "crpc/interceptor.h"
#include "crpc/crpc-proxy.h"
#include "gen-cpp/example.h"
#include "gen-cpp/example-service.h"

using namespace crpc::example;
using namespace soce::crpc;
using namespace soce::cortengine;
using namespace std;

/**
 * crpc目前支持同步客户端和协程客户端，其中协程客户端只能在工作线程中使用。
 *
 * 本示例注册了两个服务：SyncDemo服务和CortDemo服务。
 * 在函数client_demo()中使用SyncDemo服务的同步客户端访问SyncDemo服务。
 * 在SyncDemo服务收到请求后，使用CortDemo的协程客户端CortDemoCortClient
 * 来访问CortDemo服务。
 *
 * 服务所有的代码都在工作线程中执行，因此服务启动的定时器也在工作线程中执行。
 * 示例中还启动了一个定时器，在定时器中尝试RPC调用。
 *
 * 若想crpc对外提供服务，可以重载服务的start()方法，
 * 在此方法中利用transport_成员监听端口，实现用户特定的对外协议。
 * 服务所提供的方法都是在工作线程中运行，所以可以在用户特定协议
 * 的处理函数中调用协程客户端，从而使用crpc框架。
 * 另外一种思路是在start()方法中开启新线程，在新线程中接受外部请求，
 * 再利用分发队列传给工作线程。
 */
class SyncDemo : public SyncDemoIf
{
public:
    void timer_entry()
        {
            SOCE_INFO << _D("Timer test");
            CortEngine::CortId id = SCortEngine.create([&]{
                    CortDemoCortClient clt;
                    clt.set_timeout(1000);
                    //clt.set_target_addr("127.0.0.1:7891");
                    int value = 0;
                    if (clt.echo(value, 100)){
                        SOCE_ERROR << _D("Timer test, echo failed");
                    }
                    else{
                        SOCE_INFO << _S("Timer test, echo value", value);
                    }
                });

            SCortEngine.resume(id);
        }

    void start()
        {
            std::shared_ptr<void> timer_arg;
            transport_->add_timer(1000,
                                  soce::transport::TransportIf::kTimerModeOnceAndDel,
                                  std::bind(&SyncDemo::timer_entry, this));
        }

    void end()
        {
        }

    virtual std::string hello(const string& msg)
        {
            SOCE_INFO << _S("Service", "SyncServer") << _S("RecvMsg", msg);
            /*
             * 默认情况下会为每个请求创建一个协程来处理。
             * 无处理过程中不会再使用与协议相关的东西，
             * 比如channel，或者利用协程客户端访问
             * 则可以修改执行模式，避免创建协程，提升性能。
             */
            CortDemoCortClient clt;

            int value = 0;
            if (clt.echo(value, 200)){
                SOCE_ERROR << _D("[Client] Echo failed");
            }
            else{
                SOCE_INFO << _S("[Client] Echo success , echo value", value);
            }

            if (clt.echo(value, 5)){
                SOCE_ERROR << _D("[Client] Echo failed, condition p < 3 || p > 10");
            }
            else{
                SOCE_INFO << _S("[Client] Echo success, echo value", value);
            }

            return msg + ", world";
        }
};

class SyncDemoFactory : public ServiceFactoryIf
{
public:
    virtual std::shared_ptr<ServiceIf> create()
        {
            SyncDemo* p = new SyncDemo();
            return std::shared_ptr<ServiceIf>(p);
        }
};

class CortDemo : public CortDemoIf
{
public:
    void start()
        {
        }

    void end()
        {
        }

    int32_t  echo(const int32_t& p){
        SOCE_INFO << _S("[Server] echo", p);
        return p;
    }

    void test_void(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] null_test", p);
        }

    void test_null(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] void_test", p);
        }

    void test_uncondcast(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] uncondcast_test", p);
        }

    void test_condcast(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] condcast_test", p);
        }
};

class CortDemoFactory : public ServiceFactoryIf
{
public:
    virtual std::shared_ptr<ServiceIf> create()
    {
        CortDemo* p = new CortDemo();
        p->set_echo_filter("p <= 3 || p > 10");
        p->set_test_condcast_filter("p >= 3");
        return std::shared_ptr<ServiceIf>(p);
    }
};

class LibeventTransportFactory : public TransportFactoryIf
{
public:
    virtual std::shared_ptr<soce::transport::TransportIf> create()
        {
            soce::transport::TransportLibevent* transport = new soce::transport::TransportLibevent;
            transport->init();
            return std::shared_ptr<soce::transport::TransportIf>(transport);
        }
};

class ProcessorFactory : public ProcessorFactoryIf
{
public:
    virtual std::shared_ptr<Processor> create()
        {
            std::shared_ptr<Processor> processor(new Processor);

            processor->set_shared_thread_num(4);
            processor->add_service_factory(std::shared_ptr<ServiceFactoryIf>(new SyncDemoFactory), false);
            processor->add_service_factory(std::shared_ptr<ServiceFactoryIf>(new CortDemoFactory), false);
            return processor;
        }
};

class NameServerZkFactory : public NameServerFactoryIf
{
public:
    virtual std::shared_ptr<NameServerIf> create()
        {
            NameServerZk* nameserver = new NameServerZk;
            nameserver->init("127.0.0.1:2181", 4000);
            nameserver->watch_service("SyncDemo");
            nameserver->watch_service("CortDemo");
            return std::shared_ptr<NameServerIf>(nameserver);
        }
};

static void client_demo()
{
    sleep(1);

    SyncDemoSyncClient client;
    client.init("127.0.0.1", 7890);
    string hello_resp;
    soce::crpc::RpcStatus status = client.hello(hello_resp, "hello");
    SOCE_INFO << _S("Status", status) << _S("hello_resp", hello_resp);
}

/**
 * 拦截器示例，可以每个请求执行前执行切片函数，
 * 记录执行前和执行后的时间。
 */

int interceptor_before(soce::crpc::CrpcReqHeader& header,soce::crpc::CrpcErrorResp& err_resp)
{
    (void) err_resp;
    int64_t req_id = *(int64_t*)const_cast<char*>(header.get_req_id());
    SOCE_INFO << _S("req_id", req_id) << _S("Service", header.get_service());
    return 0;
}

int interceptor_after(soce::crpc::CrpcRespHeader& header,soce::crpc::CrpcErrorResp& err_resp)
{
    (void) err_resp;
    int64_t req_id = *(int64_t*)const_cast<char*>(header.get_req_id());
    SOCE_INFO << _S("req_id", req_id);
    return 0;
}

void proxy()
{
    CrpcProxyBuilder builer;
    builer.set_service_addr("127.0.0.1:7890")
        .set_ns_addr("127.0.0.1:2181")
        .build()
        ->start();
}

int main()
{
    // std::string service = "CortDemo";
    // std::string method = "echo";
    // SInterceptor.add_before_itcptor("echo_before", 1, interceptor_before, service, method, false);
    // SInterceptor.add_after_itcptor("echo_after", 1, interceptor_after, service, method, false);

    std::thread c = std::thread(client_demo);
    std::thread p = std::thread(proxy);

    Server server("127.0.0.1:0",
                  std::shared_ptr<TransportFactoryIf>(new LibeventTransportFactory),
                  std::shared_ptr<ProcessorFactoryIf>(new ProcessorFactory),
                  std::shared_ptr<NameServerFactoryIf>(new NameServerZkFactory));


    server.run();

    c.join();
    p.join();

    return 0;
}
