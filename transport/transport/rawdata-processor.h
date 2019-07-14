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

#ifndef _RAWDATA_PROCESSOR_maH3XuJwWt_H_
#define _RAWDATA_PROCESSOR_maH3XuJwWt_H_

#include <functional>
#include "processor-if.h"

namespace soce{
namespace transport{

    class RawDataProcessor : public ProcessorIf
    {
    public:
        virtual size_t get_max_header_size()
        {
            return buffer_size_;
        }

        virtual size_t get_full_pkt_size(const char* data, size_t len)
        {
            (void) data;
            return len;
        }

    protected:
        uint32_t buffer_size_ = 1024;
    };

} // namespace transport
} // namespace soce

#endif
