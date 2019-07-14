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

#ifndef _SOCE_TYPES_DEF_CPP_D7clbsVQFW_H_
#define _SOCE_TYPES_DEF_CPP_D7clbsVQFW_H_

#include "types-def.h"

namespace soce{
namespace compiler{

    class ReferenceCpp : public ReferenceIf
    {
    public:
        virtual void gen_header(std::ostringstream& oss, const std::set<std::string>& types);
    };

    class TypeEnumCpp : public TypeEnum
    {
    public:
    TypeEnumCpp(std::string&& name, std::vector<std::pair<std::string, std::string>>&& elems)
        : TypeEnum(std::move(name), std::move(elems))
        {
        }

        virtual void gen_header(std::ostringstream& oss, size_t indent);
        virtual void gen_impl(std::ostringstream& oss, size_t indent);
    };

    class TypeListCpp : public TypeList
    {
    public:
    TypeListCpp(std::string&& name, std::string&& elem_type)
        : TypeList(std::move(name), std::move(elem_type))
        {
        }

        virtual void gen_header(std::ostringstream& oss, size_t indent);
        virtual void gen_impl(std::ostringstream& oss, size_t indent);
    };

    class TypeSetCpp : public TypeSet
    {
    public:
    TypeSetCpp(std::string&& name, std::string&& elem_type)
        : TypeSet(std::move(name), std::move(elem_type))
        {
        }

        virtual void gen_header(std::ostringstream& oss, size_t indent);
        virtual void gen_impl(std::ostringstream& oss, size_t indent);
    };

    class TypeMapCpp : public TypeMap
    {
    public:
        TypeMapCpp(std::string&& name, std::string&& key_type, std::string&& val_type)
            : TypeMap(std::move(name), std::move(key_type), std::move(val_type))
        {
        }

        virtual void gen_header(std::ostringstream& oss, size_t indent);
        virtual void gen_impl(std::ostringstream& oss, size_t indent);
    };

    class TypeStructCpp : public TypeStruct
    {
    public:
    TypeStructCpp(std::string&& name)
        : TypeStruct(std::move(name))
        {
        }

        virtual void gen_header(std::ostringstream& oss, size_t indent);
        virtual void gen_impl(std::ostringstream& oss, size_t indent);

    private:
        void gen_write(std::ostringstream& oss, size_t indent);
        void gen_read(std::ostringstream& oss, size_t indent);
    };

    class TypeStructElemPrimeCpp : public TypeStruct::TypeStructElemPrime
    {
    public:
    TypeStructElemPrimeCpp(std::string&& type,
                           std::string&& name,
                           bool required)
        :TypeStruct::TypeStructElemPrime(std::move(type), std::move(name), required)
        {
        }

        virtual void gen_hash_code(std::ostringstream& oss, size_t indent);
        virtual void gen_read(std::ostringstream& oss, size_t indent);
        virtual void gen_write(std::ostringstream& oss, size_t indent);
    };

    class TypeStructElemCustomCpp : public TypeStruct::TypeStructElemCustom
    {
    public:
    TypeStructElemCustomCpp(std::string&& type,
                           std::string&& name,
                           bool required)
        :TypeStruct::TypeStructElemCustom(std::move(type), std::move(name), required)
        {
        }

        virtual void gen_hash_code(std::ostringstream& oss, size_t indent);
        virtual void gen_read(std::ostringstream& oss, size_t indent);
        virtual void gen_write(std::ostringstream& oss, size_t indent);
    };

    class TypeStructElemListCpp : public TypeStruct::TypeStructElemList
    {
    public:
    TypeStructElemListCpp(std::string&& type,
                          std::string&& name,
                          bool required,
                          std::vector<std::string>&& ext)
        :TypeStruct::TypeStructElemList(std::move(type), std::move(name), required, std::move(ext))
        {
        }

        virtual std::string format_type() const;
        virtual void gen_hash_code(std::ostringstream& oss, size_t indent);
        virtual void gen_read(std::ostringstream& oss, size_t indent);
        virtual void gen_write(std::ostringstream& oss, size_t indent);
    };

    class TypeStructElemSetCpp : public TypeStruct::TypeStructElemSet
    {
    public:
    TypeStructElemSetCpp(std::string&& type,
                          std::string&& name,
                          bool required,
                          std::vector<std::string>&& ext)
        :TypeStruct::TypeStructElemSet(std::move(type), std::move(name), required, std::move(ext))
        {
        }

        virtual std::string format_type() const;
        virtual void gen_hash_code(std::ostringstream& oss, size_t indent);
        virtual void gen_read(std::ostringstream& oss, size_t indent);
        virtual void gen_write(std::ostringstream& oss, size_t indent);
    };

    class TypeStructElemMapCpp : public TypeStruct::TypeStructElemMap
    {
    public:
    TypeStructElemMapCpp(std::string&& type,
                         std::string&& name,
                         bool required,
                         std::vector<std::string>&& ext)
        :TypeStruct::TypeStructElemMap(std::move(type), std::move(name), required, std::move(ext))
        {
        }

        virtual std::string format_type() const;
        virtual void gen_hash_code(std::ostringstream& oss, size_t indent);
        virtual void gen_read(std::ostringstream& oss, size_t indent);
        virtual void gen_write(std::ostringstream& oss, size_t indent);
    };

    class TypeStructElemByteCpp : public TypeStruct::TypeStructElemByte
    {
    public:
    TypeStructElemByteCpp(std::string&& type,
                          std::string&& name,
                          bool required,
                          std::vector<std::string>&& ext)
        :TypeStruct::TypeStructElemByte(std::move(type), std::move(name), required, std::move(ext))
        {
        }

        virtual std::string format_type() const;
        virtual void gen_hash_code(std::ostringstream& oss, size_t indent);
        virtual void gen_class_member(std::ostringstream& oss, size_t indent);
        virtual void gen_read(std::ostringstream& oss, size_t indent);
        virtual void gen_write(std::ostringstream& oss, size_t indent);
        virtual void gen_setter_and_getter(std::ostringstream& oss, size_t indent, size_t index);
    };

} // namespace compiler
} // namespace soce

#endif
