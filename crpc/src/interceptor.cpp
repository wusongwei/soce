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

#include "crpc/interceptor.h"

namespace soce{
namespace crpc{
    int Interceptor::add_before_itcptor(const std::string& name,
                                        int priority,
                                        BeforeFunc func,
                                        const std::string& service,
                                        const std::string& method,
                                        bool client)
    {
        rwlock_.write_lock();

        if (itcptor_infos_.find(name) != itcptor_infos_.end()){
            rwlock_.write_unlock();
            return -1;
        }

        if (client){
            cli_bef_itcptors_.add_itcptor(name, priority, func, service, method);
        }
        else{
            svr_bef_itcptors_.add_itcptor(name, priority, func, service, method);
        }

        ItcptorInfo info;
        info.priority = priority;
        info.service = service;
        info.method = method;
        info.before = true;
        info.client = client;
        itcptor_infos_[name] = info;

        rwlock_.write_unlock();
        return 0;
    }

    int Interceptor::add_after_itcptor(const std::string& name,
                                       int priority,
                                       AfterFunc func,
                                       const std::string& service,
                                       const std::string& method,
                                       bool client)
    {
        rwlock_.write_lock();

        if (itcptor_infos_.find(name) != itcptor_infos_.end()){
            rwlock_.write_unlock();
            return -1;
        }

        if (client){
            cli_aft_itcptors_.add_itcptor(name, priority, func, service, method);
        }
        else{
            svr_aft_itcptors_.add_itcptor(name, priority, func, service, method);
        }

        ItcptorInfo info;
        info.priority = priority;
        info.service = service;
        info.method = method;
        info.before = false;
        info.client = client;
        itcptor_infos_[name] = info;

        rwlock_.write_unlock();
        return 0;
    }

    std::string Interceptor::do_before_itcptor(CrpcReqHeader& req_header,
                                               const std::string& service,
                                               const std::string& method,
                                               bool client)
    {
        rwlock_.read_lock();

        std::string rc;
        if (client){
            rc = cli_bef_itcptors_.do_itcptors(req_header, service, method);
        }
        else{
            rc = svr_bef_itcptors_.do_itcptors(req_header, service, method);
        }

        rwlock_.read_unlock();
        return rc;
    }

    std::string Interceptor::do_after_itcptor(CrpcRespHeader& resp_header,
                                              const std::string& service,
                                              const std::string& method,
                                              bool client)
    {
        rwlock_.read_lock();

        std::string rc;
        if (client){
            rc = cli_aft_itcptors_.do_itcptors(resp_header, service, method);
        }
        else{
            rc = svr_aft_itcptors_.do_itcptors(resp_header, service, method);
        }

        rwlock_.read_unlock();
        return rc;
    }

    void Interceptor::remove_itcptor(const std::string& name)
    {
        rwlock_.write_lock();

        auto iter = itcptor_infos_.find(name);
        if (iter == itcptor_infos_.end()){
            rwlock_.write_unlock();
            return;
        }

        ItcptorInfo& info = iter->second;
        if (info.before){
            if (info.client){
                cli_bef_itcptors_.remove_itcptor(name,
                                                 info.priority,
                                                 info.service,
                                                 info.method);
            }
            else{
                cli_bef_itcptors_.remove_itcptor(name,
                                                 info.priority,
                                                 info.service,
                                                 info.method);
            }
        }
        else{
            if (info.client){
                cli_aft_itcptors_.remove_itcptor(name,
                                                 info.priority,
                                                 info.service,
                                                 info.method);
            }
            else{
                cli_aft_itcptors_.remove_itcptor(name,
                                                 info.priority,
                                                 info.service,
                                                 info.method);
            }
        }

        itcptor_infos_.erase(iter);
        rwlock_.write_unlock();
    }

} // namespace crpc
} // namespace soce
