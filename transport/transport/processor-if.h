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

#ifndef _SOCE_PROCESSOR_IF_0xNKImlQRp_H_
#define _SOCE_PROCESSOR_IF_0xNKImlQRp_H_

#include <string>
#include <stdint.h>
#include <memory>

#define IMCOMPLETED_PACKET_SIZE ((size_t)-1)

namespace soce{
namespace transport{

    class TransportIf;

    class ProcessorIf
    {
    public:
        ProcessorIf() {}
        ProcessorIf(TransportIf* t)
            : transport_(t){}

        /*
         * Get the max size of the packet header
         */
        virtual size_t get_max_header_size() = 0;

        /*
         * Get the size of the full packet include the header
         * from the given data
         *
         * @param data The data received which may just include the header
         * @param len The length of the data reveived
         *
         * @return the data length
         *         IMCOMPLETED_PACKET_SIZE packet is not completed.
         *
         */
        virtual size_t get_full_pkt_size(const char* data, size_t len) = 0;

        /*
         * Process the coming data.
         *
         * @param conn_id The id of the current connection
         * @param data The data of the packet
         */
        virtual void process(uint64_t conn_id, std::string&& data) = 0;

        /*
         * Whether read the next message automatically.
         *
         * @return true  : auto read
         *         false : suspend read before call transport_->enable_read();
         */
        virtual bool auto_read() {return true;}
        /*
         * Event callbacks
         */
        virtual void on_accept(uint64_t conn_id){}
        virtual void on_connected(uint64_t conn_id){}
        virtual void on_read(uint64_t conn_id){}
        virtual void on_error(uint64_t conn_id){}

        inline void set_transport(TransportIf* t) {transport_ = t;}
        inline TransportIf* get_transport() {return transport_;}

    protected:
        TransportIf* transport_ = nullptr;
    };

}} // end namespace
#endif
