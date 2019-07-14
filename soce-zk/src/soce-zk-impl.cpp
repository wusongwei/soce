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

#include <vector>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/eventfd.h>
#include "soce-zk-impl.h"

using std::vector;
using namespace std;

namespace soce{
namespace zookeeper{

#define ASSERT_ZK_HANDLER()                     \
    if (!handle_){                              \
        return kInvalidHandler;                 \
    }

    static void global_watcher(zhandle_t* zh, int type, int state,
                              const char* path, void* watcherCtx) {
        SoceZk::SoceZkImpl* pZkImpl = static_cast<SoceZk::SoceZkImpl*>(watcherCtx);
        SOCE_LOGGER_MGR.add_logger("soce_zk", pZkImpl->get_logger());
        SOCE_LOGGER_MGR.switch_logger("soce_zk");

        SOCEZK_DEBUG << _N("type", type) << _N("state", state);
        if (type == ZOO_SESSION_EVENT){
            if (state == ZOO_CONNECTED_STATE){
                pZkImpl->do_con_evt();
            }
            else if (state == ZOO_EXPIRED_SESSION_STATE){
                pZkImpl->do_exp_evt();
            }
        }
        else if (type == ZOO_CREATED_EVENT){
            pZkImpl->do_crt_evt(path);
        }
        else if (type == ZOO_DELETED_EVENT){
            pZkImpl->do_del_evt(path);
        }
        else if (type == ZOO_CHANGED_EVENT){
            pZkImpl->do_chg_evt(path);
        }
        else if (type == ZOO_CHILD_EVENT){
            pZkImpl->do_cld_evt(path);
        }
        else{
        }
    }

    void NotifyHelper::push_exist(const std::string& path, bool exist)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        NotifyInfo ni;
        notifies_.push_back(ni);
        notifies_.back().type = kTypeExist;
        notifies_.back().path = path;
        notifies_.back().exist = exist;

        notify();
    }

    void NotifyHelper::push_get(const std::string& path, std::string&& value)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        NotifyInfo ni;
        notifies_.push_back(ni);
        notifies_.back().type = kTypeGet;
        notifies_.back().path = path;
        notifies_.back().value = std::move(value);

