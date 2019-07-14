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

#include "proto/dynamic-getter.h"
#include "dynamic-getter-impl.h"

namespace soce {
namespace proto {

    DynamicGetter::DynamicGetter()
        : impl_(new DynamicGetterImpl) {}

    int DynamicGetter::init(char* data, size_t len)
    {
        return impl_->init(data, len);
    }

    int DynamicGetter::get_bool(const std::vector<uint32_t>& indexes, bool& val)
    {
        return impl_->get_bool(indexes, val);
    }

    int DynamicGetter::get_int32(const std::vector<uint32_t>& indexes, int32_t& val)
    {
        return impl_->get_int32(indexes, val);
    }

    int DynamicGetter::get_int64(const std::vector<uint32_t>& indexes, int64_t& val)
    {
        return impl_->get_int64(indexes, val);
    }

    int DynamicGetter::get_float(const std::vector<uint32_t>& indexes, float& val)
    {
        return impl_->get_float(indexes, val);
    }

    int DynamicGetter::get_double(const std::vector<uint32_t>& indexes, double& val)
    {
        return impl_->get_double(indexes, val);
    }

    int DynamicGetter::get_string(const std::vector<uint32_t>& indexes, std::string& val)
    {
        return impl_->get_string(indexes, val);
    }

    int DynamicGetter::get_bytes(const std::vector<uint32_t>& indexes, char* val, uint32_t expect_len)
    {
        return impl_->get_bytes(indexes, val, expect_len);
    }

    int DynamicGetter::get_enum(const std::vector<uint32_t>& indexes, int32_t& val)
    {
        return impl_->get_enum(indexes, val);
    }

    int DynamicGetter::get_bool(const std::string& path, bool& val, bool delay)
    {
        return impl_->get_bool(path, val, delay);
    }

    int DynamicGetter::get_int32(const std::string& path, int32_t& val, bool delay)
    {
        return impl_->get_int32(path, val, delay);
    }

    int DynamicGetter::get_int64(const std::string& path, int64_t& val, bool delay)
    {
        return impl_->get_int64(path, val, delay);
    }

    int DynamicGetter::get_float(const std::string& path, float& val, bool delay)
    {
        return impl_->get_float(path, val, delay);
    }

    int DynamicGetter::get_double(const std::string& path, double& val, bool delay)
    {
        return impl_->get_double(path, val, delay);
    }

    int DynamicGetter::get_string(const std::string& path, std::string& val, bool delay)
    {
        return impl_->get_string(path, val, delay);
    }

    int DynamicGetter::get_bytes(const std::string& path, char* val, uint32_t expect_len, bool delay)
    {
        return impl_->get_bytes(path, val, expect_len, delay);
    }

    int DynamicGetter::get_enum(const std::string& path, int32_t& val, bool delay)
    {
        return impl_->get_enum(path, val, delay);
    }

    int DynamicGetter::do_get()
    {
        return impl_->do_get();
    }

} // namespace proto
} // namespace soce
