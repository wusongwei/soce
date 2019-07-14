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

#ifndef _SOCE_FALIST_ZfoP7P4UaR_H_
#define _SOCE_FALIST_ZfoP7P4UaR_H_

#include <iostream>
#include <unordered_map>
#include <list>
#include <utility>
#include <functional>
#include <stdint.h>

namespace soce{
namespace utils{

    template <typename TK, typename TV, typename TC = char>
    class IndexList
    {
    public:
        using iterator = typename std::list<std::pair<TK,TV>>::iterator;
        using const_iterator = typename std::list<std::pair<TK,TV>>::const_iterator;
        using value = typename std::pair<TK,TV>;
        using reference = value&;
        using const_reference = const value&;

        // If the key exist when push
        using OverlapStrategy = enum{
            kOLSError,          // report error
            kOLSOverlap,        // overlap the old key
            kOLSRetain          // retain the old key
        };

        // If the size is bigger than max_size when push
        using OverflowStrategy = enum{
            kOFSError,          // report error
            kOFSEraseFront,     // erase the front one
            kOFSEraseBack       // erase the back one
        };

    public:
        inline void set_max_size(uint32_t n)
            {
                max_size_ = n;
            }

        inline void set_max_idle(uint32_t n)
            {
                max_idle_ = n;
            }

        inline void set_overlap_strategy(OverlapStrategy ols)
            {
                ols_ = ols;
            }

        inline void set_overflow_strategy(OverflowStrategy ofs)
            {
                ofs_ = ofs;
            }

        inline TC& get_custom()
            {
                return custom_;
            }

        // Iterators
        inline iterator begin()
            {
                return data_.begin();
            }

        inline const_iterator begin() const
            {
                return data_.begin();
            }

        inline iterator end()
            {
                return data_.end();
            }

        inline const_iterator end() const
            {
                return data_.end();
            }

        // Capacity
        inline bool empty()
            {
                return data_.empty();
            }

        inline uint32_t size()
            {
                return data_.size();
            }

        inline uint32_t max_size()
            {
                return max_size_;
            }

        // Element access
        inline reference front()
            {
                return *begin();
            }

        inline const_reference front() const
            {
                return *begin();
            }

        inline reference back()
            {
                auto iter = end();
                --iter;
                return *iter;
            }

        inline const_reference back() const
            {
                auto iter = end();
                --iter;
                return *iter;
            }

        // Modifiers
        int push_front(const TK& key, const TV& val)
            {
                return add(key, val, true, [&key, &val, this]()->iterator{
                        data_.push_front(std::make_pair(key, val));
                        return data_.begin();
                    });
            }

        int push_front(const TK& key, TV&& val)
            {
                return add(key, std::move(val), true, [&key, &val, this](){
                        data_.push_front(std::make_pair(key, std::move(val)));
                        return data_.begin();
                    });
            }

        int push_back(const TK& key, const TV& val)
            {
                return add(key, val, false, [&key, &val, this](){
                        data_.push_back(std::make_pair(key, val));
                        return --data_.end();
                    });
            }

        int push_back(const TK& key, TV&& val)
            {
                return add(key, std::move(val), false, [&key, &val, this](){
                        data_.push_back(std::make_pair(key, std::move(val)));
                        return --data_.end();
                    });
            }

        void pop_front()
            {
                pop([this]()->iterator{
                        return data_.begin();
                    });
            }

        void pop_back()
            {
                pop([this]()->iterator{
                        return --data_.end();
                    });
            }

        int get(const TK& key, TV** out)
            {
                auto iter = index_.find(key);
                if (iter == index_.end()){
                    return -1;
                }

                *out = &iter->second->second;
                return 0;
            }

        bool find(const TK& key) const
            {
                return (index_.find(key) == index_.end()) ? false : true;
            }

        int update_to_front(const TK& key, TV** out)
            {
                return update(key, out, [this](iterator iter){
                        data_.splice(data_.begin(), data_, iter);
                    });
            }

        int update_to_back(const TK& key, TV** out)
            {
                return update(key, out, [this](iterator iter){
                        data_.splice(data_.end(), data_, iter);
                    });
            }

        void erase(const TK& key)
            {
                auto iter = index_.find(key);
                if (iter == index_.end()){
                    return;
                }

                if (idle_.size() < max_idle_){
                    idle_.splice(idle_.begin(), data_, iter->second);
                }
                else{
                    data_.erase(iter->second);
                }

                index_.erase(iter);
            }

