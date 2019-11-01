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

#include "dispatcher.h"
#include "crpc/worker-thread.h"
#include "transport/bypass-processor.h"
#include "proto/binary-proto.h"
#include "proto/dynamic-getter.h"
#include "proto/dynamic-setter.h"

using soce::transport::BypassProcessor;

namespace soce {
namespace crpc {

    int ProxyDispatcher::init(int thread_index,
                              soce::transport::TransportIf* transport,
                              std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyRequest>> req_queue,
                              std::shared_ptr<soce::utils::DispatchQueue<ProxyDispatcher::ProxyResponse>> resp_queue)
    {
        thread_index_ = thread_index;
        req_queue_ = req_queue;
        resp_queue_ = resp_queue;

        transport->watch(req_queue_->get_consumer_fd(thread_index_),
                         std::shared_ptr<BypassProcessor>(
                             new BypassProcessor(
                                 std::bind(&ProxyDispatcher::recv_queue_message,
                                           this,
                                           std::placeholders::_1))));

        return 0;
    }

    void ProxyDispatcher::recv_queue_message(int fd)
    {
        (void) fd;

        soce::utils::FQVector<ProxyRequest> reqs;
        if (req_queue_->try_consume(thread_index_, reqs)){
            return;
        }

        for (auto& i : reqs){
            handle_queue_message(i.conn_id, std::move(i.request));
        }
    }

    void ProxyDispatcher::handle_queue_message(uint64_t conn_id, std::string&& request)
    {
        soce::cortengine::CortEngine::CortId id = SCortEngine.create([&, conn_id]{
                soce::crpc::WorkerThread* work_thread = soce::crpc::WorkerThread::s_self;
                int64_t req_id = work_thread->get_reqid();
                CrpcRequest crpc_req;
                soce::proto::BinaryProto bp;

                if (request.size() < 4){
                    append_resp(conn_id, true, soce::crpc::kRpcStatusProtoError);
                    return;
                }

                char* data = const_cast<char*>(request.data() + 4);
                size_t size = request.size() - 4;
                bp.init(data, size);
                if (crpc_req.deserialize(&bp) == 0) {
                    append_resp(conn_id, true, soce::crpc::kRpcStatusProtoError);
                    return;
                }

                char old_req_id[8];
                soce::proto::DynamicGetter dg;
                dg.init(data, size);
                if (dg.get_bytes(reqid_index_, old_req_id, 8)) {
                    append_resp(conn_id, true, soce::crpc::kRpcStatusProtoError);
                    return;
                }

                soce::proto::DynamicSetter ds;
                ds.init(data, size);
                if (ds.set_bytes(reqid_index_, (char*)&req_id, 8)){
                    append_resp(conn_id, true, soce::crpc::kRpcStatusProtoError);
                    return;
                }
                
                crpc_req.mutable_header().set_req_id((char*)&req_id);
                work_thread->append_req(crpc_req.get_header(),
                                        std::move(std::string(data, size)));
                work_thread->add_request(req_id, SCortEngine.get_cur_cort_id());

                SCortEngine.yield();
                work_thread = soce::crpc::WorkerThread::s_self;
                std::string* resp_str = work_thread->get_last_resp();
                if (resp_str == NULL || resp_str->empty()) {
                    append_resp(conn_id, true, soce::crpc::kRpcStatusServiceNotAvailable);
                    return;
                }

                ds.init(const_cast<char*>(resp_str->data()), resp_str->size());
                ds.set_bytes(reqid_index_, old_req_id, 8);
                resp_queue_->produce(conn_id, false, std::move(*resp_str));
            });

        SCortEngine.resume(id);
    }

    void ProxyDispatcher::append_resp(uint64_t conn_id, bool err, soce::crpc::RpcStatus status)
    {
        char req_id[8];
        CrpcErrorResp resp;
        resp.mutable_header().set_type(soce::crpc::kRpcTypeResp);
        resp.mutable_header().set_req_id(req_id);
        resp.mutable_header().set_status(soce::crpc::kRpcStatusProtoError);

        soce::proto::BinaryProto bp;
        if (resp.serialize((soce::proto::ProtoIf*)&bp)) {
            resp_queue_->produce(conn_id, err, std::move(std::string(bp.data(), bp.size())));
        }
    }

} // namespace crpc
} // namespace soce
