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
#include <cassert>
#include "crpc/name-server.h"
#include "transport/evtfd-processor.h"
#include "log4rel/logger.h"
#include "soce-zk/soce-zk.h"
#include "proto/binary-proto.h"
#include "proto/dynamic-getter.h"
#include "cond-tree.h"

using std::string;
using std::vector;
using soce::transport::EvtfdProcessor;

namespace soce{
namespace crpc{

    ServerRepo::ServerRepo()
    {
    }

    void ServerRepo::add_server(uint64_t conn_id, const soce::crpc::attr::ServerAttrs& server_attrs)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        const string& service = server_attrs.get_name();
        for (auto& i : server_attrs.get_methods()){
            const string& method = i.first;

            if (!i.second.has_filter()){
                server_attrs_[conn_id][service].push_back(method);
                servers_[service].uncond_methods[method].push_back(conn_id, conn_id);
            }
            else{
                const string& filter = i.second.get_filter();

                soce::utils::CondExpr cond_expr;
                if (cond_expr.parse(filter.c_str(), filter.size())){
                    continue;
                }

                server_attrs_[conn_id][service].push_back(method);
                servers_[service].cond_methods[method].push_back(conn_id, cond_expr);

                auto cond_tree = get_cond_tree(service, method);
                if (!cond_tree){
                    cond_tree = std::make_shared<CondTree>();
                    cond_trees_[service][method] = cond_tree;
                }
                cond_tree->add_ver();

                for (auto& j : i.second.get_val_type()){
                    cond_tree->add_node(j.first, (soce::proto::SoceDataType)j.second);
                }
            }
        }
    }

    void ServerRepo::del_server(uint64_t conn_id)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        auto iter = server_attrs_.find(conn_id);
        if (iter == server_attrs_.find(conn_id)){
            return;
        }

