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

#include "log4rel/logger.h"
#include "utils/index-list.hpp"

using namespace soce::utils;

class ExampleIndexList
{
public:
    ExampleIndexList()
        {
            index_list_.push_back("a", 1);
            index_list_.push_back("b", 2);
            index_list_.push_back("c", 3);
        }

    void start()
        {
            SOCE_DEBUG << _D("--------------- sample_index_list ---------------");

            insert_and_access();
            update();
            overlap();
            overflow();
        }

private:
    void insert_and_access()
        {
            for (auto& i : index_list_){
                SOCE_DEBUG << _S("ordered access key", i.first)
                           << _S("val", i.second);
            }

            std::string key = "b";
            int* val = nullptr;
            index_list_.get(key, &val);
            SOCE_DEBUG << _S("random access key", key) << _S("val", *val);
        }

    void update()
        {
            std::string key = "b";
            SOCE_DEBUG << _D("update to back") << _S("key", key);
            int* val = nullptr;

            index_list_.update_to_back(key, &val);
            *val = 4;

            for (auto& i : index_list_){
                SOCE_DEBUG << _S("key", i.first) << _S("val", i.second);
            }
        }

    void overlap()
        {
            // report error
            std::string key = "c";
            int val = 5;
            int rc = index_list_.push_front(key, val);
            SOCE_DEBUG << _S("overlap strategy", "report error") << _S("rc", rc);

            // overlap
            index_list_.set_overlap_strategy(IndexList<std::string, int>::kOLSOverlap);
            rc = index_list_.push_front(key, val);
            SOCE_DEBUG << _S("overlap strategy", "overlap")
                       << _S("action", "push_front")
                       << _S("rc", rc);
            for (auto& i : index_list_){
                SOCE_DEBUG << _S("key", i.first) << _S("val", i.second);
            }

            // retain
            index_list_.set_overlap_strategy(IndexList<std::string, int>::kOLSRetain);
            rc = index_list_.push_back(key, val + 2);
            SOCE_DEBUG << _S("overlap strategy", "retain")
                       << _S("action", "push_back")
                       << _S("rc", rc);
            for (auto& i : index_list_){
                SOCE_DEBUG << _S("key", i.first) << _S("val", i.second);
            }
        }

    void overflow()
        {
            index_list_.set_max_size(3);

            // report error
            std::string key = "d";
            std::string key1 = "f";
            int val = 5;
            int rc = index_list_.push_front(key, val);
            SOCE_DEBUG << _S("overflow strategy", "report error") << _S("rc", rc);

            // overlap
            index_list_.set_overflow_strategy(IndexList<std::string, int>::kOFSEraseFront);
            rc = index_list_.push_back(key, val);
            SOCE_DEBUG << _S("overflow strategy", "erase front")
                       << _S("action", "push_back")
                       << _S("rc", rc);
            for (auto& i : index_list_){
                SOCE_DEBUG << _S("key", i.first) << _S("val", i.second);
            }

            // retain
            index_list_.set_overflow_strategy(IndexList<std::string, int>::kOFSEraseBack);
            rc = index_list_.push_front(key1, val);
            SOCE_DEBUG << _S("overflow strategy", "erase back")
                       << _S("action", "push_front")
                       << _S("rc", rc);
            for (auto& i : index_list_){
                SOCE_DEBUG << _S("key", i.first) << _S("val", i.second);
            }
        }

private:
    IndexList<std::string, int> index_list_;
};

void index_list()
{
    ExampleIndexList sil;
    sil.start();
}
