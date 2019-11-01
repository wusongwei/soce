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

#include <unistd.h>
#include "crpc/server.h"
#include "transport/transport-libevent.h"
#include "proto/binary-proto.h"
#include "utils/dispatch-queue.hpp"
#include "crpc/crpc-proxy.h"
#include "entry-processor.h"

using namespace soce::utils;
using namespace soce::transport;

namespace soce {
namespace crpc {

    class CrpcProxyLibeventTransportFactory : public TransportFactoryIf
    {
    public:
        virtual std::shared_ptr<TransportIf> create()
            {
                TransportLibevent* transport = new TransportLibevent;
                transport->init();

                if (-1 == thread_index_){
                    entry_processor_ = std::make_shared<ProxyEntryProcessor>(shared_thread_num_,
                        transport);
                    transport->listen(service_addr_, entry_processor_);
                }else{
                    std::shared_ptr<ProxyDispatcher> dispatcher = std::make_shared<ProxyDispatcher>();
                    dispatcher->init(thread_index_,
                                     transport,
                                     entry_processor_->get_req_queue(),
                                     entry_processor_->get_resp_queue());
                    dispatchers_.push_back(dispatcher);
                }
                ++thread_index_;
                return std::shared_ptr<TransportIf>(transport);
            }

        void set_shared_thread_num(size_t num)
            {
                shared_thread_num_ = num;
            }

        void set_service_addr(const std::string& service_addr)
            {
                service_addr_ = service_addr;
            }

    private:
        std::string service_addr_;
        size_t shared_thread_num_ = 0;
        int thread_index_ = -1;
        std::shared_ptr<ProxyEntryProcessor> entry_processor_;
        std::vector<std::shared_ptr<ProxyDispatcher>> dispatchers_;
    };

    class CrpcProxyProcessorFactory : public ProcessorFactoryIf
    {
    public:
        virtual std::shared_ptr<Processor> create()
            {
                std::shared_ptr<Processor> processor(new Processor);

                processor->set_shared_thread_num(shared_thread_num_);
                return processor;
            }

        void set_shared_thread_num(size_t num)
            {
                shared_thread_num_ = num;
            }

    private:
        size_t shared_thread_num_ = 0;
    };

    class NameServerZkFactory : public NameServerFactoryIf
    {
    public:
        virtual std::shared_ptr<NameServerIf> create()
            {
                NameServerZk* nameserver = new NameServerZk;
                nameserver->init(ns_addr_, timeout_);
                nameserver->set_service_dir(service_addr_);
                nameserver->watch_all();
                return std::shared_ptr<NameServerIf>(nameserver);
            }

        void set_name_server_addr(const std::string& addr)
            {
                ns_addr_ = addr;
            }

        void set_name_server_timeout(size_t timeout)
            {
                timeout_ = timeout;
            }

        void set_service_addr(const std::string& service_addr)
            {
                service_addr_ = service_addr;
            }

    private:
        std::string ns_addr_;
        size_t timeout_ = 4000;
        std::string service_addr_;
    };

    CrpcProxy::CrpcProxy(const std::string& service_addr,
                         const std::string& ns_addr,
                         size_t ns_timeout,
                         const std::string& service_dir,
                         size_t shared_thread_num)
        : service_addr_(service_addr),
          ns_addr_(ns_addr),
          ns_timeout_(ns_timeout),
          service_dir_(service_dir),
          shared_thread_num_(shared_thread_num)
    {
    }

    void CrpcProxy::start()
    {
        std::shared_ptr<CrpcProxyLibeventTransportFactory> transport_fac = std::make_shared<CrpcProxyLibeventTransportFactory>();
        transport_fac->set_service_addr(service_addr_);
        transport_fac->set_shared_thread_num(shared_thread_num_);

        std::shared_ptr<CrpcProxyProcessorFactory> processor_fac = std::make_shared<CrpcProxyProcessorFactory>();
        processor_fac->set_shared_thread_num(shared_thread_num_);

        std::shared_ptr<NameServerZkFactory> nameserver_fac = std::make_shared<NameServerZkFactory>();
        nameserver_fac->set_name_server_addr(ns_addr_);
        nameserver_fac->set_name_server_timeout(ns_timeout_);
        nameserver_fac->set_service_addr(service_dir_);

        Server server("127.0.0.1:0", transport_fac, processor_fac, nameserver_fac);
        server.run();
    }

    CrpcProxyBuilder& CrpcProxyBuilder::set_service_addr(const std::string& addr)
    {
        service_addr_ = addr;
        return *this;
    }


    CrpcProxyBuilder& CrpcProxyBuilder::set_ns_addr(const std::string& addr)
    {
        ns_addr_ = addr;
        return *this;
    }

    CrpcProxyBuilder& CrpcProxyBuilder::set_ns_timeout(size_t timeout)
    {
        ns_timeout_ = timeout;
        return *this;
    }

    CrpcProxyBuilder& CrpcProxyBuilder::set_service_dir(const std::string& dir)
    {
        service_dir_ = dir;
        return *this;
    }

    CrpcProxyBuilder& CrpcProxyBuilder::set_shared_thread_num(size_t num)
    {
        shared_thread_num_ = num;
        return *this;
    }

    std::shared_ptr<CrpcProxy> CrpcProxyBuilder::build()
    {
        std::shared_ptr<CrpcProxy> proxy = std::make_shared<CrpcProxy>(service_addr_,
                                                                       ns_addr_,
                                                                       ns_timeout_,
                                                                       service_dir_,
                                                                       shared_thread_num_);
        return proxy;
    }

} // namespace soce
} // namespace crpc
