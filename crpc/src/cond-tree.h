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

#ifndef _SOCE_COND_TREE_q0v5Ipe6W9_H_
#define _SOCE_COND_TREE_q0v5Ipe6W9_H_

#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include "proto/proto-if.h"

using namespace soce::proto;

namespace soce{
namespace crpc{

    class CondLeafIf
    {
    public:
        virtual int get_val(ProtoIf* proto) = 0;
        virtual bool match(const std::string& oper, const std::string& val) = 0;
        virtual SoceDataType get_type() = 0;
        virtual void show() = 0;;
    };

    class CondLeafBool : public CondLeafIf
    {
    public:
        CondLeafBool();

        virtual int get_val(ProtoIf* proto);
        virtual bool match(const std::string& oper, const std::string& val);
        virtual SoceDataType get_type() {return SoceDataType::kTypeBool;}
        virtual void show();
        bool eq(bool val);
        bool neq(bool val);

    private:
        bool val_ = 0;
        std::unordered_map<std::string, std::function<bool (bool)>> opers_;
    };

    class CondLeafI32 : public CondLeafIf
    {
    public:
        CondLeafI32();

        virtual int get_val(ProtoIf* proto);
        virtual bool match(const std::string& oper, const std::string& val);
        virtual SoceDataType get_type() {return SoceDataType::kTypeInt32;}
        virtual void show();
        bool eq(int32_t val);
        bool neq(int32_t val);
        bool gt(int32_t val);
        bool egt(int32_t val);
        bool lt(int32_t val);
        bool elt(int32_t val);

    private:
        int32_t val_ = 0;
        std::unordered_map<std::string, std::function<bool (int32_t)>> opers_;
    };

    class CondLeafI64 : public CondLeafIf
    {
    public:
        CondLeafI64();

        virtual int get_val(ProtoIf* proto);
        virtual bool match(const std::string& oper, const std::string& val);
        virtual SoceDataType get_type() {return SoceDataType::kTypeInt64;}
        virtual void show();
        bool eq(int64_t val);
        bool neq(int64_t val);
        bool gt(int64_t val);
        bool egt(int64_t val);
        bool lt(int64_t val);
        bool elt(int64_t val);

    private:
        int64_t val_ = 0;
        std::unordered_map<std::string, std::function<bool (int64_t)>> opers_;
    };

    class CondLeafFloat : public CondLeafIf
    {
    public:
        CondLeafFloat();

        virtual int get_val(ProtoIf* proto);
        virtual bool match(const std::string& oper, const std::string& val);
        virtual SoceDataType get_type() {return SoceDataType::kTypeFloat;}
        virtual void show();
        bool eq(float val);
        bool neq(float val);
        bool gt(float val);
        bool egt(float val);
        bool lt(float val);
        bool elt(float val);

    private:
        float val_ = 0;
        std::unordered_map<std::string, std::function<bool (float)>> opers_;
    };

    class CondLeafDouble : public CondLeafIf
    {
    public:
        CondLeafDouble();

        virtual int get_val(ProtoIf* proto);
        virtual bool match(const std::string& oper, const std::string& val);
        virtual SoceDataType get_type() {return SoceDataType::kTypeDouble;}
        virtual void show();
        bool eq(double val);
        bool neq(double val);
        bool gt(double val);
        bool egt(double val);
        bool lt(double val);
        bool elt(double val);

    private:
        double val_ = 0;
        std::unordered_map<std::string, std::function<bool (double)>> opers_;
    };

    class CondLeafString : public CondLeafIf
    {
    public:
        CondLeafString();

        virtual int get_val(ProtoIf* proto);
        virtual bool match(const std::string& oper, const std::string& val);
        virtual SoceDataType get_type() {return SoceDataType::kTypeString;}
        virtual void show();
        bool eq(const std::string& val);
        bool neq(const std::string& val);
        bool regex_match(const std::string& val);

    private:
        std::string val_ = 0;
        std::unordered_map<std::string, std::function<bool (const std::string&)>> opers_;
    };

    class CondNode
    {
    public:
        std::shared_ptr<CondNode> get(int n);
        std::shared_ptr<CondNode> try_del(int n);
        bool match(const std::string& oper, const std::string& val);
        int get_val(ProtoIf* proto);
        inline void add_leaf(CondLeafIf* leaf) {leaf_.reset(leaf);}
        inline bool is_leaf() {return leaf_ ? true : false;}
        inline std::map<int, std::shared_ptr<CondNode>>& get_nodes() {return  nodes_;}
        inline void add_ref() {++ref_;}
        inline int del_ref() {return --ref_;}
        int get_type(SoceDataType& type);
        void clear();
        void show();

    private:
        std::map<int, std::shared_ptr<CondNode>> nodes_;
        std::shared_ptr<CondLeafIf> leaf_;
        int ref_ = 0;
        bool set_val_ = false;
    };

    class CondTree
    {
    public:
        CondTree();

        int add_node(const std::string& path, SoceDataType type);
        int del_node(const std::string& path);
        int parse(ProtoIf* proto);
        bool match(const std::string& path,
                   const std::string& oper,
                   const std::string& val);
        void clear();
        inline uint32_t get_ver() {return ver_;}
        inline void add_ver() {++ver_;}
        std::shared_ptr<CondTree> clone();
        void show();

    private:
        bool check_type(SoceDataType type);
        CondLeafIf* create_leaf(SoceDataType type);
        int parse(std::shared_ptr<CondNode> node, ProtoIf* proto);
        int skip_elem(ProtoIf* proto);
        int skip_elems(int n, ProtoIf* proto);
        bool match_rand100(const std::string& oper, const std::string& val);

    private:
        uint32_t ver_ = 0;
        std::shared_ptr<CondNode> root_{new CondNode};
        std::unordered_map<std::string, std::shared_ptr<CondNode>> nodes_;
    };

} // namespace crpc
} // namespace soce

#endif
