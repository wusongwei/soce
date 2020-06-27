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

#include <sys/types.h>
#include <sys/stat.h>
#include "code-generator.h"
#include "log4rel/logger.h"

namespace soce{
namespace compiler{

    int CodeGenerator::make_dir(const std::string& dir)
    {
        struct stat s_buf;
        stat(dir.c_str(), &s_buf);
        if (!S_ISDIR(s_buf.st_mode)){
            if (mkdir(dir.c_str(), 0755)){
                SOCE_ERROR << _S("mkdir", dir);
                return -1;
            }
        }

        return 0;
    }

} // end compiler
} // end soce
