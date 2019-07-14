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

#ifndef _SOCE_GROUP_LIST_M8RNNpbjGQ_H_
#define _SOCE_GROUP_LIST_M8RNNpbjGQ_H_

#include <unordered_map>
#include <functional>
#include "index-list.hpp"

namespace soce{
namespace utils{

    template <typename TK, typename TV>
    class GroupList
    {
    public:
        using gid_t = uint32_t;
        using value = TV;
        using group_list = IndexList<TK, TV, int>;

    public:
        int append(const TK& key, const TV& val, gid_t& gid = 0)
            {
                return append(key, gid, [&](group_list* plist){
                        plist->push_back(key, val);
                    });
            }

        int append(const TK& key, TV&& val, gid_t& gid = 0)
            {
                return append(key, gid, [&](group_list* plist){
                        plist->push_back(key, std::move(val));
                    });
            }

        int ack(const TK& key, value** val)
            {
                auto iter = key_index_.find(key);
                if (iter == key_index_.end()){
                    return -1;
                }

                if (iter->second->get(key, val)){
                    return -1;
                }

                --iter->second->get_custom();
                return iter->second->get_custom();
            }

        int get(gid_t gid, group_list** pval)
            {
                return data_.get(gid, pval);
            }

        int free(gid_t gid)
            {
                group_list* pgl = 0;
                if (data_.get(gid, &pgl)){
                    return -1;
                }

                pgl->clear();
                data_.erase(gid);
                return 0;
            }

    private:
        gid_t get_free_id()
            {
                group_list* plist = NULL;

                do{
                    ++gid_;
                    if (data_.get(gid_, &plist)){
                        break;
                    }
                } while (1);

                return gid_;
            }

        int append(const TK& key, gid_t& gid, std::function<void(group_list* plist)> push)
            {
                group_list* plist = NULL;

                if (gid == 0){
                    gid = get_free_id();
                    group_list val;
                    val.get_custom() = 0;
                    data_.push_back(gid, std::move(val));
                }

                if (data_.get(gid, &plist)){
                    return -1;
                }

                push(plist);
                ++plist->get_custom();
                key_index_[key] = plist;
                return 0;
            }

    private:
        IndexList<gid_t, group_list> data_;
        std::unordered_map<TK, group_list*> key_index_;
        gid_t gid_ = 0;
    };

} // namspace soce
} // namespace utils

#endif
