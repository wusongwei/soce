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

#ifndef _LOGGER_IMPL_20161231_PtxS0NwIif_H_
#define _LOGGER_IMPL_20161231_PtxS0NwIif_H_

#include <string>
#include <functional>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <stdint.h>
#include "utils/rwlock.hpp"
#include "log4rel/type.h"
#include "log4rel/sink-if.h"
#include "filter.h"

namespace soce{
namespace log4rel{

    class LoggerImpl
    {
    public:
        LoggerImpl();
        ~LoggerImpl();

        void add_sink(std::shared_ptr<ISink> sink);
        void set_log_level(LogLevel level);
        LogLevel get_log_level();
        void reserve_field(LogField field, bool rsv);
        void set_fmt(LogFmt fmt);
        LogFmt get_fmt();
        void set_default_key(const std::string& dkey);
        std::string get_default_key();
        int add_key_filter(LogFilterType type, const std::string& filter);
        int del_key_filter(LogFilterType type, const std::string& filter);
        int add_record_filter(LogFilterType type, const std::string& filter);
        int del_record_filter(LogFilterType type, const std::string& filter);
        bool match_key(const std::string& key);
        bool match_record(LogLevel level, const std::string& record);
        void log(LogLevel level, const std::string& file, const std::string& msg);

    private:
        void log_simple_fmt(const struct timeval& tv,
                            LogLevel level,
                            const std::string& pos,
                            const std::string& msg,
                            std::string& slog);
        void log_json_fmt(const struct timeval& tv,
                          LogLevel level,
                          const std::string& pos,
                          const std::string& msg,
                          std::string& slog);

        void log_simple_field_time(const struct timeval& tv, std::string& slog);
        void log_simple_field_level(LogLevel level, std::string& slog);
        void log_simple_field_pos(const std::string& pos, std::string& slog);
        void log_simple_field_pid(std::string& slog);
        void log_simple_field_tid(std::string& slog);

        void log_json_field_time(const struct timeval& tv, std::string& slog);
        void log_json_field_level(LogLevel level, std::string& slog);
        void log_json_field_pos(const std::string& pos, std::string& slog);
        void log_json_field_pid(std::string& slog);
        void log_json_field_tid(std::string& slog);

        inline void log_simple_field_time_null(const struct timeval&, std::string&) {}
        inline void log_simple_field_level_null(LogLevel, std::string&) {}
        inline void log_simple_field_pos_null(const std::string&, std::string&) {}
        inline void log_simple_field_pid_null(std::string&) {}
        inline void log_simple_field_tid_null(std::string&) {}

        inline void log_json_field_time_null(const struct timeval&, std::string&) {}
        inline void log_json_field_level_null(LogLevel, std::string&) {}
        inline void log_json_field_key_null(const std::string&, std::string&) {}
        inline void log_json_field_pos_null(const std::string&, std::string&) {}
        inline void log_json_field_pid_null(std::string&) {}
        inline void log_json_field_tid_null(std::string&) {}

        std::string get_cur_time_str(const struct timeval& tv);
        std::string cvt_log_level(LogLevel level);
        std::string strip_pos(const std::string& file);

    private:
        typedef void(LoggerImpl::*LogFmtPtr)(const struct timeval&,
                                             LogLevel,
                                             const std::string&,
                                             const std::string&,
                                             std::string&);
        typedef void(LoggerImpl::*LogFieldTimePtr)(const struct timeval&, std::string&);
        typedef void(LoggerImpl::*LogFieldLevelPtr)(LogLevel, std::string&);
        typedef void(LoggerImpl::*LogFieldKeyPtr)(const std::string&, std::string&);
        typedef void(LoggerImpl::*LogFieldPosPtr)(const std::string&, std::string&);
        typedef void(LoggerImpl::*LogFieldPidPtr)(std::string&);
        typedef void(LoggerImpl::*LogFieldTidPtr)(std::string&);

        KeyFilter key_filter_;
        RecordFilter record_filter_;
        std::unordered_set<std::shared_ptr<ISink>> sinks_;
        std::unordered_map<LogField, bool, std::hash<int>> fields_;
        LogLevel level_ = kLogLevelDebug;
        LogFmt fmt_ = kLogFmtSimple;
        std::string default_key_= "msg";
        LogFmtPtr log_fmt_;
        LogFieldTimePtr log_time_;
        LogFieldLevelPtr log_level_;
        LogFieldKeyPtr log_key_;
        LogFieldPosPtr log_pos_;
        LogFieldPidPtr log_pid_;
        LogFieldTidPtr log_tid_;
        bool default_sink_ = true;
        soce::utils::RWLock lock_;
    };

} // namespace log4rel
} // namespace log4rel

#endif
