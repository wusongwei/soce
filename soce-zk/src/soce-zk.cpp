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

#include <unistd.h>
#include "soce-zk/soce-zk.h"
#include "soce-zk-impl.h"

using namespace std;

namespace soce{
namespace zookeeper{

    int SoceZk::init(const string& addr, uint32_t timeout)
    {
        impl_.reset(new SoceZkImpl);
        return impl_->init(addr, timeout);
    }

    int SoceZk::create(const string& path, const string& value, CreateMode mode)
    {
        return impl_->create(path, value, mode);
    }

    int SoceZk::del(const string& path)
    {
        return impl_->del(path);
    }

    int SoceZk::exist(const string& path, WatchMode mode, bool& exist)
    {
        return impl_->exist(path, mode, exist);
    }

    int SoceZk::get(const string& path, WatchMode mode, string& value)
    {
        return impl_->get(path, mode, value);
    }

    int SoceZk::set(const string& path, const string& value)
    {
        return impl_->set(path, value);
    }

    int SoceZk::get_children(const string& path, WatchMode mode, vector<string>& value)
    {
        return impl_->get_children(path, mode, value);
    }

    int SoceZk::get_children_content(const string& path, WatchMode mode, vector<pair<string, string> >& value)
    {
        return impl_->get_children_content(path, mode, value);
    }

    int SoceZk::get_notify_evtfd()
    {
        return impl_->get_notify_evtfd();
    }

    void SoceZk::get_notifies(std::vector<NotifyInfo>& notifies)
    {
        return impl_->get_notifies(notifies);
    }

}} // end namespace
