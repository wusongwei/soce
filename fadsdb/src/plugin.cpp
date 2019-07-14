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

#include "plugin.h"

using soce::proto::SoceDataType;

namespace soce{
namespace fadsdb{

    Plugin::Plugin()
    {
        add_type(SoceDataType::kTypeBool);
        add_type(SoceDataType::kTypeInt32);
        add_type(SoceDataType::kTypeInt64);
        add_type(SoceDataType::kTypeFloat);
        add_type(SoceDataType::kTypeDouble);
        add_type(SoceDataType::kTypeString);
        add_type(SoceDataType::kTypeList);
        add_type(SoceDataType::kTypeSet);
        add_type(SoceDataType::kTypeMap);
        add_type(SoceDataType::kTypeStruct);
    }

    Plugin::~Plugin()
    {
    }

    void Plugin::add_type(soce::proto::SoceDataType type)
    {
        plugs_[type] = std::shared_ptr<PluginDetail>(new PluginDetail);
    }

    int Plugin::register_update_func(soce::proto::SoceDataType type,
                                     const std::string& name,
                                     UpdatePlugin plugin)
    {
        auto iter = plugs_.find(type);
        if (iter == plugs_.end()){
            return -1;
        }

        return iter->second->register_update_func(name, plugin);
    }


    void Plugin::unregister_update_func(soce::proto::SoceDataType type,
                                        const std::string& name)
    {
        auto iter = plugs_.find(type);
        if (iter != plugs_.end()){
            iter->second->unregister_update_func(name);
        }
    }

    int Plugin::get_update_func(soce::proto::SoceDataType type,
                                const std::string& name,
                                UpdatePlugin& plugin)
    {
        auto iter = plugs_.find(type);
        if (iter == plugs_.end()){
            return -1;
        }

        return iter->second->get_update_func(name, plugin);
    }

    int Plugin::register_select_func(soce::proto::SoceDataType type,
                                     const std::string& name,
                                     SelectPlugin plugin)
    {
        auto iter = plugs_.find(type);
        if (iter == plugs_.end()){
            return -1;
        }

        return iter->second->register_select_func(name, plugin);
    }

    void Plugin::unregister_select_func(soce::proto::SoceDataType type,
                                        const std::string& name)
    {
        auto iter = plugs_.find(type);
        if (iter != plugs_.end()){
            iter->second->unregister_select_func(name);
        }
    }

    int Plugin::get_select_func(soce::proto::SoceDataType type,
                                const std::string& name,
                                SelectPlugin& plugin)
    {
        auto iter = plugs_.find(type);
        if (iter == plugs_.end()){
            return -1;
        }

        return iter->second->get_select_func(name, plugin);
    }

    int PluginDetail::register_update_func(const std::string& name, UpdatePlugin plugin)
    {
        int rc = (update_funcs_.find(name) == update_funcs_.end()) ? 0 : 1;
        update_funcs_[name] = plugin;
        return rc;
    }

    void PluginDetail::unregister_update_func(const std::string& name)
    {
        update_funcs_.erase(name);
    }

    int PluginDetail::get_update_func(const std::string& name,
                                      UpdatePlugin& plugin)
    {
        auto iter = update_funcs_.find(name);
        if (iter == update_funcs_.end()){
            return -1;
        }
        plugin = iter->second;
        return 0;
    }

    int PluginDetail::register_select_func(const std::string& name, SelectPlugin plugin)
    {
        int rc = (select_funcs_.find(name) == select_funcs_.end()) ? 0 : 1;
        select_funcs_[name] = plugin;
        return rc;
    }

    void PluginDetail::unregister_select_func(const std::string& name)
    {
        select_funcs_.erase(name);
    }

    int PluginDetail::get_select_func(const std::string& name, SelectPlugin& plugin)
    {
        auto iter = select_funcs_.find(name);
        if (iter == select_funcs_.end()){
            return -1;
        }
        plugin = iter->second;
        return 0;
    }

} // namespace fadsdb
} // namespace soce
