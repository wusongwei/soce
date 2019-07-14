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

#ifndef _SOCE_NAME_SERVER_UmdZhtsl3m_H_
#define _SOCE_NAME_SERVER_UmdZhtsl3m_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <thread>
#include "gen-cpp/server-attr.h"
#include "transport/transport-if.h"
#include "transport/processor-if.h"
#include "utils/index-list.hpp"
#include "utils/cond-expr.h"
#include "utils/rwlock.hpp"
#include "soce-zk/soce-zk.h"

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
                             std::vector<uint64_t>& conn_ids);
        int get_all_servers(const std::string& service,
                            const std::string& method,
                            std::vector<uint64_t>& conn_ids);

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

    class NameServerIf
    {
    public:
        inline void set_server_addr(const std::string& server_addr) {server_addr_ = server_addr;}
        inline std::string get_server_addr() {return server_addr_;}
        void set_processor(std::shared_ptr<soce::transport::ProcessorIf> processor);
        void watch_service(const std::string& service);
        virtual void on_connected(uint64_t conn_id) = 0;
        virtual int add_server(const std::string& service,
                               const std::string& value) = 0;
        virtual void del_server(uint64_t conn_id) = 0;
        virtual uint64_t get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request) = 0;
        virtual uint64_t get_server(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    const std::string& target_addr) = 0;
        virtual int get_cond_servers(const std::string& service,
                                     const std::string& method,
                                     const std::string& request,
                                     std::vector<uint64_t>& conn_ids) = 0;
        virtual int get_all_servers(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    std::vector<uint64_t>& conn_ids) = 0;
        virtual void do_watch_service(const std::string& service) = 0;
        virtual void run() = 0;

    protected:
        soce::transport::TransportIf* transport_ = nullptr;
        std::shared_ptr<soce::transport::ProcessorIf> processor_;
        std::string server_addr_;
        std::unordered_set<std::string> watched_services_;
    };

    class NameServerZk : public NameServerIf
    {
    public:
        int init(const std::string& zk_addr, uint32_t timeout);
        void set_zk_root(const std::string& zk_root);
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
                                     std::vector<uint64_t>& conn_ids);
        virtual int get_all_servers(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    std::vector<uint64_t>& conn_ids);
        virtual void do_watch_service(const std::string& service);
        virtual void run();
        void update(int fd);

    private:
        void resolve_servers(std::vector<std::pair<std::string, std::string>>& servers);

    private:
        using TempSvrInfo = struct{
            std::string addr;
            soce::crpc::attr::ServerAttrs attrs;
        };

    private:
        std::string zk_addr_;
        soce::zookeeper::SoceZk soce_zk_;
        std::string zk_root_ = "/soce";
        int watch_fd_ = 0;
        ServerRepo server_repo_;
        soce::utils::RWLock lock_;
        std::unordered_map<uint64_t, std::string> connid2addr_;
        std::unordered_map<std::string, uint64_t> addr2connid_;
        std::unordered_map<uint64_t, TempSvrInfo> temp_svr_info_;
        std::vector<uint32_t> service_index_{0, 1};
        std::vector<uint32_t> method_index_{0, 2};
    };

} // namespace crpc
} // namespace soce

#endif
