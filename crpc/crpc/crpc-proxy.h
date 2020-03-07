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

#ifndef _SOCE_CRPC_CRPC_PROXY_R2Nq0j9q4A_H_
#define _SOCE_CRPC_CRPC_PROXY_R2Nq0j9q4A_H_

#include <memory>

namespace soce {
namespace crpc {

    class CrpcProxy
    {
    public:
        CrpcProxy(const std::string& ctl_addr,
                  const std::string& local_req_addr,
                  const std::string& remote_req_addr,
                  const std::string& ns_addr,
                  size_t ns_timeout,
                  const std::string& service_dir,
                  size_t shared_thread_num);

        void start();

    private:
        std::string ctl_addr_;
        std::string local_req_addr_;
        std::string remote_req_addr_;
        std::string ns_addr_;
        size_t ns_timeout_ = 0;
        std::string service_dir_;
        size_t shared_thread_num_ = 0;
    };

    class CrpcProxyBuilder
    {
    public:
        CrpcProxyBuilder& set_ctl_addr(const std::string& addr);
        CrpcProxyBuilder& set_local_req_addr(const std::string& addr);
        CrpcProxyBuilder& set_remote_req_addr(const std::string& addr);
        CrpcProxyBuilder& set_ns_addr(const std::string& addr);
        CrpcProxyBuilder& set_ns_timeout(size_t timeout);
        CrpcProxyBuilder& set_service_dir(const std::string& dir);
        CrpcProxyBuilder& set_shared_thread_num(size_t num);
        std::shared_ptr<CrpcProxy> build();

    private:
        std::string ctl_addr_ = "0.0.0.0:9020";
        std::string local_req_addr_ = "0.0.0.0:9021";
        std::string remote_req_addr_;
        std::string ns_addr_;
        size_t ns_timeout_ = 4000;
        std::string service_dir_ = "/soce/service";
        size_t shared_thread_num_ = 0;
    };

} // namespace crpc
} // namespace soce
#endif
