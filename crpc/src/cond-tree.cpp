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

#include <iostream>
#include <regex>
#include <stdlib.h>
#include <time.h>
#include "cond-tree.h"
#include "utils/str-helper.h"
#include "proto/binary-proto.h"

using namespace soce::utils;
using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace soce{
namespace crpc{

    CondLeafBool::CondLeafBool()
    {
        opers_["=="] = std::bind(&CondLeafBool::eq, this, std::placeholders::_1);
        opers_["!="] = std::bind(&CondLeafBool::neq, this, std::placeholders::_1);
    }

    int CondLeafBool::get_val(ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeBool){
            return -1;
        }
        return proto->read_bool(val_) ? 0 : -1;
    }

    bool CondLeafBool::match(const std::string& oper, const std::string& val)
    {
        auto iter = opers_.find(oper);
        if (iter == opers_.end()){
            return false;
        }

        bool bval = true;
        if (val == "true"){
            bval = true;
        }
        else if (val == "false"){
            bval = false;
        }
        else{
            return false;
        }

        return iter->second(bval);
    }

    void CondLeafBool::show()
    {
        cout << "val = " << val_ << endl;
    }

    bool CondLeafBool::eq(bool val)
    {
        return val_ == val;
    }

    bool CondLeafBool::neq(bool val)
    {
        return val_ != val;
    }

    CondLeafI32::CondLeafI32()
    {
        opers_["=="] = std::bind(&CondLeafI32::eq, this, std::placeholders::_1);
        opers_["!="] = std::bind(&CondLeafI32::neq, this, std::placeholders::_1);
        opers_[">"] = std::bind(&CondLeafI32::gt, this, std::placeholders::_1);
        opers_[">="] = std::bind(&CondLeafI32::egt, this, std::placeholders::_1);
        opers_["<"] = std::bind(&CondLeafI32::lt, this, std::placeholders::_1);
        opers_["<="] = std::bind(&CondLeafI32::elt, this, std::placeholders::_1);
    }

    int CondLeafI32::get_val(ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeInt32){
            return -1;
        }

        return proto->read_int32(val_) ? 0 : -1;
    }

    bool CondLeafI32::match(const std::string& oper, const std::string& val)
    {
        auto iter = opers_.find(oper);
        if (iter == opers_.end()){
            return false;
        }

        return iter->second(std::stoi(val));
    }

    void CondLeafI32::show()
    {
        cout << "val = " << val_ << endl;
    }

    bool CondLeafI32::eq(int32_t val)
    {
        return val_ == val;
    }

    bool CondLeafI32::neq(int32_t val)
    {
        return val_ != val;
    }

    bool CondLeafI32::gt(int32_t val)
    {
        return val_ > val;
    }

    bool CondLeafI32::egt(int32_t val)
    {
        return val_ >= val;
    }

    bool CondLeafI32::lt(int32_t val)
    {
        return val_ < val;
    }

    bool CondLeafI32::elt(int32_t val)
    {
        return val_ <= val;
    }

    CondLeafI64::CondLeafI64()
    {
        opers_["=="] = std::bind(&CondLeafI64::eq, this, std::placeholders::_1);
        opers_["!="] = std::bind(&CondLeafI64::neq, this, std::placeholders::_1);
        opers_[">"] = std::bind(&CondLeafI64::gt, this, std::placeholders::_1);
        opers_[">="] = std::bind(&CondLeafI64::egt, this, std::placeholders::_1);
        opers_["<"] = std::bind(&CondLeafI64::lt, this, std::placeholders::_1);
        opers_["<="] = std::bind(&CondLeafI64::elt, this, std::placeholders::_1);
    }

    int CondLeafI64::get_val(ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeInt64){
            return -1;
        }
        return proto->read_int64(val_) ? 0 : -1;
    }

    bool CondLeafI64::match(const std::string& oper, const std::string& val)
    {
        auto iter = opers_.find(oper);
        if (iter == opers_.end()){
            return false;
        }

        return iter->second(std::stoll(val));
    }

    void CondLeafI64::show()
    {
        cout << "val = " << val_ << endl;
    }

    bool CondLeafI64::eq(int64_t val)
    {
        return val_ == val;
    }

    bool CondLeafI64::neq(int64_t val)
    {
        return val_ != val;
    }

    bool CondLeafI64::gt(int64_t val)
    {
        return val_ > val;
    }

    bool CondLeafI64::egt(int64_t val)
    {
        return val_ >= val;
    }

    bool CondLeafI64::lt(int64_t val)
    {
        return val_ < val;
    }

    bool CondLeafI64::elt(int64_t val)
    {
        return val_ <= val;
    }

    CondLeafFloat::CondLeafFloat()
    {
        opers_["=="] = std::bind(&CondLeafFloat::eq, this, std::placeholders::_1);
        opers_["!="] = std::bind(&CondLeafFloat::neq, this, std::placeholders::_1);
        opers_[">"] = std::bind(&CondLeafFloat::gt, this, std::placeholders::_1);
        opers_[">="] = std::bind(&CondLeafFloat::egt, this, std::placeholders::_1);
        opers_["<"] = std::bind(&CondLeafFloat::lt, this, std::placeholders::_1);
        opers_["<="] = std::bind(&CondLeafFloat::elt, this, std::placeholders::_1);
    }

    int CondLeafFloat::get_val(ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeFloat){
            return -1;
        }
        return proto->read_float(val_) ? 0 : -1;
    }

    bool CondLeafFloat::match(const std::string& oper, const std::string& val)
    {
        auto iter = opers_.find(oper);
        if (iter == opers_.end()){
            return false;
        }

        return iter->second(std::stof(val));
    }

    void CondLeafFloat::show()
    {
        cout << "val = " << val_ << endl;
    }

    bool CondLeafFloat::eq(float val)
    {
        return val_ == val;
    }

    bool CondLeafFloat::neq(float val)
    {
        return val_ != val;
    }

    bool CondLeafFloat::gt(float val)
    {
        return val_ > val;
    }

    bool CondLeafFloat::egt(float val)
    {
        return val_ >= val;
    }

    bool CondLeafFloat::lt(float val)
    {
        return val_ < val;
    }

    bool CondLeafFloat::elt(float val)
    {
        return val_ <= val;
    }

    CondLeafDouble::CondLeafDouble()
    {
        opers_["=="] = std::bind(&CondLeafDouble::eq, this, std::placeholders::_1);
        opers_["!="] = std::bind(&CondLeafDouble::neq, this, std::placeholders::_1);
        opers_[">"] = std::bind(&CondLeafDouble::gt, this, std::placeholders::_1);
        opers_[">="] = std::bind(&CondLeafDouble::egt, this, std::placeholders::_1);
        opers_["<"] = std::bind(&CondLeafDouble::lt, this, std::placeholders::_1);
        opers_["<="] = std::bind(&CondLeafDouble::elt, this, std::placeholders::_1);
    }

    int CondLeafDouble::get_val(ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeDouble){
            return -1;
        }
        return proto->read_double(val_) ? 0 : -1;
    }

    bool CondLeafDouble::match(const std::string& oper, const std::string& val)
    {
        auto iter = opers_.find(oper);
        if (iter == opers_.end()){
            return false;
        }

        return iter->second(std::stod(val));
    }

    void CondLeafDouble::show()
    {
        cout << "val = " << val_ << endl;
    }

    bool CondLeafDouble::eq(double val)
    {
        return val_ == val;
    }

    bool CondLeafDouble::neq(double val)
    {
        return val_ != val;
    }

    bool CondLeafDouble::gt(double val)
    {
        return val_ > val;
    }

    bool CondLeafDouble::egt(double val)
    {
        return val_ >= val;
    }

    bool CondLeafDouble::lt(double val)
    {
        return val_ < val;
    }

    bool CondLeafDouble::elt(double val)
    {
        return val_ <= val;
    }

    CondLeafString::CondLeafString()
    {
        opers_["=="] = std::bind(&CondLeafString::eq, this, std::placeholders::_1);
        opers_["!="] = std::bind(&CondLeafString::neq, this, std::placeholders::_1);
        opers_["regex_match"] = std::bind(&CondLeafString::regex_match, this, std::placeholders::_1);
    }

    int CondLeafString::get_val(ProtoIf* proto)
    {
        SoceDataType type;
        if (!proto->read_type(type) || type != SoceDataType::kTypeString){
            return -1;
        }
        return proto->read_string(val_) ? 0 : -1;
    }

    bool CondLeafString::match(const std::string& oper, const std::string& val)
    {
        auto iter = opers_.find(oper);
        if (iter == opers_.end()){
            return false;
        }

        return iter->second(val);
    }

    void CondLeafString::show()
    {
        cout << "val = " << val_ << endl;
    }

    bool CondLeafString::eq(const std::string& val)
    {
        return val_ == val;
    }

    bool CondLeafString::neq(const std::string& val)
    {
        return val_ != val;
    }

    bool CondLeafString::regex_match(const std::string& val)
    {
        return std::regex_match(val_, std::regex(val));
    }

    std::shared_ptr<CondNode> CondNode::get(int n)
    {
        std::shared_ptr<CondNode> node;
        auto iter = nodes_.find(n);
        if (iter == nodes_.end()){
            node.reset(new CondNode);
            nodes_[n] = node;
        }
        else{
            node = iter->second;
        }

        node->add_ref();
        return node;
    }

    std::shared_ptr<CondNode> CondNode::try_del(int n)
    {
        std::shared_ptr<CondNode> node;
        auto iter = nodes_.find(n);
        if (iter == nodes_.end()){
            return node;
        }
        else{
            node = iter->second;
        }

        if (node->del_ref() == 0){
            nodes_.erase(n);
        }

        return node;
    }

    bool CondNode::match(const std::string& oper, const std::string& val)
    {
        if (!leaf_){
            return false;
        }

        return leaf_->match(oper, val);
    }

    int CondNode::get_val(ProtoIf* proto)
    {
        if (!is_leaf()){
            return -1;
        }

        if (leaf_->get_val(proto)){
            return -1;
        }

        set_val_ = true;
        return 0;
    }

    int CondNode::get_type(SoceDataType& type)
    {
        int rc = -1;
        if (leaf_){
            type = leaf_->get_type();
            rc = 0;
        }

        return rc;
    }

    void CondNode::clear()
    {
        set_val_ = false;
    }

    void CondNode::show()
    {
        if (leaf_){
            leaf_->show();
        }
    }

    CondTree::CondTree()
    {
        srand(time(NULL));
    }

    int CondTree::add_node(const std::string& path, SoceDataType type)
    {
        if (path == "_rand100"){
            return 0;
        }

        if (!check_type(type)){
            return -1;
        }

        vector<string> vpath;
        if (StrHelper::split(path, ".", vpath)){
            return -1;
        }

        std::shared_ptr<CondNode> node = root_;
        for (auto& i : vpath){
            int n = std::stoi(i);
            node = node->get(n);
        }

        CondLeafIf* l = create_leaf(type);
        if (!l){
            return -1;
        }
        node->add_leaf(l);
        nodes_[path] = node;

        return 0;
    }
    int CondTree::del_node(const std::string& path)
    {
        vector<string> vpath;
        if (StrHelper::split(path, ".", vpath)){
            return -1;
        }

        std::shared_ptr<CondNode> node = root_;
        for (auto& i : vpath){
            int n = std::stoi(i);
            node = node->try_del(n);
            if (!node){
                return -1;
            }
        }

        nodes_.erase(path);
        return 0;
    }

    int CondTree::parse(ProtoIf* proto)
    {
        return parse(root_, proto);
    }

    bool CondTree::match(const std::string& path,
                         const std::string& oper,
                         const std::string& val)
    {
        if (path == "_rand100"){
            return match_rand100(oper, val);
        }

        auto iter = nodes_.find(path);
        if (iter == nodes_.end()){
            return false;
        }

        return iter->second->match(oper, val);
    }

    void CondTree::clear()
    {
        for (auto& i : nodes_){
            i.second->clear();
        }
    }

    std::shared_ptr<CondTree> CondTree::clone()
    {
        std::shared_ptr<CondTree> tree = std::shared_ptr<CondTree>(new CondTree);
        tree->ver_ = ver_;
        for (auto& i : nodes_){
            SoceDataType type;
            if (!i.second->get_type(type)){
                tree->add_node(i.first, type);
            }
        }
        return tree;
    }

    bool CondTree::check_type(SoceDataType type)
    {
        if (type == SoceDataType::kTypeBool
            || type == SoceDataType::kTypeInt32
            || type == SoceDataType::kTypeInt64
            || type == SoceDataType::kTypeFloat
            || type == SoceDataType::kTypeDouble
            || type == SoceDataType::kTypeString){
            return true;
        }

        return false;
    }

    CondLeafIf* CondTree::create_leaf(SoceDataType type)
    {
        CondLeafIf* leaf = NULL;
        switch (type){
        case SoceDataType::kTypeBool:
            leaf = new CondLeafBool;
            break;

        case SoceDataType::kTypeInt32:
            leaf = new CondLeafI32;
            break;

        case SoceDataType::kTypeInt64:
            leaf = new CondLeafI64;
            break;

        case SoceDataType::kTypeFloat:
            leaf = new CondLeafFloat;
            break;

        case SoceDataType::kTypeDouble:
            leaf = new CondLeafDouble;
            break;

        case SoceDataType::kTypeString:
            leaf = new CondLeafString;
            break;

        default:
            break;
        }

        return leaf;
    }


    int CondTree::parse(std::shared_ptr<CondNode> node, ProtoIf* proto)
    {
        uint32_t len = 0;
        if (proto->read_len(len) == 0){
            return -1;
        }
        uint32_t pos = proto->get_cur_pos();
        uint32_t elems = 0;
        if (!proto->read_len(elems)){
            return -1;
        }

        int last = 0;
        for (auto& i : node->get_nodes()){
            skip_elems(i.first - last, proto);

            if (i.second->is_leaf()){
                if (i.second->get_val(proto)){
                    return -1;
                }
            }
            else{
                SoceDataType type;
                if (!proto->read_type(type) || type != SoceDataType::kTypeStruct){
                    return -1;
                }

                if (parse(i.second, proto)){
                    return -1;
                }
            }

            last = i.first + 1;
        }

        proto->skip_to(len + pos);
        return 0;
    }

    int CondTree::skip_elem(ProtoIf* proto)
    {
        SoceDataType type;
        uint32_t size = 0;
        uint32_t len = 0;
        int32_t i32 = 0;
        int64_t i64 = 0;

        if (!proto->read_type(type)){
            return -1;
        }

        switch(type){
        case SoceDataType::kTypeNull:
        case SoceDataType::kTypeBool:
            if (proto->skip(1)){
                return -1;
            }
            break;

        case SoceDataType::kTypeInt32:
        case SoceDataType::kTypeEnum:
            size = proto->read_int32(i32);
            if (!size){
                return -1;
            }
            break;

        case SoceDataType::kTypeFloat:
            if (proto->skip(4)){
                return -1;
            }
            break;

        case SoceDataType::kTypeInt64:
            size = proto->read_int64(i64);
            if (!size){
                return -1;
            }
            break;

        case SoceDataType::kTypeDouble:
            if (proto->skip(8)){
                return -1;
            }
            break;

        default:
            size = proto->read_len(len);
            if (size == 0){
                return -1;
            }
            if (proto->skip(len)){
                return -1;
            }
            break;
        }

        return 0;
    }

    int CondTree::skip_elems(int n, ProtoIf* proto)
    {
        for (int i=0; i<n; ++i){
            if (skip_elem(proto)){
                return -1;
            }
        }

        return 0;
    }

    bool CondTree::match_rand100(const std::string& oper, const std::string& val)
    {
        auto num = rand() % 101;
        auto v = atoi(val.c_str());

        bool rc = false;
        if (oper == "<"){
            rc = (num < v) ? true : false;
        }
        else if (oper == "<="){
            rc = (num <= v) ? true : false;
        }
        else if (oper == "=="){
            rc = (num == v) ? true : false;
        }
        else if (oper == ">"){
            rc = (num > v) ? true : false;
        }
        else if (oper == ">="){
            rc = (num >= v) ? true : false;
        }
        else if (oper == "!="){
            rc = (num != v) ? true : false;
        }

        return rc;
    }

    void CondTree::show()
    {
        for (auto& i : nodes_){
            i.second->show();
        }
    }

} // namespace crpc
} // namespace soce
