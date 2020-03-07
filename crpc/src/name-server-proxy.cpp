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

#include <stdlib.h>
#include "crpc/log.h"
#include "crpc/nameserver/name-server-proxy.h"
#include "crpc/base-processor.h"
#include "gen-cpp/proxy-msg.h"
#include "proto/binary-proto.h"

using namespace soce::transport;
using soce::proto::ProtoIf;
using soce::proto::BinaryProto;

namespace soce{
namespace crpc{

    class ProxyCtlProcessor : public BaseProcessor
    {
    public:
        virtual void process(uint64_t conn_id, std::string&& data)
            {
                (void) conn_id;
                (void) data;
            }

        virtual void on_connected(uint64_t conn_id)
            {
                (void) conn_id;
            }

        virtual void on_error(uint64_t conn_id)
            {
                (void) conn_id;
            }
    };

    NameServerProxy::NameServerProxy()
    {
    }

    NameServerProxy::NameServerProxy(const std::string& ctl_addr, const std::string& req_addr)
        : ctl_addr_(ctl_addr),
          req_addr_(req_addr)
    {
    }

    void NameServerProxy::set_ctl_addr(const std::string& ctl_addr)
    {
        ctl_addr_ = ctl_addr;
    }

    void NameServerProxy::set_req_addr(const std::string& req_addr)
    {
        req_addr_ = req_addr;
    }

    void NameServerProxy::watch_all()
    {
    }

    void NameServerProxy::on_connected(uint64_t conn_id)
    {
        (void) conn_id;
    }

    int NameServerProxy::add_server(const std::string& service,
                                    const std::string& value)
    {
        send_register_info(service, value);

        return 0;
    }

    void NameServerProxy::del_server(uint64_t conn_id)
    {
        (void) conn_id;
    }

    uint64_t NameServerProxy::get_server(const std::string& service,
                                         const std::string& method,
                                         const std::string& request)
    {
        (void) service;
        (void) method;
        (void) request;

        return req_fd_;
    }

    uint64_t NameServerProxy::get_server(const std::string& service,
                                         const std::string& method,
                                         const std::string& request,
                                         const std::string& target_addr)
    {
        (void) service;
        (void) method;
        (void) request;
        (void) target_addr;

        return req_fd_;
    }

    int NameServerProxy::get_cond_servers(const std::string& service,
                                          const std::string& method,
                                          const std::string& request,
                                          std::set<uint64_t>& conn_ids)
    {
        (void) service;
        (void) method;
        (void) request;

        conn_ids.insert(req_fd_);

        return 0;
    }

    int NameServerProxy::get_all_servers(const std::string& service,
                                         const std::string& method,
                                         const std::string& request,
                                         std::set<uint64_t>& conn_ids)
    {
        (void) service;
        (void) method;
        (void) request;

        conn_ids.insert(req_fd_);

        return 0;
    }

    void NameServerProxy::do_watch_service(const std::string& service)
    {
        send_watched_service(service);
    }

    void NameServerProxy::run()
    {
        auto transport = processor_->get_transport();

        int rc = transport->connect(req_addr_,
                                    processor_,
                                    req_fd_);
        if (rc != 0) {
            SOCE_ERROR << _S("InvalidProxyReqAddr", req_addr_);
            exit(-1);
        }

        rc = transport->connect(ctl_addr_,
                                std::shared_ptr<ProcessorIf>(new ProxyCtlProcessor),
                                ctl_fd_);
        if (rc != 0) {
            SOCE_ERROR << _S("InvalidProxyCtlAddr", ctl_addr_);
            exit(-1);
        }

        send_server_info();
    }

    void NameServerProxy::send_server_info()
    {
        CRPC_DEBUG << _S("LocalServerAddr", server_addr_);

        proxy::ProxyMsgServerInfo msg;
        msg.set_type(proxy::kProxyMsgTypeServerInfo);
        msg.set_server_addr(server_addr_);
        BinaryProto bp;
        if (msg.serialize(&bp) == 0) {
            SOCE_ERROR << _D("NameServerProxy failed to serialize server info");
            exit(-1);
        }

        send_msg(bp.data(), bp.size());
    }

    void NameServerProxy::send_register_info(const std::string& service, const std::string& filters)
    {
        proxy::ProxyMsgRegisterInfo msg;
        msg.set_type(proxy::kProxyMsgTypeRegister);
        msg.set_service(service);
        msg.set_filters(filters);
        BinaryProto bp;
        if (msg.serialize(&bp) == 0) {
            SOCE_ERROR << _D("NameServerProxy failed to serialize server info");
            exit(-1);
        }

        send_msg(bp.data(), bp.size());
    }

    void NameServerProxy::send_watched_service(const std::string& service)
    {
        proxy::ProxyMsgWatchService msg;
        msg.set_type(proxy::kProxyMsgTypeWatchService);
        msg.set_service(service);
        BinaryProto bp;
        if (msg.serialize(&bp) == 0) {
            SOCE_ERROR << _D("NameServerProxy failed to serialize server info");
            exit(-1);
        }

        send_msg(bp.data(), bp.size());
    }

    void NameServerProxy::send_msg(const char* data, size_t size)
    {
        auto transport = processor_->get_transport();

        char buff[4];
        for (size_t i =0; i<4; ++i){
            buff[i] = (size >> (3-i)*8) & 0xff;
        }

        if (transport->send(ctl_fd_, buff, 4)
            || transport->send(ctl_fd_, data, size)){
            SOCE_ERROR << _D("Failed to send msg to proxy") << _S("ctl_fd", ctl_fd_);
            exit(-1);
        }
    }

} // namespace crpc
} // namespace soce
