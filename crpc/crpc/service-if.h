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

#ifndef _SOCE_SERVICE_IF_AtlwzPRFYL_H_
#define _SOCE_SERVICE_IF_AtlwzPRFYL_H_

#include <string>
#include <memory>
#include "transport/transport-if.h"

namespace soce{
namespace crpc{

    class ServiceIf
    {
    public:
        using SvrProcType = enum{
            kSvrProcTypeSync = 0,
            kSvrProcTypeAsync,
            kSvrProcTypeCort
        };

    public:
        virtual std::string get_name() = 0;
        virtual std::string process(const std::string& data)  = 0;
        virtual std::string get_service_info() = 0;
        virtual void start(){}
        virtual void end(){}
        virtual SvrProcType get_svr_proc_type() {return kSvrProcTypeCort;}

        inline void set_transport(std::shared_ptr<soce::transport::TransportIf> transport){
            transport_ = transport;
        }
        inline std::shared_ptr<soce::transport::TransportIf> get_transport() {return transport_;}

    protected:
        std::shared_ptr<soce::transport::TransportIf> transport_;
    };

    class ServiceFactoryIf
    {
    public:
        virtual std::shared_ptr<ServiceIf> create() = 0;
    };

} // namespace crpc
} // namespace soce
#endif
