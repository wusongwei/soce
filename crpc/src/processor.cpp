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

#include <sys/sysinfo.h>        // get_nprocs
#include <unistd.h>
#include <string>
#include <sys/eventfd.h>
#include "crpc/worker-thread.h"
#include "crpc/log.h"
#include "dispatch-queue.h"
#include "crpc/processor.h"
#include "transport/transport-if.h"
#include "transport/bypass-processor.h"
#include "proto/binary-proto.h"
#include "proto/dynamic-getter.h"
#include "gen-cpp/server-attr.h"
#include "gen-cpp/crpc-header.h"

using std::string;
using soce::transport::BypassProcessor;

namespace soce{
namespace crpc{

    Processor::Processor()
    {
    }

    void Processor::process(uint64_t conn_id, std::string&& data)
    {
        soce::proto::DynamicGetter dg;
        dg.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);

        int32_t rpc_type;
        if (dg.get_enum(type_index_, rpc_type)){
            SOCE_ERROR << _S("conn_id", conn_id) << _D("protocol error");
            return;
        }

        if (rpc_type == kRpcTypeResp){
            char p_req_id[8];
            if (dg.get_bytes(reqid_index_, p_req_id, 8)){
                SOCE_ERROR << _S("conn_id", conn_id) << _S("type", "response") << _D("protocol error");
                return;
            }

            int64_t req_id = *(int64_t*)p_req_id;
            int32_t tid = 0;
            auto it = reqid_map_.find(req_id);
            if (it != reqid_map_.end()){
                tid = it->second.second;
                auto it_conn_id = conn_id_map_.find(it->second.first);
                if (it_conn_id != conn_id_map_.end()){
                    auto it_tid = it_conn_id->second.find(it->second.second);
                    if (it_tid != it_conn_id->second.end()){
                        it_tid->second.erase(req_id);
                    }
                }

                reqid_map_.erase(it);
            }else{
                SOCE_ERROR<< _S("conn_id", conn_id) << _S("NoCachedReqid", req_id);
                return;
            }

            auto iter = dispatch_threads_.find(tid);
            if (iter == dispatch_threads_.end()){
                SOCE_ERROR<< _S("conn_id", conn_id) << _S("type", "response") << _S("no such thread", tid);
                return;
            }

            CRPC_DEBUG << _D("RecvResp")
                       << _S("ConnId", conn_id)
                       << _S("ReqId", req_id)
                       << _S("Tid", tid);

            iter->second->append_resp(conn_id, req_id, std::move(data));
        }
        else{
            string service;
            if (dg.get_string(service_index_, service)){
                SOCE_ERROR << _S("conn_id", conn_id) << _S("type", "request") << _D("protocol error");
                return;
            }

            CRPC_DEBUG << _D("RecvRequest")
                       << _S("RpcType", rpc_type)
                       << _S("ConnId", conn_id)
                       << _S("Service", service);

            auto iter = req_in_.find(service);
            if (iter != req_in_.end()){
                iter->second->produce(conn_id, std::move(service), std::move(data));
            }
            else{
                // In fact, should not be here!
                SOCE_ERROR << _S("conn_id", conn_id) << _S("type", "request") << _S("NoSuchService", service);
            }
        }

        return;
    }

    void Processor::on_connected(uint64_t conn_id)
    {
        nameserver_->on_connected(conn_id);
    }

    void Processor::on_error(uint64_t conn_id)
    {
        CRPC_DEBUG << _S("closed", conn_id);
        auto it_conn_id = conn_id_map_.find(conn_id);
        if (it_conn_id == conn_id_map_.end()){
            CRPC_DEBUG << _S("ShutdownOnInvalidConnId", conn_id);
            return;
        }

        for (auto& i : it_conn_id->second){
            auto it_dt = dispatch_threads_.find(i.first);
            if (it_dt != dispatch_threads_.end()){
                for (auto reqid : i.second){
                    it_dt->second->append_resp(conn_id, reqid, std::move(string()));
                    reqid_map_.erase(reqid);
                }
            }
        }

        conn_id_map_.erase(it_conn_id);
        nameserver_->del_server(conn_id);
    }

    int Processor::add_service_factory(std::shared_ptr<ServiceFactoryIf> factory, bool share_thread)
    {
        if (!factory){
            return -1;
        }

        std::shared_ptr<ServiceIf> service = factory->create();

        string name = service->get_name();
        if (thread_shared_services_.find(name) != thread_shared_services_.end()
            || thread_per_services_.find(name) != thread_per_services_.end()){
            return -1;
        }

        if (share_thread){
            thread_shared_services_[name] = service;
            service_factories_.push_back(factory);
        }
        else{
            thread_per_services_[name] = service;
        }

        return 0;
    }

    std::string Processor::get_service_info()
    {
        string rc;
        soce::crpc::attr::ProcessorAttrs pa;
        vector<string> attrs;

        for (auto& i : thread_shared_services_){
            string attr = i.second->get_service_info();
            if (!attr.empty()){
                attrs.push_back(std::move(attr));
            }
        }

        for (auto& i : thread_per_services_){
            string attr = i.second->get_service_info();
            if (!attr.empty()){
                attrs.push_back(std::move(attr));
            }
        }

        pa.set_server_attrs(std::move(attrs));

        soce::proto::BinaryProto pb;
        size_t len = pa.serialize((soce::proto::ProtoIf*)&pb);
        if (len != 0){
            rc.assign(pb.data(), pb.size());
        }

        return std::move(rc);
    }

