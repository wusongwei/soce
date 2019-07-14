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

#ifndef _SOCE_ZKCPP_IMPL_F7JQmIYuZy_H_
#define _SOCE_ZKCPP_IMPL_F7JQmIYuZy_H_

#include <string>
#include <map>
#include <set>
#include <zookeeper/zookeeper.h>
#include <mutex>
#include "soce-zk/soce-zk.h"
#include "soce-zk/types.h"
#include "soce-zk/log.h"

using std::string;
using std::map;
using std::set;

namespace soce{
namespace zookeeper{

    class NotifyHelper
    {
    public:
        void push_exist(const std::string& path, bool exist);
        void push_get(const std::string& path, std::string&& value);
        void push_get_cld(const std::string& path, std::vector<std::string>&& value);
        void push_get_cld_cnt(const std::string& path, std::vector<std::pair<std::string, std::string>>&& value);

        inline void set_notify_evtfd(int fd)
        {
            notify_evtfd_ = fd;
        }

        inline int get_notify_evtfd()
        {
            return notify_evtfd_;
        }

       void get_notifies(std::vector<NotifyInfo>& notifies);

    private:
       void notify();

    private:
        int notify_evtfd_ = 0;
        std::mutex mtx_;
        std::vector<NotifyInfo> notifies_;
        bool notified_ = false;
    };

    typedef enum{
        kOperTypeExt,
        kOperTypeCrtEph,
        kOperTypeCrtEphSeq,
        kOperTypeGet,
        kOperTypeGetCld,
        kOperTypeGetCldCnt,
        kOperTypeNone
    }OperType;

    class EvtWatcher{
    public:
        using WatcherInfo = struct{
            WatchMode mode;
            string value;
        };

        using Watcher_t = std::map<OperType, WatcherInfo>;

    public:
        EvtWatcher();
        EvtWatcher(const EvtWatcher&) = delete;
        EvtWatcher& operator=(const EvtWatcher&) = delete;

        void add(const std::string& path,
                 WatchMode mode,
                 OperType type,
                 const std::string& value);
        void del(const std::string& path);
        void del(const std::string& path, OperType op_type);
        Watcher_t get(const std::string& path);

        inline std::map<std::string, Watcher_t>& get_watchers(){
            return watchers_;
        }

    private:
        std::map<std::string, Watcher_t> watchers_;
        std::mutex mtx_;
    };

    class SoceZk::SoceZkImpl
    {
    public:
        SoceZkImpl();
        ~SoceZkImpl();

        int init(const std::string& addr, uint32_t timeout);
        void unwatch(const std::string& path);
        void unwatch(const std::string& path, OperType op_type);
        int create(const std::string& path,
                   const std::string& value,
                   CreateMode mode);
        int del(const std::string& path);
        int exist(const std::string& path, WatchMode mode, bool& exist);
        int get(const std::string& path, WatchMode mode, std::string& value);
        int set(const std::string& path, const std::string& value);
        int get_children(const std::string& path, WatchMode mode, std::vector<std::string>& value);
        int get_children_content(const std::string& path, WatchMode mode, std::vector<std::pair<std::string, std::string>>& value);

        int get_notify_evtfd();
        void get_notifies(std::vector<NotifyInfo>& notifies);

        int get_unsafe(const std::string& path, WatchMode mode, std::string& value);
        void do_con_evt();
        void do_exp_evt();
        void do_crt_evt(const std::string& path);
        void do_del_evt(const std::string& path);
        void do_chg_evt(const std::string& path);
        void do_cld_evt(const std::string& path);

        std::shared_ptr<soce::log4rel::Logger> get_logger(){return logger_;}

    private:
        void handle_event(const std::string& path, std::set<OperType>& influence);
        void handle_ext_evt(const std::string& path, WatchMode mode);
        void handle_crt_evt(const std::string& path, const std::string& value, bool seq);
        void handle_get_evt(const std::string& path, WatchMode mode);
        void handle_get_cld_evt(const std::string& path, WatchMode mode);
        void handle_get_cld_cnt_evt(const std::string& path, WatchMode mode);
        void add_watch_oper_none(const std::string& path);
        int create_dir(const std::string& path);

    private:
        zhandle_t* handle_ = NULL;
        std::string addr_;
        uint32_t timeout_ = 0;
        std::mutex mtx_;
        NotifyHelper notify_helper_;
        std::shared_ptr<soce::log4rel::Logger> logger_;

        EvtWatcher evt_wc_;
        std::set<OperType> ses_evt_{kOperTypeExt,
                kOperTypeCrtEph,
                kOperTypeCrtEphSeq,
                kOperTypeGet,
                kOperTypeGetCld,
                kOperTypeGetCldCnt,
                kOperTypeNone};
        std::set<OperType> crt_evt_{kOperTypeExt,
                kOperTypeGet,
                kOperTypeGetCld,
                kOperTypeGetCldCnt,
                kOperTypeNone};
        std::set<OperType> del_evt_{kOperTypeExt,
                kOperTypeCrtEph,
                kOperTypeCrtEphSeq,
                kOperTypeGetCld,
                kOperTypeGetCldCnt,
                kOperTypeNone};
        std::set<OperType> chg_evt_{kOperTypeExt, kOperTypeGet};
        std::set<OperType> cld_evt_{kOperTypeGetCld, kOperTypeGetCldCnt};
    };

}} // end namespace
#endif
