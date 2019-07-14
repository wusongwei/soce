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

#ifndef _SOCE_OPTION_MGR_fkcxRCxoCV_H_
#define _SOCE_OPTION_MGR_fkcxRCxoCV_H_

#include <string>

#include "utils/nocopyable.hpp"
#include "utils/singleton.hpp"
#include "utils/str-helper.h"

namespace soce{
namespace compiler{

    class OptionMgr : public soce::utils::NoCopyable
    {
    public:
        OptionMgr() = default;
        ~OptionMgr() = default;

        inline void set_namespace(const std::string& ns) {namespace_ = ns;}
        inline const std::string& get_namespace() const {return namespace_;}
        inline void set_output_path(const std::string& output_path) {output_path_ = output_path;}
        inline const std::string& get_output_path() const {return output_path_;}
        inline void set_lang(const std::string& lang) {lang_ = soce::utils::StrHelper::tolower(lang);}
        inline const std::string& get_lang() const {return lang_;}

    private:
        std::string namespace_ = "fads";
        std::string output_path_ = ".";
        std::string lang_ = "cpp";
    };

} // namespace compiler
} // namespace soce

#endif
