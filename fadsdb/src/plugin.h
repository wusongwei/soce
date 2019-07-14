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

#ifndef _SOCE_PLUGIN_lqbJt49MI1_H_
#define _SOCE_PLUGIN_lqbJt49MI1_H_

#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include "proto/proto-if.h"
#include "utils/singleton.hpp"

namespace soce{
namespace fadsdb{

    class NodeIf;
    class PluginDetail;

#define UpdatePlugin std::function<int(const std::vector<std::string>&, NodeIf*)>
#define SelectPlugin std::function<std::shared_ptr<NodeIf>(const std::vector<std::string>&, const NodeIf*)>

    class PluginDetail
    {
    public:
        int register_update_func(const std::string& name, UpdatePlugin plugin);
        void unregister_update_func(const std::string& name);
        int get_update_func(const std::string& name, UpdatePlugin& plugin);

        int register_select_func(const std::string& name, SelectPlugin plugin);
        void unregister_select_func(const std::string& name);
        int get_select_func(const std::string& name, SelectPlugin& plugin);

    private:
        std::unordered_map<std::string, UpdatePlugin> update_funcs_;
        std::unordered_map<std::string, SelectPlugin> select_funcs_;
    };

    class Plugin
    {
    public:
        Plugin();
        ~Plugin();

        void add_type(soce::proto::SoceDataType type);

        int register_select_func(soce::proto::SoceDataType type,
                                 const std::string& name,
                                 SelectPlugin plugin);
        void unregister_select_func(soce::proto::SoceDataType type,
                                    const std::string& name);
        int get_select_func(soce::proto::SoceDataType type,
                            const std::string& name,
                            SelectPlugin& plugin);

        int register_update_func(soce::proto::SoceDataType type,
                                 const std::string& name,
                                 UpdatePlugin plugin);
        void unregister_update_func(soce::proto::SoceDataType type,
                                    const std::string& name);
        int get_update_func(soce::proto::SoceDataType type,
                            const std::string& name,
                            UpdatePlugin& plugin);
    private:
        struct EnumClassHash
        {
            template <typename T>
            std::size_t operator()(T t) const
                {
                    return static_cast<std::size_t>(t);
                }
        };

        std::unordered_map<soce::proto::SoceDataType,
            std::shared_ptr<PluginDetail>,
            EnumClassHash> plugs_;
    };

#define SPlugin soce::utils::SoceSingleton<Plugin>::get_instance()

} // namespace fadsdb
} // namespace soce

#endif
