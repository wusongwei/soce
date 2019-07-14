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

#ifndef _EVTFD_PROCESSOR_gMVdfBAnzg_H_
#define _EVTFD_PROCESSOR_gMVdfBAnzg_H_

#include <functional>
#include "processor-if.h"

namespace soce{
namespace transport{

    class EvtfdProcessor : public ProcessorIf
    {
    public:
    EvtfdProcessor(std::function<void(int)> callback)
        : callback_(callback){}

        virtual size_t get_max_header_size()
        {
            return 8;
        }

        virtual size_t get_full_pkt_size(const char* data, size_t len)
        {
            return (len == 8) ? 8 : IMCOMPLETED_PACKET_SIZE;
        }

        virtual void process(uint64_t conn_id, std::string&& data)
        {
            (void) data;

            callback_((int)conn_id);
        }

        virtual void on_error(uint64_t conn_id)
        {
            (void) conn_id;
        }

    protected:
        std::function<void(int)> callback_;
    };

} // namespace transport
} // namespace soce

#endif
