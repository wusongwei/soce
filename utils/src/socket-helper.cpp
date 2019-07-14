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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include "utils/socket-helper.h"

namespace soce{
namespace utils{

    int SocketHelper::get_socket_name(int fd, std::string& name)
    {
        struct sockaddr_in sa;
        int len = sizeof(sa);
        if(getsockname(fd, (struct sockaddr *)&sa, (socklen_t*)&len)){
            return -1;
        }

        std::ostringstream oss;
        oss << inet_ntoa(sa.sin_addr) << ":" << ntohs(sa.sin_port);
        name = oss.str();
        return 0;
    }

    int SocketHelper::get_peer_name(int fd, std::string& name)
    {
        struct sockaddr_in sa;
        int len = sizeof(sa);
        if(getpeername(fd, (struct sockaddr *)&sa, (socklen_t*)&len)){
            return -1;
        }

        std::ostringstream oss;
        oss << inet_ntoa(sa.sin_addr) << ":" << ntohs(sa.sin_port);
        name = oss.str();
        return 0;
    }

}
}
