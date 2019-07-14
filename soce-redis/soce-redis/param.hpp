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

#ifndef _SOCE_PARAM_8DMRMTUOO1_H_
#define _SOCE_PARAM_8DMRMTUOO1_H_

#include <string>
#include <vector>
#include <sstream>

namespace soce{
namespace redis{

    class Param
    {
    public:
        template <class T>
        Param& operator<< (T val)
            {
                std::ostringstream oss;
                oss << val;
                params_.push_back(std::move(oss.str()));
                return *this;
            }

        inline const std::string& get_key()
            {
                return (params_.size() < 2) ? empty_ : params_[1];
            }

        inline const std::vector<std::string>& get_params()
            {
                return params_;
            }

        inline void clear()
            {
                params_.clear();
            }

    private:
        std::vector<std::string> params_;
        std::string empty_;
    };

}} //end namespace
#endif
