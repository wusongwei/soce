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

#include "crpc/client/base-cort-client.h"
#include "crpc/worker-thread.h"

namespace soce {
namespace crpc {

    RpcStatus BaseCortClient::do_req(const std::string& service,
                                     const std::string& method,
                                     soce::crpc::RpcType type,
                                     int64_t req_id,
                                     soce::crpc::CrpcReqHeader& req_header,
                                     soce::fads::FadsMessage* req_msg,
                                     soce::crpc::CrpcRespHeader& resp_header,
                                     soce::fads::FadsMessage* resp_msg)
    {
        soce::crpc::WorkerThread* work_thread = soce::crpc::WorkerThread::s_self;
        if (0 == req_id){
            req_id = work_thread->get_reqid();
        }

        soce::proto::BinaryProto bp;
        RpcStatus status;

        init_req_header(req_header, service, method, type, req_id);
        int rc = do_before_itcptor(req_header, service, method, resp_msg, status);
        if (rc != 0){
            return status;
        }

        if (!target_server_.empty()) {
            req_header.set_target_addr(target_server_);
        }

        bp.reset();
        if (0 == req_msg->serialize(&bp)) {
            return kRpcStatusProtoError;
        }

        work_thread->append_req(req_header, std::move(std::string(bp.data(), bp.size())));
        work_thread->add_request(req_id, SCortEngine.get_cur_cort_id());

        uint32_t timeout = get_timeout(method);
        SCortEngine.yield(timeout);

        if(SCortEngine.is_timeout()){
            work_thread->del_request(req_id);
            return soce::crpc::kRpcStatusTimeout;
        }

        work_thread = soce::crpc::WorkerThread::s_self;
        string* resp_str = work_thread->get_last_resp();
        if (!resp_str || resp_str->empty()) {
            return soce::crpc::kRpcStatusServiceNotAvailable;
        }

        status = deserialize_resp(resp_msg, resp_str->c_str(), resp_str->size());
        if (status != kRpcStatusOK) {
            return status;
        }

        return do_after_itcptor(resp_header, service, method);
    }

    RpcStatus BaseCortClient::do_req(const std::string& service,
                                     const std::string& method,
                                     soce::crpc::RpcType type,
                                     int64_t req_id,
                                     soce::crpc::CrpcReqHeader& req_header,
                                     soce::fads::FadsMessage* req_msg)
    {
        soce::crpc::WorkerThread* work_thread = soce::crpc::WorkerThread::s_self;
        if (0 == req_id) {
            req_id = work_thread->get_reqid();
        }


        init_req_header(req_header, service, method, type, req_id);
        if (!target_server_.empty()) {
            req_header.set_target_addr(target_server_);
        }

        soce::proto::BinaryProto bp;
        if (0 == req_msg->serialize(&bp)) {
            return kRpcStatusProtoError;
        }

        work_thread->append_req(req_header, std::move(std::string(bp.data(), bp.size())));
        return kRpcStatusOK;
    }

} // namespace crpc
} // namespace soce