        for (auto& i : iter->second){
            auto iter = servers_.find(i.first);
            if (iter == servers_.end()){
                continue;
            }

            for (auto& j : i.second){
                auto iter_m = iter->second.cond_methods.find(j);
                if (iter_m != iter->second.cond_methods.end()){
                    iter_m->second.erase(conn_id);
                    if (iter_m->second.empty()){
                        iter->second.cond_methods.erase(iter_m);
                    }
                }
                else{
                    auto iter_m = iter->second.uncond_methods.find(j);
                    if (iter_m != iter->second.uncond_methods.end()){
                        iter_m->second.erase(conn_id);
                        if (iter_m->second.empty()){
                            iter->second.uncond_methods.erase(iter_m);
                        }
                    }
                }
            }

            if (iter->second.cond_methods.empty() && iter->second.uncond_methods.empty()){
                servers_.erase(iter);
            }
        }
    }

    uint64_t ServerRepo::get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        auto iter = servers_.find(service);
        if (iter == servers_.end()){
            return 0;
        }

        uint64_t conn_id = 0;
        conn_id = get_cond_server(service, method, iter->second, request);
        if (conn_id == 0){
            conn_id = get_uncond_server(method, iter->second);
        }

        return conn_id;
    }

    uint64_t ServerRepo::get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    uint64_t conn_id)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        auto it_svr = servers_.find(service);
        if (it_svr == servers_.end()){
            return 0;
        }

        auto& uncond_methods = it_svr->second.uncond_methods;
        auto it_uc_md = uncond_methods.find(method);
        if (it_uc_md != uncond_methods.end()){
            if (it_uc_md->second.find(conn_id)){
                return conn_id;
            }
        }
        else{
            auto& cond_methods = it_svr->second.cond_methods;
            auto it_c_md = cond_methods.find(method);
            if (it_c_md == cond_methods.end()){
                return 0;
            }
            auto cond_tree = get_tl_cond_tree(service, method);
            if (!cond_tree){
                return 0;
            }
            soce::utils::CondExpr* expr = nullptr;
            if (it_c_md->second.get(conn_id, &expr)
                || expr->calc(std::bind([&](const std::string& lvalue,
                                            const std::string& oper,
                                            const std::string& rvalue){
                                            return cond_tree->match(lvalue, oper, rvalue);
                                        },
                                        std::placeholders::_1,
                                        std::placeholders::_2,
                                        std::placeholders::_3))){
                return 0;
            }
        }

        return conn_id;
    }


    int ServerRepo::get_cond_servers(const std::string& service,
                                     const std::string& method,
                                     const std::string& request,
                                     std::set<uint64_t>& conn_ids)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        auto iter = servers_.find(service);
        if (iter == servers_.end()){
            return -1;
        }

        auto cond_tree = get_tl_cond_tree(service, method);
        if (!cond_tree){
            return -1;
        }

        cond_tree->clear();
        soce::proto::BinaryProto bp;
        bp.init(const_cast<char*>(request.c_str()), request.size(), false);
        if (cond_tree->parse((soce::proto::ProtoIf*)&bp)){
            return -1;
        }

        Methods& methods = iter->second;
        auto iter_m = methods.cond_methods.find(method);
        if (iter_m == methods.cond_methods.end()){
            return -1;
        }

        for (auto iter=iter_m->second.begin(); iter!=iter_m->second.end(); ++iter){
            if (iter->second.calc(std::bind([&](const std::string& lvalue,
                                                const std::string& oper,
                                                const std::string& rvalue){
                                                return cond_tree->match(lvalue, oper, rvalue);
                                            },
                                            std::placeholders::_1,
                                            std::placeholders::_2,
                                            std::placeholders::_3))){
                soce::utils::CondExpr* p = NULL;
                iter_m->second.update_to_back(iter->first, &p);
                conn_ids.insert(iter->first);
            }
        }

        return 0;
    }

    int ServerRepo::get_all_servers(const std::string& service,
                                    const std::string& method,
                                    std::set<uint64_t>& conn_ids)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        auto iter = servers_.find(service);
        if (iter == servers_.end()){
            return -1;
        }

        Methods& methods = iter->second;
        auto iter_cm = methods.cond_methods.find(method);
        if (iter_cm != methods.cond_methods.end()){
            for (auto it=iter_cm->second.begin();
                 it!=iter_cm->second.end(); ++it){
                conn_ids.insert(it->first);
            }
        }

        auto iter_ucm = methods.uncond_methods.find(method);
        if (iter_ucm != methods.uncond_methods.end()){
            for (auto it=iter_ucm->second.begin();
                 it!=iter_ucm->second.end(); ++it){
                conn_ids.insert(it->first);
            }
        }

        return 0;
    }

    uint64_t ServerRepo::get_cond_server(const std::string& service,
                                         const std::string& method,
                                         Methods& methods,
                                         const std::string& request)
    {
        auto cond_tree = get_tl_cond_tree(service, method);
        if (!cond_tree){
            return 0;
        }
        cond_tree->clear();

        soce::proto::BinaryProto bp;
        bp.init(const_cast<char*>(request.c_str()), request.size(), false);
        if (cond_tree->parse((soce::proto::ProtoIf*)&bp)){
            return 0;
        }

        auto iter_m = methods.cond_methods.find(method);
        if (iter_m == methods.cond_methods.end()){
            return 0;
        }

        uint64_t conn_id = 0;
        for (auto iter=iter_m->second.begin(); iter!=iter_m->second.end(); ++iter){
            if (iter->second.calc(std::bind([&](const std::string& lvalue,
                                                const std::string& oper,
                                                const std::string& rvalue){
                                                return cond_tree->match(lvalue, oper, rvalue);
                                            },
                                            std::placeholders::_1,
                                            std::placeholders::_2,
                                            std::placeholders::_3))){
                conn_id = iter->first;
                soce::utils::CondExpr* p = NULL;
                iter_m->second.update_to_back(conn_id, &p);
                break;
            }
        }

        return conn_id;
    }

    uint64_t ServerRepo::get_uncond_server(const std::string& method, Methods& methods)
    {
        auto iter = methods.uncond_methods.find(method);
        if (iter == methods.uncond_methods.end()){
            return 0;
        }

        if (iter->second.empty()){
            return 0;
        }

        auto value = iter->second.front();
        uint64_t conn_id = value.second;
        uint64_t* pconn_id = NULL;
        iter->second.update_to_back(conn_id, &pconn_id);

        return conn_id;
    }

    std::shared_ptr<CondTree> ServerRepo::get_cond_tree(const std::string& service,
                                                        const std::string& method)
    {
        std::shared_ptr<CondTree> cond_tree;
        auto iter = cond_trees_.find(service);
        if (iter != cond_trees_.end()){
            auto it = iter->second.find(method);
            if (it != iter->second.end()){
                cond_tree = it->second;
            }
        }

        return cond_tree;
    }

    std::shared_ptr<CondTree> ServerRepo::get_tl_cond_tree(const std::string& service,
                                                           const std::string& method)
    {
        std::shared_ptr<CondTree> cond_tree;

        auto it_ct = cond_trees_.find(service);
        if (it_ct == cond_trees_.end()){
            return cond_tree;
        }

        auto it_ctm = it_ct->second.find(method);
        if (it_ctm == it_ct->second.end()){
            return cond_tree;
        }

        std::thread::id tid = std::this_thread::get_id();
        uint32_t ver = it_ctm->second->get_ver();
        cond_tree = tl_cond_trees_[tid][service][method];

        if (!cond_tree || cond_tree->get_ver() != ver){
            cond_tree = it_ctm->second->clone();
            tl_cond_trees_[tid][service][method] = cond_tree;
        }

        return cond_tree;
    }

    void NameServerIf::set_processor(std::shared_ptr<soce::transport::ProcessorIf> processor)
    {
        processor_ = processor;
        transport_ = processor_->get_transport();

        assert(processor_ && transport_);
    }

    void NameServerIf::watch_service(const std::string& service)
    {
        if (transport_){
            do_watch_service(service);
        }
        else{
            watched_services_.insert(service);
        }
    }

    int NameServerZk::init(const string& zk_addr, uint32_t timeout)
    {
        zk_addr_ = zk_addr;
        if (soce_zk_.init(zk_addr, timeout)){
            return -1;
        }
        watch_fd_ = soce_zk_.get_notify_evtfd();
        return 0;
    }

    void NameServerZk::set_zk_root(const std::string& zk_root)
    {
        zk_root_ = zk_root;
    }

    void NameServerZk::on_connected(uint64_t conn_id)
    {
        auto iter = temp_svr_info_.find(conn_id);
        if (iter == temp_svr_info_.end()){
            return;
        }

        addr2connid_[iter->second.addr] = conn_id;
        server_repo_.add_server(conn_id, iter->second.attrs);
        temp_svr_info_.erase(iter);
    }

    int NameServerZk::add_server(const std::string& service,
                                 const string& value)
    {
        string full_path = zk_root_ + "/" + service + "/" + server_addr_;
        return soce_zk_.create(full_path, value, soce::zookeeper::kCrtModeEphemeral);
    }

    void NameServerZk::del_server(uint64_t conn_id)
    {
        lock_.write_lock();
        temp_svr_info_.erase(conn_id);

        auto iter = connid2addr_.find(conn_id);
        if (iter != connid2addr_.end()){
            addr2connid_.erase(iter->second);
            connid2addr_.erase(iter);
        }

        server_repo_.del_server(conn_id);
        lock_.write_unlock();
    }

    void NameServerZk::do_watch_service(const string& service)
    {
        string full_path = zk_root_ + "/" + service;
        std::vector<std::pair<std::string, std::string>> servers;
        if (soce_zk_.get_children_content(full_path,
                                          soce::zookeeper::kWatchModeRepeat,
                                          servers)){
            return;
        }

        if (!servers.empty()){
            resolve_servers(servers);
        }
    }

    uint64_t NameServerZk::get_server(const std::string& service,
                                      const std::string& method,
                                      const string& request)
    {
        lock_.read_lock();
        uint64_t conn_id = server_repo_.get_server(service, method, request);
        lock_.read_unlock();

        return conn_id;
    }

    uint64_t NameServerZk::get_server(const std::string& service,
                                      const std::string& method,
                                      const string& request,
                                      const std::string& target_addr)
    {
        uint64_t conn_id = 0;

        lock_.read_lock();
        auto iter = addr2connid_.find(target_addr);
        if (iter != addr2connid_.end()){
            conn_id = server_repo_.get_server(service, method, request, iter->second);
        }
        lock_.read_unlock();

        return conn_id;
    }

    int NameServerZk::get_cond_servers(const std::string& service,
                                       const std::string& method,
                                       const string& request,
                                       std::set<uint64_t>& conn_ids)
    {
        lock_.read_lock();
        int rc = server_repo_.get_cond_servers(service, method, request, conn_ids);
        lock_.read_unlock();
        return rc;
    }

    int NameServerZk::get_all_servers(const std::string& service,
                                      const std::string& method,
                                      const std::string& request,
                                      std::set<uint64_t>& conn_ids)
    {
        lock_.read_lock();
        int rc = server_repo_.get_all_servers(service, method, conn_ids);
        lock_.read_unlock();

        return rc;
    }

    void NameServerZk::run()
    {
        transport_->watch(watch_fd_,
                          std::shared_ptr<EvtfdProcessor>(new EvtfdProcessor(
                                                              std::bind(&NameServerZk::update,
                                                                        this,
                                                                        std::placeholders::_1))));

        for (auto& i : watched_services_){
            do_watch_service(i);
        }

        watched_services_.clear();
    }

    void NameServerZk::update(int fd)
    {
        (void) fd;

        vector<soce::zookeeper::NotifyInfo> notifies;
        soce_zk_.get_notifies(notifies);
        for (auto& info : notifies){
            if (info.type != soce::zookeeper::kTypeGetCldCnt){
                return;
            }

            resolve_servers(info.cldcnt);
        }
    }

    void NameServerZk::resolve_servers(std::vector<std::pair<std::string, std::string>>& servers)
    {
        lock_.write_lock();

        std::unordered_map<std::string, uint64_t> invalid_servers = addr2connid_;
        for (auto& i : servers){
            uint64_t conn_id = 0;

            TempSvrInfo tsi;
            soce::proto::BinaryProto bp;
            bp.init(const_cast<char*>(i.second.c_str()), i.second.size(), false);
            if (tsi.attrs.deserialize((soce::proto::ProtoIf*)&bp) == 0){
                continue;
            }

            auto iter = addr2connid_.find(i.first);
            if (iter == addr2connid_.end()){
                if (transport_->connect(i.first, processor_, conn_id)){
                    continue;
                }

                tsi.addr = i.first;
                temp_svr_info_[conn_id] = tsi;
            }
            else{
                conn_id = iter->second;
                server_repo_.add_server(conn_id, tsi.attrs);
            }

            invalid_servers.erase(i.first);
        }

        for (auto& i : invalid_servers){
            auto iter = addr2connid_.find(i.first);
            if (iter != addr2connid_.end()){
                uint64_t conn_id = iter->second;
                connid2addr_.erase(conn_id);
                server_repo_.del_server(conn_id);
                addr2connid_.erase(iter);
            }
        }
        lock_.write_unlock();
    }

} // namespace crpc
} // namespace soce
