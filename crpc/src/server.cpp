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

#include "crpc/server.h"
#include "utils/socket-helper.h"

using soce::transport::TransportIf;
using soce::transport::ProcessorIf;
using soce::utils::SocketHelper;

namespace soce{
namespace crpc{

    Server::Server(const std::string& addr,
                   std::shared_ptr<TransportFactoryIf> transport_factory,
                   std::shared_ptr<ProcessorFactoryIf> processor_factory,
                   std::shared_ptr<NameServerFactoryIf> nameserver_factory,
                   std::shared_ptr<soce::utils::Snowflake> snowflake)
        : addr_(addr),
          transport_factory_(transport_factory),
          processor_factory_(processor_factory),
          nameserver_factory_(nameserver_factory),
          snowflake_(snowflake)
    {
    }

    int Server::run()
    {
        transport_ = transport_factory_->create();
        if (!transport_){
            return -1;
        }

        processor_ = processor_factory_->create();
        if (!processor_){
            return -1;
        }

        nameserver_ = nameserver_factory_->create();
        if (!nameserver_){
            return -1;
        }

        int fd = 0;
        if (transport_->listen(addr_, processor_, &fd)){
            return -1;
        }
        string server_addr;
        if (SocketHelper::get_socket_name((int)fd, server_addr)){
            return -1;
        }

        processor_->set_transport(transport_.get());
        processor_->set_transport_factory(transport_factory_);
        processor_->set_nameserver(nameserver_);
        processor_->set_snowflake(snowflake_);

        nameserver_->set_server_addr(server_addr);
        nameserver_->set_processor(processor_);

        nameserver_->run();
        processor_->run();
        transport_->run();

        return 0;
    }

} // namespace crpc
} // namespace soce
