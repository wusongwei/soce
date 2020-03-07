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

#ifndef _SOCE_CRPC_NAME_SERVER_EHBdOZZCG7_H_
#define _SOCE_CRPC_NAME_SERVER_EHBdOZZCG7_H_

#include "name-server.h"

namespace soce{
namespace crpc{

    class NameServerProxy : public NameServerIf
    {
    public:
        NameServerProxy();
        NameServerProxy(const std::string& ctl_addr, const std::string& req_addr);
        void set_ctl_addr(const std::string& ctl_addr);
        void set_req_addr(const std::string& req_addr);
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

    private:
        void send_server_info();
        void send_register_info(const std::string& service, const std::string& filters);
        void send_watched_service(const std::string& service);
        void send_msg(const char* data, size_t size);

    private:
        std::string ctl_addr_ = "0.0.0.0:9020";
        std::string req_addr_ = "0.0.0.0:9021";
        uint64_t ctl_fd_ = 0;
        uint64_t req_fd_ = 0;
    };


} // namespace crpc
} // namespace soce


#endif
