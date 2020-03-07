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

#include "crpc/log.h"
#include "control-processor.h"
#include "proto/binary-proto.h"
#include "proto/dynamic-getter.h"
#include "gen-cpp/proxy-msg.h"

namespace soce{
namespace crpc{

    ProxyControlProcessor::ProxyControlProcessor(const std::string& remote_req_addr,
                                                 std::shared_ptr<NameServerIf> nameserver)
        : remote_req_addr_(remote_req_addr),
          nameserver_(nameserver)
    {
    }

    ProxyControlProcessor::~ProxyControlProcessor()
    {
        if (run_) {
            run_ = false;

            transport_.stop();
            if (remote_req_thread_.joinable()) {
                remote_req_thread_.join();
            }
        }
    }

    void ProxyControlProcessor::process(uint64_t conn_id, std::string&& data)
    {
        char* msg = const_cast<char*>(data.c_str() + 4);
        size_t size = data.size() - 4;
        soce::proto::DynamicGetter dg;
        dg.init(msg, size);

        int32_t msg_type;
        if (dg.get_enum(type_index_, msg_type)){
            SOCE_ERROR << _S("conn_id", conn_id) << _S("FailedAction", "getType");
            return;
        }

        CRPC_DEBUG << _S("ProxyMsgType", msg_type);

        if (msg_type == proxy::kProxyMsgTypeServerInfo)
        {
            handle_server_info_msg(msg, size);
        }
        else if (msg_type == proxy::kProxyMsgTypeRegister)
        {
            handle_register_msg(msg, size);
        }
        else if (msg_type == proxy::kProxyMsgTypeWatchService)
        {
            handle_watch_service_msg(msg, size);
        }
        else{
            SOCE_ERROR << _S("InvalidControlMsgType", msg_type);
        }
    }

    void ProxyControlProcessor::on_connected(uint64_t conn_id)
    {
    }

    void ProxyControlProcessor::on_error(uint64_t conn_id)
    {
    }

    void ProxyControlProcessor::handle_server_info_msg(char* data, size_t size)
    {
        soce::proto::BinaryProto bp;
        bp.init(data, size);

        proxy::ProxyMsgServerInfo msg;
        if (msg.deserialize(&bp) == 0) {
            SOCE_ERROR << _D("Invalid proxy server info");
        }

        remote_req_thread_ = std::thread(&ProxyControlProcessor::handle_remote_req, this, msg.get_server_addr());
    }

    void ProxyControlProcessor::handle_register_msg(char* data, size_t size)
    {
        soce::proto::BinaryProto bp;
        bp.init(data, size);

        proxy::ProxyMsgRegisterInfo msg;
        if (msg.deserialize(&bp) == 0) {
            SOCE_ERROR << _D("Invalid proxy register info");
        }

        nameserver_->set_server_addr(remote_req_addr_);
        nameserver_->add_server(msg.get_service(), msg.get_filters());
    }

    void ProxyControlProcessor::handle_watch_service_msg(char* data, size_t size)
    {
        soce::proto::BinaryProto bp;
        bp.init(data, size);

        proxy::ProxyMsgWatchService msg;
        if (msg.deserialize(&bp) == 0) {
            SOCE_ERROR << _D("Invalid proxy watch service info");
        }

        nameserver_->do_watch_service(msg.get_service());
    }

    void ProxyControlProcessor::handle_remote_req(const std::string& addr)
    {
        transport_.init();
        run_ = true;

        local_req_processor_ = std::make_shared<LocalReqProcessor>();
        remote_req_processor_ = std::make_shared<RemoteReqProcessor>();

        transport_.connect(addr, local_req_processor_, local_server_fd_);
        transport_.listen(remote_req_addr_, remote_req_processor_);

        local_req_processor_->set_transport(&transport_);
        local_req_processor_->set_remote_req_processor(remote_req_processor_);
        remote_req_processor_->set_transport(&transport_);
        remote_req_processor_->set_local_server_fd(local_server_fd_);

        transport_.run();
    }

} // namespace crpc
} // namespace soce