        iterator erase(iterator iter)
            {
                if (iter == data_.end()){
                    return data_.end();
                }

                TK key = (*iter).first;
                iterator rc = iter;
                ++rc;

                if (idle_.size() < max_idle_){
                    idle_.splice(idle_.begin(), data_, iter->second);
                }
                else{
                    data_.erase(iter->second);
                }

                index_.erase(key);
                return rc;
            }

        inline void clear()
            {
                data_.clear();
                index_.clear();
            }

        void show()
            {
                std::cout << "********** SHOW BEGIN **********" << std::endl;
                std::cout << "---------- show index ----------" << std::endl;
                for (auto& i : index_){
                    std::cout << "key = " << i.first
                              << " => [" << i.second->first << ":" << i.second->second << "]"
                              << std::endl;
                }
                std::cout << "---------- show data ----------" << std::endl;
                for (auto& i : data_){
                    std::cout << "key = " << i.first << ", val = " << i.second << std::endl;
                }
                std::cout << "---------- idle  ----------" << std::endl;
                std::cout << "idle size = " << idle_.size() << std::endl;
                std::cout << "********** SHOW END **********" << std::endl;
            }

    private:
        int add(const TK& key, const TV& val, bool front, std::function<iterator()> push)
            {
                return add(key,
                           front,
                           [&val](TV& dst){
                               dst = val;
                           },
                           push);
            }

        int add(const TK& key, TV&& val, bool front, std::function<iterator()> push)
            {
                return add(key,
                           front,
                           [&val](TV& dst){
                               dst = std::move(val);
                           },
                           push);
            }

        int exist_key(const TK& key, bool front, std::function<void(TV& tv)> assign, TV& tv)
            {
                int rc = 0;

                switch (ols_){
                case kOLSError:
                    rc = -1;
                    break;

                case kOLSOverlap:
                    assign(tv);
                    rc = front ? update_to_front(key, nullptr) : update_to_back(key, nullptr);
                    break;

                case kOLSRetain:
                    rc = front ? update_to_front(key, nullptr) : update_to_back(key, nullptr);
                    break;

                default:
                    rc = -1;
                    break;
                }

                return rc;
            }

        int overflow(const TK& key, bool front, std::function<void(TV& tv)> assign)
            {
                int rc = 0;

                switch (ofs_){
                case kOFSEraseFront:
                    index_.erase(data_.front().first);
                    data_.front().first = key;
                    assign(data_.front().second);

                    if (front){
                        index_[key] = data_.begin();
                    }
                    else{
                        data_.splice(data_.end(), data_, data_.begin());
                        index_[key] = --data_.end();
                    }

                    break;

                case kOFSEraseBack:
                    index_.erase(data_.back().first);
                    data_.back().first = key;
                    assign(data_.back().second);

                    if (front){
                        data_.splice(data_.begin(), data_, --data_.end());
                        index_[key] = data_.begin();
                    }
                    else{
                        index_[key] = --data_.end();
                    }
                    break;

                case kOFSError:
                default:
                    rc = -1;
                    break;
                }

                return rc;
            }

        int add(const TK& key, bool front, std::function<void(TV& tv)> assign, std::function<iterator()> push)
            {
                int rc = 0;
                auto iter = index_.find(key);
                if (iter != index_.end()){
                    rc = exist_key(key, front, assign, iter->second->second);
                }
                else if (max_size_ != 0 && data_.size() >= max_size_){
                    rc = overflow(key, front, assign);
                }
                else if(!idle_.empty()){
                    idle_.front().first = key;
                    assign(idle_.front().second);
                    iterator pos = front ? data_.begin() : data_.end();
                    data_.splice(pos, idle_, idle_.begin());

                    index_[key] = front ? data_.begin() : --data_.end();
                }
                else{
                    index_[key] = push();
                }

                return rc;
            }

        void pop(std::function<iterator()> action)
            {
                if (data_.empty()){
                    return;
                }

                auto iter = action();
                index_.erase(iter->first);

                if (idle_.size() < max_idle_){
                    idle_.splice(idle_.begin(), data_, iter);
                }
                else{
                    data_.erase(iter);
                }
            }

        int update(const TK& key, TV** out, std::function<void(iterator iter)> splice)
            {
                auto iter = index_.find(key);
                if (iter == index_.end()){
                    return -1;
                }

                splice(iter->second);
                if (out){
                    *out = &iter->second->second;
                }
                return 0;
            }

    private:
        std::list<std::pair<TK, TV>> data_;
        std::list<std::pair<TK, TV>> idle_;
        std::unordered_map<TK, iterator> index_;
        TC custom_;
        uint32_t max_size_ = 0;
        uint32_t max_idle_ = 100;
        OverlapStrategy ols_ = kOLSError;
        OverflowStrategy ofs_ = kOFSError;
    };

}} // end namespace
#endif