    void Processor::register_to_ns()
    {
        for (auto& i : thread_shared_services_){
            nameserver_->add_server(i.second->get_name(),
                                    i.second->get_service_info());
        }

        for (auto& i : thread_per_services_){
            nameserver_->add_server(i.second->get_name(),
                                    i.second->get_service_info());
        }
    }

    void Processor::watch_request_out()
    {
        transport_->watch(req_out_->get_consumer_fd(),
                          std::shared_ptr<BypassProcessor>(new BypassProcessor(
                                                               std::bind(&Processor::handle_request_out,
                                                                         this,
                                                                         std::placeholders::_1))));
    }

    void Processor::watch_response_out()
    {
        transport_->watch(resp_out_->get_consumer_fd(),
                          std::shared_ptr<BypassProcessor>(new BypassProcessor(
                                                               std::bind(&Processor::handle_response_out,
                                                                         this,
                                                                         std::placeholders::_1))));
    }

    void Processor::handle_request_out(int fd)
    {
        (void) fd;

        soce::utils::FQVector<RequestOut::QueueData> reqs;
        if (req_out_->try_consume(reqs)){
            return;
        }

        for (auto& i : reqs){
            std::set<uint64_t>& conn_ids = i.conn_ids_;
            int64_t& req_id = i.req_id_;
            int32_t& tid = i.tid_;
            std::string& data = i.data_;

            if (conn_ids.empty()){
                append_null_resp(0, req_id, tid);
                continue;
            }

            for (auto conn_id : i.conn_ids_) {
                if (conn_id == 0 || send(conn_id, data.c_str(), data.size())) {
                    append_null_resp(conn_id, req_id, tid);
                    continue;
                }

                conn_id_map_[conn_id][tid].insert(req_id);
                reqid_map_[req_id] = std::make_pair(conn_id, tid);

                CRPC_DEBUG << _D("SendRequest")
                           << _S("ConnId", conn_id)
                           << _S("ReqId", req_id)
                           << _S("Tid", tid);
            }
        }
    }

    void Processor::append_null_resp(uint64_t conn_id, int64_t req_id, int32_t tid)
    {
        auto iter = dispatch_threads_.find(tid);
        if (iter != dispatch_threads_.end()){
            iter->second->append_resp(conn_id, req_id, std::move(string()));
        }
    }

    void Processor::handle_response_out(int fd)
    {
        (void) fd;

        soce::utils::FQVector<ResponseOut::QueueData> resps;
        if (resp_out_->try_consume(resps)){
            return;
        }

        for (auto& i : resps){
            send(i.conn_id_, i.data_.c_str(), i.data_.size());
            CRPC_DEBUG << _D("SendResponse") << _S("ConnId", i.conn_id_);
        }
    }

    int Processor::run()
    {
        size_t cores = shared_threads_;
        if (cores == 0){
            cores = 2 * (size_t)get_nprocs() + 1;
        }

        req_out_.reset(new RequestOut());
        resp_out_.reset(new ResponseOut());
        if (!req_out_->good() || !resp_out_->good()){
            return -1;
        }

        int tid = 0;
        int rc = run_thread_shared_service(cores, tid);
        if (rc){
            return -1;
        }

        rc = run_thread_per_service(tid);
        if (rc){
            return -1;
        }

        for (auto& i : dispatch_threads_){
            i.second->run();
        }

        watch_request_out();
        watch_response_out();
        register_to_ns();

        return 0;
    }

    int Processor::run_thread_shared_service(size_t cores, int& tid)
    {
        std::shared_ptr<RequestIn> req_in(new RequestIn(cores));
        std::shared_ptr<ResponseIn> resp_in(new ResponseIn(cores));
        if (!req_in->good() || !resp_in->good()){
            return -1;
        }

        for (size_t i=0; i<cores; ++i){
            auto transport = transport_factory_->create();
            if (!transport){
                return -1;
            }
            std::shared_ptr<WorkerThread> dt(new WorkerThread(tid,
                                                              i,
                                                              req_in,
                                                              req_out_,
                                                              resp_in,
                                                              resp_out_,
                                                              nameserver_,
                                                              transport,
                                                              get_snowflake()));
            if (i == 0){
                for (auto& i : thread_shared_services_){
                    dt->add_service(i.second);
                    i.second->set_transport(dt->get_transport());
                    req_in_[i.first] = req_in;
                }
            }
            else{
                for (auto& i : service_factories_){
                    std::shared_ptr<ServiceIf> service = i->create();
                    if (!service){
                        return -1;
                    }
                    dt->add_service(service);
                    service->set_transport(dt->get_transport());
                }
            }
            dispatch_threads_[tid] = dt;
            ++tid;
        }

        return 0;
    }

    int Processor::run_thread_per_service(int& tid)
    {
        for (auto& i : thread_per_services_){
            std::shared_ptr<RequestIn> req_in(new RequestIn(1));
            std::shared_ptr<ResponseIn> resp_in(new ResponseIn(1));
            if (!req_in->good() || !resp_in->good()){
                return -1;
            }

            auto transport = transport_factory_->create();
            if (!transport){
                return -1;
            }

            std::shared_ptr<WorkerThread> dt(new WorkerThread(tid,
                                                              0,
                                                              req_in,
                                                              req_out_,
                                                              resp_in,
                                                              resp_out_,
                                                              nameserver_,
                                                              transport,
                                                              get_snowflake()));
            dt->add_service(i.second);
            i.second->set_transport(dt->get_transport());
            req_in_[i.first] = req_in;
            dispatch_threads_[tid] = dt;
            ++tid;
        }

        return 0;
    }

} // namespace crpc
} // namespace soce
