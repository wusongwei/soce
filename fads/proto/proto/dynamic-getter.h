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

#ifndef _SOCE_DYNAMIC_GETTER_kOB3KUgC4W_H_
#define _SOCE_DYNAMIC_GETTER_kOB3KUgC4W_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <vector>

namespace soce {
namespace proto{

    class DynamicGetterImpl;

    class DynamicGetter
    {
    public:
        DynamicGetter();
        ~DynamicGetter() = default;

        int init(char* data, size_t len);

        int get_bool(const std::vector<uint32_t>& indexes, bool& val);
        int get_int32(const std::vector<uint32_t>& indexes, int32_t& val);
        int get_int64(const std::vector<uint32_t>& indexes, int64_t& val);
        int get_float(const std::vector<uint32_t>& indexes, float& val);
        int get_double(const std::vector<uint32_t>& indexes, double& val);
        int get_string(const std::vector<uint32_t>& indexes, std::string& val);
        int get_bytes(const std::vector<uint32_t>& indexes, char* val, uint32_t expect_len);
        int get_enum(const std::vector<uint32_t>& indexes, int32_t& val);

        int get_bool(const std::string& path, bool& val, bool delay = false);
        int get_int32(const std::string& path, int32_t& val, bool delay = false);
        int get_int64(const std::string& path, int64_t& val, bool delay = false);
        int get_float(const std::string& path, float& val, bool delay = false);
        int get_double(const std::string& path, double& val, bool delay = false);
        int get_string(const std::string& path, std::string& val, bool delay = false);
        int get_bytes(const std::string& path, char* val, uint32_t expect_len, bool delay = false);
        int get_enum(const std::string& path, int32_t& val, bool delay = false);
        int do_get();

    private:
        std::shared_ptr<DynamicGetterImpl> impl_;
    };

} // namespace proto
} // namespace soce

#endif
