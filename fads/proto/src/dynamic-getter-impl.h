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

#ifndef _SOCE_DYNAMIC_GETTER_IMPL_uCdH46IW45_H_
#define _SOCE_DYNAMIC_GETTER_IMPL_uCdH46IW45_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "proto/proto-if.h"

namespace soce {
namespace proto{

    class DynamicGetterImpl
    {
    public:
        class CompositeIf;
        class CompositeBool;
        class CompositeI32;
        class CompositeI64;
        class CompositeFloat;
        class CompositeDouble;
        class CompositeString;
        class CompositeBytes;
        class CompositeEnum;
        class Component;

    public:
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
        int get(const std::vector<uint32_t>& indexes,
                SoceDataType type,
                std::function<int(ProtoIf*)> getter);
        int get(const std::string& path, std::shared_ptr<CompositeIf> composite, bool delay);
        int add_path(const std::string& path, std::shared_ptr<CompositeIf> composite);
        int do_get(std::shared_ptr<Component> node, soce::proto::ProtoIf* proto);
        int skip_elem(soce::proto::ProtoIf* proto);
        int skip_elems(int n, soce::proto::ProtoIf* proto);

    private:
        char* data_ = NULL;
        uint32_t len_ = 0;
        std::shared_ptr<Component> root_;
    };

    class DynamicGetterImpl::CompositeIf
    {
    public:
        virtual int get_val(soce::proto::ProtoIf* proto) = 0;
    };

    class DynamicGetterImpl::CompositeBool : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeBool(bool& val)
        {
            val_ = &val;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        bool* val_ = NULL;
    };

    class DynamicGetterImpl::CompositeI32 : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeI32(int32_t& val)
        {
            val_ = &val;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        int32_t* val_ = NULL;
    };

    class DynamicGetterImpl::CompositeI64 : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeI64(int64_t& val)
        {
            val_ = &val;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        int64_t* val_ = NULL;
    };

    class DynamicGetterImpl::CompositeFloat : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeFloat(float& val)
        {
            val_ = &val;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        float* val_ = NULL;
    };

    class DynamicGetterImpl::CompositeDouble : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeDouble(double& val)
        {
            val_ = &val;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        double* val_ = NULL;
    };

    class DynamicGetterImpl::CompositeString : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeString(std::string& val)
        {
            val_ = &val;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        std::string* val_ = NULL;
    };

    class DynamicGetterImpl::CompositeBytes : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeBytes(char* val, uint32_t expect_len)
        {
            val_ = val;
            expect_len_ = expect_len;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        char* val_ = NULL;
        uint32_t expect_len_ = 0;
    };

    class DynamicGetterImpl::CompositeEnum : public DynamicGetterImpl::CompositeIf
    {
    public:
        CompositeEnum(int32_t& val)
        {
            val_ = &val;
        }

        virtual int get_val(soce::proto::ProtoIf* proto);

    private:
        int32_t* val_ = NULL;
    };

    class DynamicGetterImpl::Component
    {
    public:
        std::shared_ptr<Component> get_node(int i);
        std::shared_ptr<Component> add_node(int i);

        void set_composite(std::shared_ptr<DynamicGetterImpl::CompositeIf> composite)
        {
            composite_ = composite;
        }

        std::map<int, std::shared_ptr<Component>>& get_nodes()
        {
            return nodes_;
        }

        bool is_leaf()
        {
            return composite_ ? true : false;
        }

        int get_val(soce::proto::ProtoIf* proto)
        {
            if (!is_leaf()){
                return -1;
            }

            return composite_->get_val(proto);
        }

    private:
        std::map<int, std::shared_ptr<Component>> nodes_;
        std::shared_ptr<DynamicGetterImpl::CompositeIf> composite_;
    };

} // namespace proto
} // namespace soce

#endif
