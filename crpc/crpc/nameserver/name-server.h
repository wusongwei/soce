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

#ifndef _SOCE_CRPC_NAME_SERVER_UmdZhtsl3m_H_
#define _SOCE_CRPC_NAME_SERVER_UmdZhtsl3m_H_

#include <string>
#include <unordered_set>
#include <set>
#include "transport/transport-if.h"
#include "transport/processor-if.h"

namespace soce{
namespace crpc{

    class NameServerIf
    {
    public:
        inline void set_server_addr(const std::string& server_addr) {server_addr_ = server_addr;}
        inline std::string get_server_addr() {return server_addr_;}
        void set_processor(std::shared_ptr<soce::transport::ProcessorIf> processor);
        void watch_service(const std::string& service);
        inline void set_reconnect_time(size_t tm_ms) {
            reconnect_time_ = tm_ms;
        }

        virtual void watch_all() = 0;
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
                                     std::set<uint64_t>& conn_ids) = 0;
        virtual int get_all_servers(const std::string& service,
                                    const std::string& method,
                                    const std::string& request,
                                    std::set<uint64_t>& conn_ids) = 0;
        virtual void do_watch_service(const std::string& service) = 0;
        virtual void run() = 0;

    protected:
        soce::transport::TransportIf* transport_ = nullptr;
        std::shared_ptr<soce::transport::ProcessorIf> processor_;
        std::string server_addr_;
        std::unordered_set<std::string> watched_services_;
        size_t reconnect_time_ = 1000;
    };


} // namespace crpc
} // namespace soce

#endif
