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

#ifndef _SOCE_SERVER_BKreRdGEbT_H_
#define _SOCE_SERVER_BKreRdGEbT_H_

#include <string>
#include <memory>
#include "factory.h"
#include "processor.h"
#include "name-server.h"
#include "utils/snowflake.h"

namespace soce{
namespace crpc{

    class Server{
    public:
        Server(const std::string& addr,
               std::shared_ptr<TransportFactoryIf> transport_factory,
               std::shared_ptr<ProcessorFactoryIf> processor_factory,
               std::shared_ptr<NameServerFactoryIf> nameserver_factory,
               std::shared_ptr<soce::utils::Snowflake> snowflake = std::shared_ptr<soce::utils::Snowflake>(new soce::utils::Snowflake));

        int run();

    private:
        std::string addr_;
        std::shared_ptr<TransportFactoryIf> transport_factory_;
        std::shared_ptr<ProcessorFactoryIf> processor_factory_;
        std::shared_ptr<NameServerFactoryIf> nameserver_factory_;

        std::shared_ptr<soce::transport::TransportIf> transport_;
        std::shared_ptr<soce::crpc::Processor> processor_;
        std::shared_ptr<NameServerIf> nameserver_;
        std::shared_ptr<soce::utils::Snowflake> snowflake_;
    };

} // namespace crpc
} // namespace soce

#endif
