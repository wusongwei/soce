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

#include <cassert>
#include "log4rel/logger.h"
#include "logger-impl.h"
using std::shared_ptr;

namespace soce{
namespace log4rel{

    bool Logger::is_enable_ = true;

    Logger::Logger()
        : impl_(new LoggerImpl)
    {
    }

    void Logger::add_sink(std::shared_ptr<ISink> sink)
    {
        assert(sink);
        impl_->add_sink(sink);
    }

    void Logger::set_log_level(LogLevel level)
    {
        impl_->set_log_level(level);
    }

    LogLevel Logger::get_log_level()
    {
        return impl_->get_log_level();
    }

    void Logger::reserve_field(LogField field, bool on)
    {
        impl_->reserve_field(field, on);
    }

    void Logger::set_fmt(LogFmt fmt)
    {
        impl_->set_fmt(fmt);
    }

    LogFmt Logger::get_fmt()
    {
        return impl_->get_fmt();
    }
    void Logger::set_default_key(const std::string& dkey)
    {
        impl_->set_default_key(dkey);
    }

    const std::string& Logger::get_default_key() const
    {
        return impl_->get_default_key();
    }

    int Logger::add_key_filter(LogFilterType type, const std::string& filter)
    {
        return impl_->add_key_filter(type, filter);
    }

    int Logger::del_key_filter(LogFilterType type, const std::string& filter)
    {
        return impl_->del_key_filter(type, filter);
    }

    int Logger::add_record_filter(LogFilterType type, const std::string& filter)
    {
        return impl_->add_record_filter(type, filter);
    }

    int Logger::del_record_filter(LogFilterType type, const std::string& filter)
    {
        return impl_->del_record_filter(type, filter);
    }

    bool Logger::match_key(StringStream& key)
    {
        return impl_->match_key(key.str());
    }

    void Logger::log(LogLevel level, const std::string& file, const std::string& msg)
    {
        impl_->log(level, file, msg);
    }

    LoggerMgr::LoggerMgr()
    {
        cur_logger_.reset(new Logger);
        add_logger("default", cur_logger_);
#if defined(SOCE_DISABLE_LOG4REL)
        enable_ = false;
#endif
    }

    int32_t LoggerMgr::add_logger(const std::string& name, std::shared_ptr<Logger> logger)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        if (name.empty() || !logger || (loggers_.find(name) != loggers_.end())){
            return -1;
        }
        loggers_[name] = logger;
        return 0;
    }

    int32_t LoggerMgr::switch_logger(const std::string& name)
    {
        std::lock_guard<std::mutex> lck(mtx_);

        auto iter = loggers_.find(name);
        if (iter == loggers_.end()){
            return -1;
        }
        cur_logger_ = iter->second;
        return 0;
    }

    std::vector<std::shared_ptr<Logger>> LoggerMgr::get_loggers()
    {
        std::lock_guard<std::mutex> lck(mtx_);

        std::vector<std::shared_ptr<Logger>> out;
        for (auto& i : loggers_){
            out.push_back(i.second);
        }

        return std::move(out);
    }

    std::shared_ptr<Logger> LoggerMgr::get_cur_logger()
    {
        std::lock_guard<std::mutex> lck(mtx_);
        return cur_logger_;
    }

    LoggerRecorder::LoggerRecorder(LogLevel level, const char* file, int line)
        : level_(level)
    {
        pos_ = std::string(file) + ":" + std::to_string(line);
    }

    LoggerRecorder::~LoggerRecorder()
    {
        SOCE_CUR_LOGGER->log(level_, pos_, oss_.str());
    }

} // namespace log4rel
} // namespace soce
