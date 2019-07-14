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

#ifndef _SOCE_TYPES_DEF_TwK7IaINBF_H_
#define _SOCE_TYPES_DEF_TwK7IaINBF_H_

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <memory>
#include "typedef-if.h"
#include "utils/index-list.hpp"

namespace soce{
namespace compiler{

    class TypeEnum : public TypeDefIf
    {
    public:
    TypeEnum(std::string&& name,
             std::vector<std::pair<std::string, std::string>>&& elems)
        : TypeDefIf("enum", std::move(name)),
            elems_(std::move(elems))
            {
            }

    protected:
        std::vector<std::pair<std::string, std::string>> elems_;
    };

    class TypeList : public TypeDefIf
    {
    public:
    TypeList(std::string&& name, std::string&& eletype_)
        : TypeDefIf("list", std::move(name)),
            m_eletype_(std::move(eletype_))
            {
            }

    protected:
        std::string m_eletype_;
    };

    class TypeSet : public TypeDefIf
    {
    public:
    TypeSet(std::string&& name, std::string&& eletype_)
        : TypeDefIf("set", std::move(name)),
            m_eletype_(std::move(eletype_))
            {
            }

    protected:
        std::string m_eletype_;
    };

    class TypeMap : public TypeDefIf
    {
    public:
    TypeMap(std::string&& name, std::string&& key_type, std::string&& val_type)
        : TypeDefIf("map", std::move(name)),
            key_type_(std::move(key_type)),
            val_type_(std::move(val_type))
            {
            }

    protected:
        std::string key_type_;
        std::string val_type_;
    };

    class TypeStruct : public TypeDefIf
    {
    public:
        class TypeStructElemBase
        {
        public:
        TypeStructElemBase(std::string&& type, std::string&& name, bool required)
            : type_(std::move(type)),
                name_(std::move(name)),
                required_(required)
                {
                }

            inline void required() {required_ = true;}
            inline bool is_required() const {return required_;}
            inline const std::string& get_name() const {return name_;}
            virtual std::string get_type() const {return type_;}
            virtual std::string format_type() const {return type_;}

            virtual void gen_setter_and_getter(std::ostringstream& oss, size_t indent, size_t index);
            virtual void gen_hash_code(std::ostringstream& oss, size_t indent) = 0;
            virtual void gen_class_member(std::ostringstream& oss, size_t indent);

            virtual void gen_read(std::ostringstream& oss, size_t indent) = 0;
            virtual void gen_write(std::ostringstream& oss, size_t indent) = 0;

        protected:
            std::string type_;
            std::string name_;
            bool required_;
        };

        class TypeStructElemPrime : public TypeStructElemBase
        {
        public:
        TypeStructElemPrime(std::string&& type,
                            std::string&& name,
                            bool required)
            :TypeStructElemBase(std::move(type), std::move(name), required)
                {
                }
        };

        class TypeStructElemCustom : public TypeStructElemBase
        {
        public:
        TypeStructElemCustom(std::string&& type,
                             std::string&& name,
                             bool required)
            : TypeStructElemBase(std::move(type), std::move(name), required)
            {
            }
        };

        class TypeStructElemComplex : public TypeStructElemBase
        {
        public:
        TypeStructElemComplex(std::string&& type,
                              std::string&& name,
                              bool required,
                              std::vector<std::string>&& ext)
            : TypeStructElemBase(std::move(type), std::move(name), required),
                ext_(std::move(ext))
                {
                }

        protected:
            std::vector<std::string> ext_;
        };

        class TypeStructElemList : public TypeStructElemComplex
        {
        public:
        TypeStructElemList(std::string&& type,
                           std::string&& name,
                           bool required,
                           std::vector<std::string>&& ext)
            : TypeStructElemComplex(std::move(type), std::move(name), required, std::move(ext))
            {
            }
        };

        class TypeStructElemSet : public TypeStructElemComplex
        {
        public:
        TypeStructElemSet(std::string&& type,
                          std::string&& name,
                          bool required,
                          std::vector<std::string>&& ext)
            : TypeStructElemComplex(std::move(type), std::move(name), required, std::move(ext))
            {
            }
        };

        class TypeStructElemMap : public TypeStructElemComplex
        {
        public:
        TypeStructElemMap(std::string&& type,
                          std::string&& name,
                          bool required,
                          std::vector<std::string>&& ext)
            : TypeStructElemComplex(std::move(type), std::move(name), required, std::move(ext))
            {
            }
        };

        class TypeStructElemByte : public TypeStructElemComplex
        {
        public:
        TypeStructElemByte(std::string&& type,
                           std::string&& name,
                           bool required,
                           std::vector<std::string>&& ext)
            : TypeStructElemComplex(std::move(type), std::move(name), required, std::move(ext))
            {
            }
        };

    public:
        using ElemList = soce::utils::IndexList<std::string, std::shared_ptr<TypeStructElemBase>>;
        using iterator = ElemList::iterator;

    public:
    TypeStruct(std::string&& name)
        : TypeDefIf("struct", std::move(name))
        {
        }

        int32_t add_elem(std::string&& type,
                         std::string&& name,
                         bool required,
                         std::vector<std::string>&& ext);
        void add_required_attr(const std::string& name);
        void add_service_header();
        inline void set_name(const std::string& name) {name_ = name;}
        inline iterator begin() {return elems_.begin();}
        inline iterator end() {return elems_.end();}
        inline size_t size() {return elems_.size();}

    protected:
        ElemList elems_;
    };

} // namespace compiler
} // namespace soce

#endif
