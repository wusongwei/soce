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

#ifndef _SOCE_INTERCEPTOR_KHLaTuHnQf_H_
#define _SOCE_INTERCEPTOR_KHLaTuHnQf_H_

#include <string>
#include <map>
#include <functional>
#include "gen-cpp/crpc-header.h"
#include "proto/binary-proto.h"
#include "utils/singleton.hpp"
#include "utils/rwlock.hpp"

namespace soce{
namespace crpc{

    class Interceptor
    {
    public:
        using BeforeFunc = std::function<int(CrpcReqHeader&, CrpcErrorResp&)>;
        using AfterFunc = std::function<int(CrpcRespHeader&, CrpcErrorResp&)>;

    public:
        int add_before_itcptor(const std::string& name,
                               int priority,
                               BeforeFunc func,
                               const std::string& service,
                               const std::string& method,
                               bool client);
        int add_after_itcptor(const std::string& name,
                              int priority,
                              AfterFunc func,
                              const std::string& service,
                              const std::string& method,
                              bool client);
        std::string do_before_itcptor(CrpcReqHeader& req_header,
                                      const std::string& service,
                                      const std::string& method,
                                      bool client);
        std::string do_after_itcptor(CrpcRespHeader& req_header,
                                     const std::string& service,
                                     const std::string& method,
                                     bool client);
        void remove_itcptor(const std::string& name);

    private:
        template<class Func, class Header>
            class InterceptorDetail
        {
        public:
            void add_itcptor(const std::string& name,
                             int priority,
                             Func func,
                             const std::string& service,
                             const std::string& method)
            {
                if (service.empty()){
                    if (method.empty()){
                        add_itcptor(global_itcptors_, name, priority, func);
                    }
                }
                else{
                    auto& svr_itcptors = service_itcptors_[service];
                    if (method.empty()){
                        add_itcptor(svr_itcptors.service_itcptors, name, priority, func);
                    }
                    else{
                        auto& mtd_itcptors = svr_itcptors.method_itcptors[method];
                        add_itcptor(mtd_itcptors, name, priority, func);
                    }
                }
            }

            std::string do_itcptors(Header& header,
                                    const std::string& service,
                                    const std::string& method)
            {
                string rc;
                CrpcErrorResp err_resp;
                soce::crpc::CrpcRespHeader& resp_header = err_resp.mutable_header();
                resp_header.mutable_type() = soce::crpc::kRpcTypeResp;
                resp_header.set_req_id(header.get_req_id());
                resp_header.mutable_status() = soce::crpc::kRpcStatusAOPFailed;

                if (do_itcptors(header, service, method, err_resp)){
                    soce::proto::BinaryProto pb;
                    size_t len = err_resp.serialize((soce::proto::ProtoIf*)&pb);
                    if (len != 0){
                        rc.assign(pb.data(), pb.size());
                    }
                    else{
                        // todo
                    }
                }

                return std::move(rc);
            }

            void remove_itcptor(const std::string& name,
                                int priority,
                                const std::string& service,
                                const std::string& method)
            {
                if (service.empty()){
                    if (method.empty()){
                        remove_itcptor(global_itcptors_, name, priority);
                    }
                }
                else{
                    auto& svr_itcptors = service_itcptors_[service];
                    if (method.empty()){
                        remove_itcptor(svr_itcptors.service_itcptors, name, priority);
                    }
                    else{
                        auto& mtd_itcptors = svr_itcptors.method_itcptors[method];
                        remove_itcptor(mtd_itcptors, name, priority);
                    }
                }
            }

        private:
            using ConflictInterceptors = struct{
                int index = 0;
                std::map<int, Func> itcptors;
            };

            using InterceptorRepo = std::map<int, ConflictInterceptors>;
            using GlobalItcptor = InterceptorRepo;
            using ServiceInterceptor = struct{
                InterceptorRepo service_itcptors;
                std::unordered_map<std::string, InterceptorRepo> method_itcptors;
            };

        private:
            void add_itcptor(InterceptorRepo& itcptor, const std::string& name, int priority, Func func)
            {
                int index = ++itcptor[priority].index;
                itcptor[priority].itcptors[index] = func;
                name_indexes_[name] = index;
            }

            int do_itcptors(Header& header,
                            const std::string& service,
                            const std::string& method,
                            CrpcErrorResp& err_resp)
            {
                if (do_itcptors(global_itcptors_, header, err_resp)){
                    return -1;
                }

                auto iter = service_itcptors_.find(service);
                if (iter != service_itcptors_.end()){
                    if (do_itcptors(iter->second.service_itcptors, header, err_resp)){
                        return -1;
                    }

                    auto it_md = iter->second.method_itcptors.find(method);
                    if (it_md != iter->second.method_itcptors.end()){
                        if (do_itcptors(it_md->second, header, err_resp)){
                            return -1;
                        }
                    }
                }

                return 0;
            }

            int do_itcptors(InterceptorRepo& itcptor, Header& header, CrpcErrorResp& err_resp)
            {
                for (auto& i : itcptor) {
                    for (auto& j : i.second.itcptors){
                        if (j.second(header, err_resp)){
                            return -1;
                        }
                    }
                }

                return 0;
            }

            void remove_itcptor(InterceptorRepo& itcptor, const std::string& name, int priority)
            {
                auto iter = name_indexes_.find(name);
                if (iter == name_indexes_.end()){
                    return;
                }

                int index = iter->second;

                auto iter_detail = itcptor.find(priority);
                if (iter_detail != itcptor.end()){
                    iter_detail->second.itcptors.erase(index);
                    if (iter_detail->second.itcptors.empty()){
                        itcptor.erase(iter_detail);
                    }
                }

                name_indexes_.erase(iter);
            }

        private:
            InterceptorRepo global_itcptors_;
            std::unordered_map<std::string, ServiceInterceptor> service_itcptors_;
            std::unordered_map<std::string, int> name_indexes_;
        };

        using ItcptorInfo = struct{
            int priority;
            std::string service;
            std::string method;
            bool before;
            bool client;
        };

        InterceptorDetail<BeforeFunc, CrpcReqHeader> cli_bef_itcptors_;
        InterceptorDetail<AfterFunc, CrpcRespHeader> cli_aft_itcptors_;
        InterceptorDetail<BeforeFunc, CrpcReqHeader> svr_bef_itcptors_;
        InterceptorDetail<AfterFunc, CrpcRespHeader> svr_aft_itcptors_;

        std::unordered_map<std::string, ItcptorInfo> itcptor_infos_;
        soce::utils::RWLock rwlock_;
    };

#define SInterceptor soce::utils::SoceSingleton<soce::crpc::Interceptor>::get_instance()

} // namespace crpc
} // namespace soce

#endif
