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

#ifndef _FILTER_20161231_dz5pDrFk6q_H_
#define _FILTER_20161231_dz5pDrFk6q_H_

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include "log4rel/type.h"
#include "utils/index-list.hpp"
#include "utils/singleton.hpp"

namespace soce{
namespace log4rel{

    class KeyCache
    {
    public:
        int parse(const std::string& filter, std::vector<std::string>** vec_filter);

    private:
        int parse_filter(const std::string& filter, std::vector<std::string>& vec_filter);

    private:
        soce::utils::IndexList<std::string, std::vector<std::string>> key_caches_;
        const size_t cache_size_ = 1000;
    };

#define KEY_CACHE soce::utils::SoceSingletonThreadLocal<soce::log4rel::KeyCache>::get_instance()

    class KeyFilterTree
    {
    public:
        int add_filter(const std::string& filter);
        int del_filter(const std::string& filter);
        bool match(const std::string& key);

        inline void set_type(LogFilterType type) {type_ = type;}
        inline bool empty() {return !endpoint_ && children_.empty();}
        inline void set_endpoint() {endpoint_ = true;}
        inline void unset_endpoint() {endpoint_ = false;}

    private:
        KeyFilterTree* get_child(const std::string& name);
        inline void del_child(const std::string& name) {children_.erase(name);}
        bool match(const std::vector<std::string>& vec_filter, size_t index);

    private:
        std::map<std::string, std::shared_ptr<KeyFilterTree>> children_;
        bool endpoint_ = false;
        LogFilterType type_ = kLogFilterAllow;
    };

    class KeyFilter
    {
    public:
        KeyFilter();
        ~KeyFilter() = default;

        int add_filter(LogFilterType type, const std::string& filter);
        int del_filter(LogFilterType type, const std::string& filter);
        bool match(LogFilterType type, const std::string& key);

    private:
        KeyFilterTree deny_tree_;
        KeyFilterTree allow_tree_;
    };

    class RecordFilter
    {
    public:
        int add_filter(LogFilterType type, const std::string& filter);
        int del_filter(LogFilterType type, const std::string& filter);
        bool match(LogFilterType type, const std::string& record, bool defMatch);

    private:
        std::unordered_set<std::string> deny_filters_;
        std::unordered_set<std::string> allow_filters_;
    };

} // namespace log4rel
} // namespace soce
#endif
