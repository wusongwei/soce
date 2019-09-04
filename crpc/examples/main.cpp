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
#include "log4rel/async-logger.h"
#include "log4rel/console-sink.hpp"
#include "crpc/server.h"
#include "transport/transport-libevent.h"
#include "gen-cpp/echo-service.h"
#include "gen-cpp/echo.h"
#include "proto/binary-proto.h"
#include "crpc/interceptor.h"

using namespace soce::crpc;
using namespace std;

/**
 * crpc目前仅支持协程客户端，只能在工作线程中使用。
 *
 * 若想crpc对外提供服务，可以重载服务的start()方法，
 * 在此方法中利用transport_成员监听端口，实现用户特定的对外协议。
 * 服务所提供的方法都是在工作线程中运行，所以可以在用户特定协议
 * 的处理函数中调用协程客户端，从而使用crpc框架。
 * 另外一种思路是在start()方法中开启新线程，在新线程中接受外部请求，
 * 再利用分发队列传给工作线程。
 *
 * 本示例注册了两个服务：Gateway服务和Echo服务。
 * 为了展示RPC调用的封装过程，示例在gateway线程中按crpc协议规范封装
 * 了一个Gateway服务的recv请求发送给Gateway服务。
 * Gateway服务在接收到请求后作为客户端发起Echo请求。
 *
 * 服务所有的代码都在工作线程中执行，因此服务启动的定时器也在工作线程中执行。
 * 示例中还启动了一个定时器，在定时器中尝试RPC调用。
 */
class GatewayService : public crpc::sample::GatewayIf
{
public:
    void timer_entry()
        {
            SOCE_INFO << _D("Timer test");
            soce::cortengine::CortEngine::CortId id = SCortEngine.create([&]{
                    crpc::sample::EchoCortClient clt;
                    clt.set_timeout(1000);
                    //clt.set_target_addr("127.0.0.1:7890");
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
                                  std::bind(&GatewayService::timer_entry, this));
        }

    void end()
        {
        }

    virtual void recv(const string& msg)
        {
            SOCE_INFO << _S("Service", "Gateway") << _S("RecvMsg", msg);

            /*
             * 默认情况下会为每个请求创建一个协程来处理。
             * 无处理过程中不会再使用与协议相关的东西，
             * 比如channel，或者利用协程客户端访问
             * 则可以修改执行模式，避免创建协程，提升性能。
             */
            crpc::sample::EchoCortClient clt;

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
        }
};

class GatewayServiceFactory : public ServiceFactoryIf
{
public:
    virtual std::shared_ptr<ServiceIf> create()
        {
            GatewayService* p = new GatewayService();
            return std::shared_ptr<ServiceIf>(p);
        }
};

class EchoService : public crpc::sample::EchoIf
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

    void do_uncondcast_test(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] uncondcast_test", p);
        }

    void do_condcast_test(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] condcast_test", p);
        }

    void do_null_test(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] null_test", p);
        }

    void do_void_test(const int32_t& p)
        {
            SOCE_INFO << _S("[Server] void_test", p);
        }
};

class EchoServiceFactory : public ServiceFactoryIf
{
public:
    virtual std::shared_ptr<ServiceIf> create()
    {
        EchoService* p = new EchoService();
        p->set_echo_filter("ceo.age <= 3 || p > 10");
        p->set_do_condcast_test_filter("p >= 3");
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
            processor->add_service_factory(std::shared_ptr<ServiceFactoryIf>(new GatewayServiceFactory), false);
            processor->add_service_factory(std::shared_ptr<ServiceFactoryIf>(new EchoServiceFactory), false);
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
            nameserver->watch_service("Echo");
            //nameserver->watch_service("Gateway");
            return std::shared_ptr<NameServerIf>(nameserver);
        }
};

static void gateway()
{
    sleep(1);

    int conndfd;
    struct sockaddr_in serverAddr;
    if(-1==(conndfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))) {
        cout << "conn error" << endl;
        return;
    }

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serverAddr.sin_port=htons(7890);
    if(-1==connect(conndfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))) {
        printf("Connetc Error\n");
        exit(0);
    }

    crpc::sample::GatewayrecvReq req;

    req.mutable_header().set_type(soce::crpc::kRpcTypeReq);
    req.mutable_header().set_service("Gateway");
    req.mutable_header().set_method("recv");
    req.mutable_header().set_req_id(1);
    req.mutable_header().set_tid(0);
    req.set_msg("abcd");

    soce::proto::BinaryProto bp;
    req.serialize(&bp);

    char buf[1000];
    size_t len = bp.size();
    for (size_t i =0; i<4; ++i){
        buf[i] = (len >> (3-i)*8) & 0xff;
    }

    memcpy(buf + 4, bp.data(), bp.size());

    for (size_t i =0; i< 1; ++i){
        send(conndfd, buf, 4 + bp.size(), 0);
    }

    sleep(20);
}
/**
 * 拦截器示例，可以每个请求执行前执行切片函数，
 * 记录执行前和执行后的时间。
 */

int interceptor_before(soce::crpc::CrpcReqHeader& header,soce::crpc::CrpcErrorResp& err_resp)
{
    (void) err_resp;
    SOCE_INFO << _S("req_id", header.get_req_id()) << _S("Service", header.get_service());
    return 0;
}

int interceptor_after(soce::crpc::CrpcRespHeader& header,soce::crpc::CrpcErrorResp& err_resp)
{
    (void) err_resp;
    SOCE_INFO << _S("req_id", header.get_req_id());
    return 0;
}

void init_logger()
{
    SAsyncLogger.set_sink(std::shared_ptr<soce::log4rel::ConsoleSink>(new soce::log4rel::ConsoleSink));
    SAsyncLogger.set_logger_creator([](){
            std::shared_ptr<soce::log4rel::Logger> logger(new soce::log4rel::Logger);
            logger->reserve_field(soce::log4rel::kLogFieldLevel, true);
            logger->reserve_field(soce::log4rel::kLogFieldPos, true);
            logger->reserve_field(soce::log4rel::kLogFieldTid, true);
            logger->set_log_level(soce::log4rel::kLogLevelInfo);
            return logger;
        });
}

int main()
{
    init_logger();

    // std::string service = "Echo";
    // std::string method = "echo";
    // SInterceptor.add_before_itcptor("echo_before", 1, interceptor_before, service, method, false);
    // SInterceptor.add_after_itcptor("echo_after", 1, interceptor_after, service, method, false);

    std::thread t = std::thread(gateway);
    Server server("127.0.0.1:7890",
                  std::shared_ptr<TransportFactoryIf>(new LibeventTransportFactory),
                  std::shared_ptr<ProcessorFactoryIf>(new ProcessorFactory),
                  std::shared_ptr<NameServerFactoryIf>(new NameServerZkFactory));
    server.run();
    t.join();

    return 0;
}
