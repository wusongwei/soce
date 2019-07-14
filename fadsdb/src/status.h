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

#ifndef _SOCE_FADS_DB_ERRCODE_9Q5n7Yba4o_H_
#define _SOCE_FADS_DB_ERRCODE_9Q5n7Yba4o_H_

namespace soce{
namespace fadsdb{

    enum Status{
        kErrInternal      = -8,
        kErrDoCmd         = -7,
        kErrInvalidField  = -6,
        kErrInvalidCmd    = -5,
        kErrKeyNotExist   = -4,
        kErrTableNotExist = -3,
        kErrCrt           = -2,
        kErrProto         = -1,
        kOk               = 0,
        kDupKey           = 1,
        kFilteredOut      = 2,
    };
}
}

#endif
