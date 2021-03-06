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

#ifndef _SOCE_CRPC_NAME_SERVER_ZK_PedhQsEiPr_H_
#define _SOCE_CRPC_NAME_SERVER_ZK_PedhQsEiPr_H_

#include <unordered_map>
#include <mutex>
#include <thread>
#include <vector>
#include "name-server.h"
#include "utils/cond-expr.h"
#include "utils/rwlock.hpp"
#include "soce-zk/soce-zk.h"
#include "gen-cpp/server-attr.h"
#include "utils/index-list.hpp"

namespace soce{
namespace crpc{

    class CondTree;

    class ServerRepo
    {
    public:
        using Methods = struct{
            std::unordered_map<std::string, soce::utils::IndexList<uint64_t, uint64_t>> uncond_methods;
            std::unordered_map<std::string, soce::utils::IndexList<uint64_t, soce::utils::CondExpr>> cond_methods;
        };

    public:
        ServerRepo();
        ~ServerRepo() = default;

        void add_server(uint64_t conn_id, const soce::crpc::attr::ServerAttrs& server_attrs);
        void del_server(uint64_t conn_id);
        uint64_t get_server(const std::string& service,
                            const std::string& method,
                            const std::string& request);
        uint64_t get_server(const std::string& service,
                            const std::string& method,
                            const std::string& request,
                            uint64_t conn_id);
        int get_cond_servers(const std::string& service,
                             const std::string& method,
                             const std::string& request,
                             std::set<uint64_t>& conn_ids);
        int get_all_servers(const std::string& service,
                            const std::string& method,
                            std::set<uint64_t>& conn_ids);

    private:
        uint64_t get_cond_server(const std::string& service,
                                 const std::string& method,
                                 Methods& methods,
                                 const std::string& request);
        uint64_t get_uncond_server(const std::string& method, Methods& methods);
        std::shared_ptr<CondTree> get_cond_tree(const std::string& service,
                                                const std::string& method);
        std::shared_ptr<CondTree> get_tl_cond_tree(const std::string& service,
                                                   const std::string& method);

    private:
        std::unordered_map<std::string, Methods> servers_;
        std::unordered_map<int, std::unordered_map<std::string, std::vector<std::string>>> server_attrs_;
        std::unordered_map<std::string,
            std::unordered_map<std::string,
            std::shared_ptr<CondTree>>> cond_trees_;
        std::unordered_map<std::thread::id,
            std::unordered_map<std::string,
            std::unordered_map<std::string,
            std::shared_ptr<CondTree>>>> tl_cond_trees_;
        std::mutex mtx_;
    };


    class NameServerZk : public NameServerIf
    {
    public:
        int init(const std::string& zk_addr, uint32_t timeout);
        void set_service_dir(const std::string& service_dir);
        virtual void watch_all();
        virtual void on_connected(uint64_t conn_id);
        virtual int add_server(const std::string& service,
                               const std::string& value);
        virtual void del_server(uint64_t conn_id);
        virtual uint64_t get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request);
        virtual uint64_t get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    const std::string& target_addr);
        virtual int get_cond_servers(const std::string& service,
                                     const std::string& method,
                                     const std::string& request,
                                     std::set<uint64_t>& conn_ids);
        virtual int get_all_servers(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    std::set<uint64_t>& conn_ids);
        virtual void do_watch_service(const std::string& service);
        virtual void run();
        void update(int fd);

    private:
        void resolve_servers(std::vector<std::pair<std::string, std::string>>& servers);
        void reconnect();

    private:
        using ConnStatus = enum{
            kConnStatusBroken,
            kConnStatusConencting,
            kConnStatusConencted
        };
        using ServerStatus = struct {
            bool conn_with_ns;
            ConnStatus status;
            soce::crpc::attr::ServerAttrs attrs;
        };

    private:
        std::string zk_addr_;
        soce::zookeeper::SoceZk soce_zk_;
        std::string service_dir_ = "/soce/service";
        int watch_fd_ = 0;
        ServerRepo server_repo_;
        soce::utils::RWLock lock_;
        std::unordered_map<uint64_t, std::string> connid2addr_;
        std::unordered_map<std::string, uint64_t> addr2connid_;
        std::unordered_map<uint64_t, std::string> connecting_;
        std::unordered_map<std::string, ServerStatus> server_status_;
        std::unordered_set<std::string> broken_servers_;
        soce::transport::TransportIf::tmid_t reconn_timer_ = 0;
        std::vector<uint32_t> service_index_{0, 1};
        std::vector<uint32_t> method_index_{0, 2};
    };

} // namespace crpc
} // namespace soce


#endif