        notify();
    }

    void NotifyHelper::push_get_cld(const std::string& path, std::vector<std::string>&& value)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        NotifyInfo ni;
        notifies_.push_back(ni);
        notifies_.back().type = kTypeGetCld;
        notifies_.back().path = path;
        notifies_.back().cld = std::move(value);

        notify();
    }

    void NotifyHelper::push_get_cld_cnt(const std::string& path, std::vector<std::pair<std::string, std::string>>&& value)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        NotifyInfo ni;
        notifies_.push_back(ni);
        notifies_.back().type = kTypeGetCldCnt;
        notifies_.back().path = path;
        notifies_.back().cldcnt = std::move(value);

        notify();
    }

    void NotifyHelper::get_notifies(std::vector<NotifyInfo>& notifies)
    {
        notifies.clear();
        std::lock_guard<std::mutex> lck(mtx_);
        notifies.swap(notifies_);
        notified_ = false;
    }

    void NotifyHelper::notify()
    {
        if (notified_){
            return;
        }
        notified_ = true;
        uint64_t ct = 1;
        (void)write(notify_evtfd_, (void*)&ct, sizeof(uint64_t));
    }

    EvtWatcher::EvtWatcher()
    {
    }

    void EvtWatcher::add(const string& path,
                        WatchMode mode,
                        OperType type,
                        const string& value){
        std::lock_guard<std::mutex> lck(mtx_);

        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode) << _S("type", type);
        WatcherInfo wi;
        wi.mode = mode;
        wi.value = value;
        watchers_[path][type] = std::move(wi);
    }

    void EvtWatcher::del(const string& path){
        std::lock_guard<std::mutex> lck(mtx_);

        SOCEZK_DEBUG << _S("path", path);
        watchers_.erase(path);
    }

    void EvtWatcher::del(const string& path, OperType op_type){
        std::lock_guard<std::mutex> lck(mtx_);

        SOCEZK_DEBUG << _S("path", path) << _S("type", op_type);

        auto iter = watchers_.find(path);
        if (iter != watchers_.end()){
            iter->second.erase(op_type);
            if (iter->second.empty()){
                watchers_.erase(iter);
            }
        }
    }

    EvtWatcher::Watcher_t EvtWatcher::get(const std::string& path)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        SOCEZK_DEBUG << _S("path", path);

        Watcher_t watchers;
        auto iter = watchers_.find(path);
        if (iter != watchers_.end()){
            watchers = iter->second;
        }

        return std::move(watchers);
    }

    SoceZk::SoceZkImpl::SoceZkImpl()
    {
        logger_ = SOCE_CUR_LOGGER;
    }

    SoceZk::SoceZkImpl::~SoceZkImpl()
    {
        if (handle_){
            zookeeper_close(handle_);
            handle_ = NULL;
        }
    }

    int SoceZk::SoceZkImpl::init(const std::string& addr, uint32_t timeout)
    {
        SOCEZK_DEBUG << _S("addr", addr);
        int fd = eventfd(0, 0);
        if (fd == -1){
            return kCrtEvtfd;
        }

        std::lock_guard<std::mutex> lck(mtx_);

        notify_helper_.set_notify_evtfd(fd);
        addr_ = addr;
        timeout_ = timeout;

        handle_ = zookeeper_init(addr.c_str(), global_watcher, timeout, 0, this, 0);
        if (!handle_){
            return kInvalidHandler;
        }

        uint32_t wait = timeout * 1000;
        while (zoo_state(handle_) != ZOO_CONNECTED_STATE){
            if (wait == 0){
                return kTimeout;
            }
            usleep(1000);
            --wait;
        }

        return OK;
    }

    void SoceZk::SoceZkImpl::unwatch(const std::string& path)
    {
        SOCEZK_DEBUG << _S("path", path);

        std::lock_guard<std::mutex> lck(mtx_);
        evt_wc_.del(path);
    }

    void SoceZk::SoceZkImpl::unwatch(const std::string& path, OperType op_type)
    {
        SOCEZK_DEBUG << _S("path", path) << _S("oper", op_type);

        std::lock_guard<std::mutex> lck(mtx_);
        evt_wc_.del(path, op_type);
    }

    int SoceZk::SoceZkImpl::create(const std::string& path,
                                   const std::string& value,
                                   CreateMode mode)
    {
        ASSERT_ZK_HANDLER();

        SOCEZK_DEBUG << _S("path", path) << _S("value", value) << _S("mode", mode);

        std::lock_guard<std::mutex> lck(mtx_);
        if (create_dir(path)){
            return kCrtDir;
        }

        int flags = 0;
        if (mode == kCrtModeEphemeral){
            flags = ZOO_EPHEMERAL;
        }
        else if (mode == kCrtModeSequence){
            flags = ZOO_SEQUENCE;
        }
        else if (mode == kCrtModeEphSeq){
            flags = ZOO_EPHEMERAL | ZOO_SEQUENCE;
        }

        int rc = zoo_create(handle_,
                            path.c_str(),
                            value.c_str(),
                            value.size(),
                            &ZOO_OPEN_ACL_UNSAFE,
                            flags,
                            NULL,
                            0);

        switch (rc){
        case ZOK:
            rc = OK;
            break;

        case ZNODEEXISTS:
            rc = kNodeExist;
            break;

        default:
            rc = kZkError;
            break;
        }

        if (mode == kCrtModeEphemeral){
            evt_wc_.add(path, kWatchModeRepeat, kOperTypeCrtEph, value);
            add_watch_oper_none(path);
        }
        else if (mode == kCrtModeEphSeq){
            evt_wc_.add(path, kWatchModeRepeat, kOperTypeCrtEphSeq, value);
            add_watch_oper_none(path);
        }
        return rc;
    }

    int SoceZk::SoceZkImpl::del(const std::string& path)
    {
        ASSERT_ZK_HANDLER();

        SOCEZK_DEBUG << _S("path", path);

        std::lock_guard<std::mutex> lck(mtx_);
        evt_wc_.del(path);
        int rc = zoo_delete(handle_, path.c_str(), -1);
        if (rc == ZNONODE){
            return kNoNode;
        }
        else if (rc != ZOK){
            return kZkError;
        }
        return OK;
    }

    int SoceZk::SoceZkImpl::exist(const std::string& path, WatchMode mode, bool& exist)
    {
        ASSERT_ZK_HANDLER();

        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        std::lock_guard<std::mutex> lck(mtx_);
        int watch = 0;
        if (mode != kWatchModeNone){
            watch = 1;
            evt_wc_.add(path, mode, kOperTypeExt, string());
        }

        int rc = zoo_exists(handle_, path.c_str(), watch, NULL);
        if (rc == ZOK){
            exist = true;
            rc = OK;
        }
        else if (rc == ZNONODE){
            exist = false;
            rc = OK;
        }
        else{
            rc = kZkError;
        }

        SOCEZK_DEBUG << _N("rc", rc) << _S("exist", exist);
        return rc;
    }

    int SoceZk::SoceZkImpl::get(const std::string& path, WatchMode mode, std::string& value)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return get_unsafe(path, mode, value);
    }

    int SoceZk::SoceZkImpl::set(const std::string& path, const std::string& value)
    {
        ASSERT_ZK_HANDLER();
        SOCEZK_DEBUG << _S("path", value);

        std::lock_guard<std::mutex> lck(mtx_);
        int rc = zoo_set(handle_, path.c_str(), value.c_str(), value.size(), -1);
        if (rc == ZNONODE){
            return kNoNode;
        }
        else if (rc != ZOK){
            return kZkError;
        }

        return OK;
    }

    int SoceZk::SoceZkImpl::get_children(const std::string& path, WatchMode mode, std::vector<std::string>& value)
    {
        ASSERT_ZK_HANDLER();

        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        std::lock_guard<std::mutex> lck(mtx_);
        int watch = 0;
        if (mode != kWatchModeNone){
            watch = 1;
            evt_wc_.add(path, mode, kOperTypeGetCld, string());
        }

        add_watch_oper_none(path);

        struct String_vector strings;
        int rc = zoo_get_children(handle_, path.c_str(), watch, &strings);
        if (rc == ZNONODE){
            return kNoNode;
        }
        else if (rc != ZOK){
            return kZkError;
        }

        for (int32_t i=0; i<strings.count; ++i){
            value.push_back(*(strings.data + i));
            SOCEZK_DEBUG << _S("child", *(strings.data + i));
        }

        return OK;
    }

    int SoceZk::SoceZkImpl::get_children_content(const std::string& path,
                                                 WatchMode mode,
                                                 std::vector<std::pair<std::string, std::string>>& value)
    {
        ASSERT_ZK_HANDLER();

        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        std::lock_guard<std::mutex> lck(mtx_);
        int watch = 0;
        if (mode != kWatchModeNone){
            watch = 1;
            evt_wc_.add(path, mode, kOperTypeGetCldCnt, string());
        }

        add_watch_oper_none(path);

        struct String_vector strings;
        int rc = zoo_get_children(handle_, path.c_str(), watch, &strings);
        if (rc == ZNONODE){
            return kNoNode;
        }
        else if (rc != ZOK){
            return kZkError;
        }

        for (int32_t i=0; i<strings.count; ++i){
            string child_value;
            string full_path = path + "/" + *(strings.data + i);
            int r = get_unsafe(full_path, kWatchModeNone, child_value);
            if (r != OK){
                return r;
            }

            value.push_back(make_pair(*(strings.data + i), child_value));
            SOCEZK_DEBUG << _S("child", *(strings.data + i)) << _S("value", child_value);
        }

        return OK;
    }

    void SoceZk::SoceZkImpl::do_con_evt()
    {
        for (auto& i : evt_wc_.get_watchers()){
            handle_event(i.first, ses_evt_);
        }
    }

    void SoceZk::SoceZkImpl::do_exp_evt()
    {
        if (handle_){
            zookeeper_close(handle_);
        }

        handle_ = zookeeper_init(addr_.c_str(), global_watcher, timeout_, 0, this, 0);
    }

    void SoceZk::SoceZkImpl::do_crt_evt(const std::string& path)
    {
        handle_event(path, crt_evt_);
    }

    void SoceZk::SoceZkImpl::do_del_evt(const std::string& path)
    {
        handle_event(path, del_evt_);
    }

    void SoceZk::SoceZkImpl::do_chg_evt(const std::string& path)
    {
        handle_event(path, chg_evt_);
    }

    void SoceZk::SoceZkImpl::do_cld_evt(const std::string& path)
    {
        handle_event(path, cld_evt_);
    }

    void SoceZk::SoceZkImpl::handle_event(const std::string& path, std::set<OperType>& influence)
    {
        EvtWatcher::Watcher_t watchers = evt_wc_.get(path);
        if (watchers.empty()){
            return;
        }

        string value;
        vector<string> children;
        for (auto& i : watchers){
            auto iter = influence.find(i.first);
            if (iter == influence.end()){
                continue;
            }

            switch (i.first){
            case kOperTypeExt:
                handle_ext_evt(path, i.second.mode);
                break;

            case kOperTypeCrtEph:
                handle_crt_evt(path, i.second.value, false);
                break;

            case kOperTypeCrtEphSeq:
                handle_crt_evt(path, i.second.value, true);
                break;

            case kOperTypeGet:
                handle_get_evt(path, i.second.mode);
                break;

            case kOperTypeGetCld:
                handle_get_cld_evt(path, i.second.mode);
                break;

            case kOperTypeGetCldCnt:
                handle_get_cld_cnt_evt(path, i.second.mode);
                break;

            case kOperTypeNone:
                zoo_exists(handle_, path.c_str(), 1, NULL);
                break;

            default:
                break;
            }
        }
    }

    void SoceZk::SoceZkImpl::handle_ext_evt(const std::string& path, WatchMode mode)
    {
        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        if (mode == kWatchModeOnce){
            evt_wc_.del(path, kOperTypeExt);
            mode = kWatchModeNone;
        }

        bool e = false;
        if (exist(path, mode, e)){
            // todo:log
            return;
        }

        SOCEZK_DEBUG << _S("exist", e);

        notify_helper_.push_exist(path, e);
    }

    void SoceZk::SoceZkImpl::handle_crt_evt(const std::string& path,
                                            const std::string& value,
                                            bool seq)
    {
        SOCEZK_DEBUG << _S("path", path) << _S("value", value) << _S("seq", seq);
        if (seq){
            create(path, value, kCrtModeEphSeq);
        }
        else{
            create(path, value, kCrtModeEphemeral);
        }
    }

    void SoceZk::SoceZkImpl::handle_get_evt(const std::string& path, WatchMode mode)
    {
        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        if (mode == kWatchModeOnce){
            evt_wc_.del(path, kOperTypeGet);
            mode = kWatchModeNone;
        }

        string value;
        if (get(path, mode, value)){
            return;
        }

        notify_helper_.push_get(path, std::move(value));
    }

    void SoceZk::SoceZkImpl::handle_get_cld_evt(const std::string& path, WatchMode mode)
    {
        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        if (mode == kWatchModeOnce){
            evt_wc_.del(path, kOperTypeGetCld);
            mode = kWatchModeNone;
        }

        vector<string> value;
        get_children(path, mode, value);

        SOCEZK_DEBUG << _S("path", path) << _N("size", value.size());

        notify_helper_.push_get_cld(path, std::move(value));
    }

    void SoceZk::SoceZkImpl::handle_get_cld_cnt_evt(const std::string& path, WatchMode mode)
    {
        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        if (mode == kWatchModeOnce){
            evt_wc_.del(path, kOperTypeGetCldCnt);
            mode = kWatchModeNone;
        }

        std::vector<std::pair<std::string, std::string>> value;
        get_children_content(path, mode, value);

        notify_helper_.push_get_cld_cnt(path, std::move(value));
    }

    int SoceZk::SoceZkImpl::get_notify_evtfd()
    {
        return notify_helper_.get_notify_evtfd();
    }

    void SoceZk::SoceZkImpl::get_notifies(std::vector<NotifyInfo>& notifies)
    {
        return notify_helper_.get_notifies(notifies);
    }

    int SoceZk::SoceZkImpl::get_unsafe(const std::string& path, WatchMode mode, std::string& value)
    {
        ASSERT_ZK_HANDLER();

        SOCEZK_DEBUG << _S("path", path) << _S("mode", mode);

        int watch = 0;
        if (mode != kWatchModeNone){
            watch = 1;
            evt_wc_.add(path, mode, kOperTypeGet, string());
        }

        add_watch_oper_none(path);

        // todo : magic number
        static char buff[1024];
        int len = 1024;
        memset(buff, 0, 1024);

        struct Stat stat;
        int rc = zoo_get(handle_, path.c_str(), watch, buff, &len, &stat);
        if (rc == ZNONODE){
            return kNoNode;
        }
        else if (rc != ZOK){
            return kZkError;
        }

        value = string(buff, len);
        SOCEZK_DEBUG << _S("value", value);
        return OK;
    }

    void SoceZk::SoceZkImpl::add_watch_oper_none(const std::string& path)
    {
        evt_wc_.add(path, kWatchModeRepeat, kOperTypeNone, string());
        zoo_exists(handle_, path.c_str(), 1, NULL);
    }

    int SoceZk::SoceZkImpl::create_dir(const std::string& path)
    {
        if (path.empty() || path[0] != '/' || path[path.size()-1] == '/'){
            return -1;
        }

        size_t pos = 1;
        while (1){
            pos = path.find_first_of('/', pos);
            if (pos == string::npos){
                return 0;
            }
            else{
                zoo_create(handle_, path.substr(0, pos).c_str(), NULL, 0,
                           &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
                ++pos;
            }
        }
        return 0;
    }

#undef ASSERT_ZK_HANDLER

}} // end namespace
