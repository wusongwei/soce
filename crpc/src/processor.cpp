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
        SAsyncLogger.init_thread_logger();
    }

    size_t Processor::get_max_header_size()
    {
        return 4;
    }

    size_t Processor::get_full_pkt_size(const char* data, size_t len)
    {
        uint32_t size = 0;
        if (len == 4){
            for (size_t i = 0; i < 4; ++i){
                size = (size << 8) | (*(data + i) & 0xFF);
            }
        }

        return size + 4;
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
            int64_t req_id = 0;
            int32_t tid = 0;
            if (dg.get_int64(reqid_index_, req_id)
                || dg.get_int32(tid_index_, tid)){
                SOCE_ERROR << _S("conn_id", conn_id) << _S("type", "response") << _D("protocol error");
                return;
            }

            auto iter = dispatch_threads_.find(tid);
            if (iter == dispatch_threads_.end()){
                SOCE_ERROR<< _S("conn_id", conn_id) << _S("type", "response") << _S("no such thread", tid);
                return;
            }

            CRPC_DEBUG << _S("conn_id", conn_id) << _S("type", "response") << _S("req_id", req_id) << _S("tid", tid);

            iter->second->append_resp(req_id, std::move(data));

            auto it = reqid_map_.find(req_id);
            if (it != reqid_map_.end()){
                auto it_conn_id = conn_id_map_.find(it->second.first);
                if (it_conn_id != conn_id_map_.end()){
                    auto it_tid = it_conn_id->second.find(it->second.second);
                    if (it_tid != it_conn_id->second.end()){
                        it_tid->second.erase(req_id);
                    }
                }

                reqid_map_.erase(it);
            }
        }
        else{
            string service;
            if (dg.get_string(service_index_, service)){
                SOCE_ERROR << _S("conn_id", conn_id) << _S("type", "request") << _D("protocol error");
                return;
            }

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
                    it_dt->second->append_resp(reqid, std::move(string()));
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

    int Processor::send_req(uint64_t conn_id, const char* data, size_t len)
    {
        char buff[4];
        for (size_t i =0; i<4; ++i){
            buff[i] = (len >> (3-i)*8) & 0xff;
        }

        if (transport_->send(conn_id, buff, 4)
            || transport_->send(conn_id, data, len)){
            return -1;
        }

        return 0;
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

        soce::utils::DQVector<RequestOut::QueueData> reqs;
        if (req_out_->try_consume(reqs)){
            return;
        }

        for (auto& i : reqs){
            std::vector<uint64_t>& conn_ids = i.conn_ids_;
            int32_t& tid = i.tid_;
            int64_t& req_id = i.req_id_;
            std::string& data = i.data_;
            bool& p2p = i.p2p_;

            if (p2p){
                if (conn_ids.size() != 1
                    || conn_ids.front() == 0
                    || send_req(conn_ids.front(), data.c_str(), data.size())){
                    auto iter = dispatch_threads_.find(tid);
                    if (iter != dispatch_threads_.end()){
                        iter->second->append_resp(req_id, std::move(string()));
                    }
                }
                else{
                    uint64_t conn_id = conn_ids.front();
                    CRPC_DEBUG << _S("reqid", req_id) << _S("tid", tid) << _S("conn_id", conn_id);

                    conn_id_map_[conn_id][tid].insert(req_id);
                    reqid_map_[req_id] = std::make_pair(conn_id, tid);
                }
            }
            else{
                for (auto conn_id : conn_ids){
                    send_req(conn_id, data.c_str(), data.size());
                }
            }
        }
    }

    void Processor::handle_response_out(int fd)
    {
        (void) fd;

        soce::utils::DQVector<ResponseOut::QueueData> resps;
        if (resp_out_->try_consume(resps)){
            return;
        }

        for (auto& i : resps){
            char buff[4];
            size_t len = i.data_.size();
            for (size_t i =0; i<4; ++i){
                buff[i] = (len >> (3-i)*8) & 0xff;
            }
            transport_->send(i.conn_id_, buff, 4);
            transport_->send(i.conn_id_, i.data_.c_str(), i.data_.size());
        }
    }

    int Processor::run()
    {
        size_t cores = 0;
        if (!thread_shared_services_.empty()){
            cores = shared_threads_;
            if (cores == 0){
                cores = (size_t)get_nprocs();
            }
        }

        size_t thread_num = cores + thread_per_services_.size();
        req_out_.reset(new RequestOut(thread_num));
        resp_out_.reset(new ResponseOut(thread_num));
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

        SAsyncLogger.start();
        watch_request_out();
        watch_response_out();
        register_to_ns();

        return 0;
    }

    int Processor::run_thread_shared_service(size_t cores, int& tid)
    {
        if (thread_shared_services_.empty()){
            return 0;
        }

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
