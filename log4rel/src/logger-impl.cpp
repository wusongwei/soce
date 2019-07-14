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

#include <sys/time.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include "logger-impl.h"
#include "log4rel/console-sink.hpp"

using std::string;

namespace soce{
namespace log4rel{

    LoggerImpl::LoggerImpl()
    {
        sinks_.insert(std::shared_ptr<ISink>(new ConsoleSink));

        log_fmt_ = &LoggerImpl::log_simple_fmt;

        reserve_field(kLogFieldTime, true);
        reserve_field(kLogFieldLevel, false);
        reserve_field(kLogFieldPos, false);
        reserve_field(kLogFieldPid, false);
        reserve_field(kLogFieldTid, false);
    }

    LoggerImpl::~LoggerImpl()
    {
    }

    void LoggerImpl::add_sink(std::shared_ptr<ISink> sink)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        if (default_sink_){
            default_sink_ = false;
            sinks_.clear();
        }
        sinks_.insert(sink);
    }

    void LoggerImpl::set_log_level(LogLevel level)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        level_ = level;
    }

    LogLevel LoggerImpl::get_log_level()
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return level_;
    }

    void LoggerImpl::reserve_field(LogField field, bool rsv)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        fields_[field] = rsv;

        if (fmt_ == kLogFmtSimple){
            switch (field){
            case kLogFieldTime:
                log_time_ = rsv ? &LoggerImpl::log_simple_field_time : &LoggerImpl::log_simple_field_time_null;
                break;

            case kLogFieldLevel:
                log_level_ = rsv ? &LoggerImpl::log_simple_field_level : &LoggerImpl::log_simple_field_level_null;
                break;

            case kLogFieldPos:
                log_pos_ = rsv ? &LoggerImpl::log_simple_field_pos : &LoggerImpl::log_simple_field_pos_null;
                break;

            case kLogFieldPid:
                log_pid_ = rsv ? &LoggerImpl::log_simple_field_pid : &LoggerImpl::log_simple_field_pid_null;
                break;

            case kLogFieldTid:
                log_tid_ = rsv ? &LoggerImpl::log_simple_field_tid : &LoggerImpl::log_simple_field_tid_null;
                break;

            default:
                break;
            }
        }
        else if (fmt_ == kLogFmtJson){
            switch (field){
            case kLogFieldTime:
                log_time_ = rsv ? &LoggerImpl::log_json_field_time : &LoggerImpl::log_json_field_time_null;
                break;

            case kLogFieldLevel:
                log_level_ = rsv ? &LoggerImpl::log_json_field_level : &LoggerImpl::log_json_field_level_null;
                break;

            case kLogFieldPos:
                log_pos_ = rsv ? &LoggerImpl::log_json_field_pos : &LoggerImpl::log_json_field_pos_null;
                break;

            case kLogFieldPid:
                log_pid_ = rsv ? &LoggerImpl::log_json_field_pid : &LoggerImpl::log_json_field_pid_null;
                break;

            case kLogFieldTid:
                log_tid_ = rsv ? &LoggerImpl::log_json_field_tid : &LoggerImpl::log_json_field_tid_null;
                break;

            default:
                break;
            }
        }
    }

    void LoggerImpl::set_fmt(LogFmt fmt)
    {
        mtx_.lock();
        fmt_ = fmt;
        if (fmt == kLogFmtSimple){
            log_fmt_ = &LoggerImpl::log_simple_fmt;
        }
        else if (fmt == kLogFmtJson){
            log_fmt_ = &LoggerImpl::log_json_fmt;
        }
        mtx_.unlock();

        for (auto& i : fields_){
            reserve_field(i.first, i.second);
        }
    }

    LogFmt LoggerImpl::get_fmt()
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return fmt_;
    }

    void LoggerImpl::set_default_key(const std::string& dkey)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        default_key_ = dkey;
    }

    const std::string& LoggerImpl::get_default_key()
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return default_key_;
    }

    int LoggerImpl::add_key_filter(LogFilterType type, const std::string& filter)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return key_filter_.add_filter(type, filter);
    }

    int LoggerImpl::del_key_filter(LogFilterType type, const std::string& filter)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return key_filter_.del_filter(type, filter);
    }

    int LoggerImpl::add_record_filter(LogFilterType type, const std::string& filter)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return record_filter_.add_filter(type, filter);
    }

    int LoggerImpl::del_record_filter(LogFilterType type, const std::string& filter)
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return record_filter_.del_filter(type, filter);
    }

    bool LoggerImpl::match_key(const std::string& key)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        return (!key_filter_.match(kLogFilterDeny, key)
                && key_filter_.match(kLogFilterAllow, key));
    }

    bool LoggerImpl::match_record(LogLevel level, const std::string& record)
    {
        if (level != kLogLevelDebug) {
            return true;
        }

        return (!record_filter_.match(kLogFilterDeny, record, false)
                && record_filter_.match(kLogFilterAllow, record, true));
    }

    void LoggerImpl::log(LogLevel level, const std::string& file, const std::string& msg)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        struct timeval tv;
        gettimeofday(&tv, NULL);

        string slog;
        (this->*log_fmt_)(tv, level, file, msg, slog);
        if (!match_record(level, slog)){
            return;
        }

        for (auto& sink : sinks_){
            sink->log(tv, level, slog);
        }
    }

    void LoggerImpl::log_simple_fmt(const struct timeval& tv,
                                    LogLevel level,
                                    const std::string& pos,
                                    const std::string& msg,
                                    string& slog)
    {
        (this->*log_time_)(tv, slog);
        (this->*log_level_)(level, slog);
        (this->*log_pos_)(pos, slog);
        (this->*log_pid_)(slog);
        (this->*log_tid_)(slog);

        slog += msg;
    }

    void LoggerImpl::log_json_fmt(const struct timeval& tv,
                                  LogLevel level,
                                  const std::string& pos,
                                  const std::string& msg,
                                  string& slog)
    {
        slog += "{";

        (this->*log_time_)(tv, slog);
        (this->*log_level_)(level, slog);
        (this->*log_pos_)(pos, slog);
        (this->*log_pid_)(slog);
        (this->*log_tid_)(slog);

        slog += "\"@fields\" : {" + msg;
        slog[slog.size()-1] = '}';
        slog += "}";
    }

    void LoggerImpl::log_simple_field_time(const struct timeval& tv, string& slog)
    {
        slog += get_cur_time_str(tv);
    }

    void LoggerImpl::log_simple_field_level(LogLevel level, string& slog)
    {
        slog += ", " + cvt_log_level(level);
    }

    void LoggerImpl::log_simple_field_pos(const std::string& pos, string& slog)
    {
        slog += ", " + strip_pos(pos);
    }

    void LoggerImpl::log_simple_field_pid(string& slog)
    {
        slog += ", " + std::to_string(getpid());
    }
    void LoggerImpl::log_simple_field_tid(string& slog)
    {
        slog += ", " + std::to_string((long int)syscall(__NR_gettid));
    }

    void LoggerImpl::log_json_field_time(const struct timeval& tv, string& slog)
    {
        slog += "\"@timestamp\" : \"" + get_cur_time_str(tv) + "\", ";
    }

    void LoggerImpl::log_json_field_level(LogLevel level, string& slog)
    {
        slog += "\"@level\" : \"" + cvt_log_level(level) + "\", ";
    }

    void LoggerImpl::log_json_field_pos(const std::string& pos, string& slog)
    {
        slog += "\"@pos\" : \"" + strip_pos(pos) + "\", ";
    }

    void LoggerImpl::log_json_field_pid(string& slog)
    {
        slog += "\"@pid\" : \"" + std::to_string(getpid()) + "\", ";
    }

    void LoggerImpl::log_json_field_tid(string& slog)
    {
        slog += "\"@tid\" : \"" + std::to_string((long int)syscall(__NR_gettid)) + "\", ";
    }

    std::string LoggerImpl::get_cur_time_str(const struct timeval& tv)
    {
        char tm_buff[22];
        time_t tsec = tv.tv_sec;
        tm r;
        memset(&r, 0, sizeof(tm));
        strftime(tm_buff, sizeof(tm_buff), "%F %T", localtime_r(&tsec, &r));

        char timestamp[100] = {0};
        sprintf(timestamp, "%s.%03ld", tm_buff, (long)tv.tv_usec / 1000);

        return string(timestamp);
    }

    std::string LoggerImpl::cvt_log_level(LogLevel level)
    {
        string slev;
        switch (level){
        case kLogLevelDebug:
            slev = "DEBUG";
            break;

        case kLogLevelInfo:
            slev = " INFO";
            break;

        case kLogLevelWarn:
            slev = " WARN";
            break;

        case kLogLevelError:
            slev = "ERROR";
            break;

        case kLogLevelFatal:
            slev = "FATAL";
            break;

        default:
            slev = "UNKNOWN";
            break;
        }

        return move(slev);
    }

    std::string LoggerImpl::strip_pos(const std::string& file)
    {
        size_t pos = file.find_last_of('/');
        return (pos != std::string::npos) ? file.substr(pos + 1) : file;
    }

} // namespace log4rel
} // namespace log4rel
