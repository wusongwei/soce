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

#ifndef _CONSOLE_LOGGER_Ki40c5lHNn_H_
#define _CONSOLE_LOGGER_Ki40c5lHNn_H_

#include <iostream>
#include <string>
#include "sink-if.h"

namespace soce{
namespace log4rel{

    class ConsoleSink : public ISink
    {
    public:
        virtual void log(const char* data, size_t len) {
            std::cout << std::string(data, len);
        }
    };

} // namespace log4rel
} // namespace soce
#endif
