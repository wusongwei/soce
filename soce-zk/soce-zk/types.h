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

#ifndef _SOCE_TYPES_BhhKZsfFml_H_
#define _SOCE_TYPES_BhhKZsfFml_H_

#include <string>
#include <vector>
#include <utility>

using std::string;
using std::vector;
using std::pair;

namespace soce{
namespace zookeeper{

    typedef enum{
        kTypeExist,
        kTypeGet,
        kTypeGetCld,
        kTypeGetCldCnt
    }NotifyType;

    typedef struct{
        NotifyType type;
        string path;

        /* for kTypeExist */
        bool exist;

        /* for kTypeGet */
        string value;

        /* for kTypeGetCld */
        vector<string> cld;

        /* for kTypeGetCldCnt */
        vector<pair<string, string>> cldcnt;
    }NotifyInfo;

    typedef enum{
        kCrtModePresistent,
        kCrtModeEphemeral,
        kCrtModeSequence,
        kCrtModeEphSeq
    }CreateMode;

    typedef enum{
        kWatchModeNone,
        kWatchModeOnce,
        kWatchModeRepeat
    }WatchMode;

    /* todo : Create Conflict */
    typedef enum{
        kCfStgReportErr,
        kCfStgRetain,
        kCfStgOverlap
    }ConflictStrategy;


}} // end namespace
#endif
