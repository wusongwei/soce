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
#include <stack>
#include <regex>
#include "filter.h"

using namespace std;

namespace soce{
namespace log4rel{

    int KeyCache::parse(const std::string& filter, vector<string>** out)
    {
        vector<string>* vec_filter = nullptr;
        if (key_caches_.update_to_front(filter, &vec_filter) != 0){

            vector<string> vkey;
            if (parse_filter(filter, vkey)){
                return -1;
            }

            key_caches_.push_front(filter, std::move(vkey));
            if (key_caches_.size() > cache_size_){
                key_caches_.pop_back();
            }
            key_caches_.get(filter, &vec_filter);
        }

        *out = vec_filter;
        return 0;
    }

    int KeyCache::parse_filter(const std::string& filter, vector<string>& vec_filter)
    {
        if (filter.empty() || filter[filter.size()-1] == '.'){
            return -1;
        }

        // check star position
        size_t star_pos = filter.find_first_of('*');
        if (star_pos != string::npos && star_pos != filter.size() -1){
            return -1;
        }

        size_t last_pos = 0;
        size_t cur_pos = last_pos;
        string tmp_filter;

        while (cur_pos != string::npos){
            cur_pos = filter.find_first_of('.', last_pos);
            if (cur_pos == string::npos){
                tmp_filter = filter.substr(last_pos);
            }
            else{
                tmp_filter = filter.substr(last_pos, cur_pos - last_pos);
            }

            // check +
            if (tmp_filter.find_first_of('+') != string::npos && tmp_filter.size() != 1){
                return -1;
            }

            if (tmp_filter.empty()){
                return -1;
            }

            vec_filter.push_back(tmp_filter);
            last_pos = cur_pos + 1;
        }

        return 0;
    }

    int KeyFilterTree::add_filter(const std::string& filter)
    {
        vector<string>* vec_filter;
        if (KEY_CACHE.parse(filter, &vec_filter)){
            return -1;
        }

        KeyFilterTree* node = this;
        for (size_t i=0; i<(*vec_filter).size(); ++i){
            node = node->get_child((*vec_filter)[i]);
        }

        node->set_endpoint();
        return 0;
    }

    int KeyFilterTree::del_filter(const std::string& filter)
    {
        vector<string>* vec_filter = nullptr;
        if (KEY_CACHE.parse(filter, &vec_filter)){
            return -1;
        }

        KeyFilterTree* node = this;
        stack<pair<KeyFilterTree*, string>> index;
        for (size_t i=0; i<(*vec_filter).size(); ++i){
            index.push(make_pair(node, (*vec_filter)[i]));
            node = node->get_child((*vec_filter)[i]);
        }
        node->unset_endpoint();

        while (!index.empty()){
            if (!node->empty()){
                break;
            }

            pair<KeyFilterTree*, string>& p = index.top();
            p.first->del_child(p.second);
            node = p.first;
            index.pop();
        }

        return 0;
    }

    bool KeyFilterTree::match(const std::string& key)
    {
        if (children_.empty()){
            return false;
        }

        vector<string>* vec_key;
        if (KEY_CACHE.parse(key, &vec_key) != 0){
            return false;
        }

        return match(*vec_key, 0);
    }


    KeyFilterTree* KeyFilterTree::get_child(const string& name)
    {
        KeyFilterTree* rc = NULL;
        auto iter = children_.find(name);
        if (iter != children_.end()){
            rc = iter->second.get();
        }
        else{
            shared_ptr<KeyFilterTree> node(new KeyFilterTree);
            children_[name] = node;
            rc = node.get();
        }

        return rc;
    }

    bool KeyFilterTree::match(const std::vector<std::string>& vec_filter, size_t index)
    {
        if (index >= vec_filter.size()){
            return endpoint_ ? true : false;
        }

        auto iter = children_.find("*");
        if (iter != children_.end()){
            return true;
        }

        auto iter1 = children_.find("+");
        if (iter1 != children_.end()){
            if (iter1->second->match(vec_filter, index + 1)){
                return true;
            }
        }

        auto iter2 = children_.find(vec_filter[index]);
        if (iter2 != children_.end()){
            if (iter2->second->match(vec_filter, index + 1)){
                return true;
            }
        }

        return false;
    }

    KeyFilter::KeyFilter()
    {
        deny_tree_.set_type(kLogFilterDeny);
        allow_tree_.set_type(kLogFilterAllow);
    }

    int KeyFilter::add_filter(LogFilterType type, const std::string& filter)
    {
        int rc = -1;
        if (type == kLogFilterDeny){
            rc = deny_tree_.add_filter(filter);
        }
        else if (type == kLogFilterAllow){
            rc = allow_tree_.add_filter(filter);
        }

        return rc;
    }

    int KeyFilter::del_filter(LogFilterType type, const std::string& filter)
    {
        int rc = -1;
        if (type == kLogFilterDeny){
            rc = deny_tree_.del_filter(filter);
        }
        else if (type == kLogFilterAllow){
            rc = allow_tree_.del_filter(filter);
        }

        return rc;
    }

    bool KeyFilter::match(LogFilterType type, const std::string& filter)
    {
        bool match = false;
        if (type == kLogFilterDeny){
            match = deny_tree_.match(filter);
        }
        else if (type == kLogFilterAllow){
            match = allow_tree_.match(filter);
        }

        return match;
    }

    int RecordFilter::add_filter(LogFilterType type, const std::string& filter)
    {
        int rc = 0;
        if (type == kLogFilterDeny){
            deny_filters_.insert(filter);
        }
        else if (type == kLogFilterAllow){
            allow_filters_.insert(filter);
        }
        else{
            rc = -1;
        }

        return rc;
    }

    int RecordFilter::del_filter(LogFilterType type, const std::string& filter)
    {
        int rc = 0;
        if (type == kLogFilterDeny){
            deny_filters_.erase(filter);
        }
        else if (type == kLogFilterAllow){
            allow_filters_.erase(filter);
        }
        else{
            rc = -1;
        }

        return rc;
    }

    bool RecordFilter::match(LogFilterType type, const std::string& record, bool defMatch)
    {
        std::unordered_set<std::string>* p = NULL;
        if (type == kLogFilterDeny){
            p = &deny_filters_;
        }
        else if(type == kLogFilterAllow){
            p = &allow_filters_;
        }
        else{
            return false;
        }

        if (p->empty()){
            return defMatch;
        }

        bool match = false;
        for (auto& i : *p){
            try{
                if (std::regex_match(record, std::regex(i))){
                    match = true;
                    break;
                }
            }catch(...){
                return false;
            }
        }

        return match;
    }

} // namespace log4rel
} // namespace soce
