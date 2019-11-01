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

#ifndef _SOCE_LOGGER_KAFbRUWpoD_H_
#define _SOCE_LOGGER_KAFbRUWpoD_H_

#include <string>
#include <memory>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "socestream.h"
#include "sink-if.h"
#include "utils/singleton.hpp"
#include "utils/nocopyable.hpp"

namespace soce{
namespace log4rel{

    class LoggerImpl;

    class StringStream
    {
    public:
        template <class T>
            StringStream& operator<<(const T& val)
        {
            oss_ << val;
            return *this;
        }

        inline const std::string str() {return std::move(oss_.str());}

    private:
        std::ostringstream oss_;
    };


    class Logger : public soce::utils::NoCopyable
    {
    public:
        Logger();
        ~Logger() = default;

        /*
         * Add a sink for the logger, more than one sink is allowed.
         * Each record will be loged to all the sinks.
         * The logger applies a default console sink.
         *
         * @param sink The sink.
         */
        void add_sink(std::shared_ptr<ISink> sink);

        /*
         * Only the record which level is higher than the given level
         * will be sent to the sinks.
         * The default level is kLogLevelDebug, see type.h.
         *
         * @param level The lowest log level.
         */
        void set_log_level(LogLevel level);

        /*
         * Get the current logger level
         *
         * @return The log level
         */
        LogLevel get_log_level();

        /*
         * Only the reserved field will be recored.
         * By default, only kLogFieldTime is on. see type.h
         *
         * @param field The field
         * @param on true on, false off
         */
        void reserve_field(LogField field, bool on);

        /*
         * Chose the format of the record, kLogFmtSimple by default.
         * Another choice is kLogFmtJson. see type.h
         *
         * @param fmt The format of the record
         */
        void set_fmt(LogFmt fmt);

        /*
         * Get the current format
         *
         * @return the current format
         */
        LogFmt get_fmt();

        /*
         * Set/Get the default for macros _D
         */
        void set_default_key(const std::string& dkey);
        std::string get_default_key() const;

        /*
         * Add filter for debug level. see README
         *
         * @param level The log level.
         * @param type Blacklist or white list.
         * @param filter Filter string.
         */
        int add_key_filter(LogFilterType type, const std::string& filter);
        int del_key_filter(LogFilterType type, const std::string& filter);
        int add_record_filter(LogFilterType type, const std::string& filter);
        int del_record_filter(LogFilterType type, const std::string& filter);

        bool match_key(StringStream& key);
        void log(LogLevel level, const std::string& file, const std::string& msg);

        static bool is_enable(){return is_enable_;}
        static void enable(bool e){is_enable_ = e;}

    private:
        std::shared_ptr<LoggerImpl> impl_;
        static bool is_enable_;
    };

    class LoggerMgr : public soce::utils::NoCopyable
    {
    public:
        LoggerMgr();

        int32_t add_logger(const std::string& name, std::shared_ptr<Logger> logger);
        int32_t switch_logger(const std::string& name);
        std::vector<std::shared_ptr<Logger>> get_loggers();
        virtual std::shared_ptr<Logger> get_cur_logger() = 0;

    protected:
        std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
        std::shared_ptr<Logger> cur_logger_;
        bool enable_ = true;
        std::mutex mtx_;
    };

    class GlobalLoggerMgr : public LoggerMgr
    {
    public:
        GlobalLoggerMgr();
        virtual std::shared_ptr<Logger> get_cur_logger();
    };

    class LocalLoggerMgr : public LoggerMgr
    {
    public:
        virtual std::shared_ptr<Logger> get_cur_logger();
    };

    class LoggerRecorder
    {
    public:
        LoggerRecorder(LogLevel level, const char* file, int line);
        ~LoggerRecorder();

        template <class T>
            LoggerRecorder& operator<<(const T& val) {
            oss_ << val;
            return *this;
        }

    private:
        LogLevel level_;
        std::string pos_;
        std::ostringstream oss_;
    };

#define SOCE_GLOBAL_LOGGER_MGR soce::utils::SoceSingleton<soce::log4rel::GlobalLoggerMgr>::get_instance()
#define SOCE_LOGGER_MGR soce::utils::SoceSingletonThreadLocal<soce::log4rel::LocalLoggerMgr>::get_instance()
#define SOCE_CUR_LOGGER soce::utils::SoceSingletonThreadLocal<soce::log4rel::LocalLoggerMgr>::get_instance().get_cur_logger()

/*
 * as string
 */
#define _S(key, val)                                                    \
    [&](std::ostringstream& oss){                                       \
        if (SOCE_CUR_LOGGER->get_fmt() == soce::log4rel::kLogFmtSimple){ \
            oss << ", " << key << " = " << val;                         \
        }                                                               \
        else if (SOCE_CUR_LOGGER->get_fmt() == soce::log4rel::kLogFmtJson){ \
            oss << " \"" << key  << "\" : \"" << val << "\",";          \
        }                                                               \
    }

/*
 * as number
 */
#define _N(key, val)                                                   \
    [&](std::ostringstream& oss){                                       \
    if (SOCE_CUR_LOGGER->get_fmt() == soce::log4rel::kLogFmtSimple){    \
        oss << ", " << key << " = " << (0 + val);                       \
    }                                                                   \
    else if (SOCE_CUR_LOGGER->get_fmt() == soce::log4rel::kLogFmtJson){ \
        oss << " \"" << key  << "\" : " << (0 + val) << ",";            \
    }                                                                   \
    }

/*
 * default key
 */
#define _D(val)                                                         \
    [&](std::ostringstream& oss){                                       \
        if (SOCE_CUR_LOGGER->get_fmt() == soce::log4rel::kLogFmtSimple){ \
            oss << ", " << val;                                         \
        }                                                               \
        else if (SOCE_CUR_LOGGER->get_fmt() == soce::log4rel::kLogFmtJson){ \
            oss << " \"" << SOCE_CUR_LOGGER->get_default_key()          \
                << "\" : \"" << val << "\",";                           \
        }                                                               \
    }

#define KEY_DEBUG(key)                                                  \
    if (!soce::log4rel::Logger::is_enable()                             \
        || SOCE_CUR_LOGGER->get_log_level() > soce::log4rel::kLogLevelDebug \
        || !SOCE_CUR_LOGGER->match_key(soce::log4rel::StringStream() << key)){} \
    else                                                                \
        soce::log4rel::LoggerRecorder(soce::log4rel::kLogLevelDebug, __FILE__, __LINE__)

#define SOCE_LOG(level)                                                 \
    if (!soce::log4rel::Logger::is_enable()                             \
        || SOCE_CUR_LOGGER->get_log_level() > level){} \
    else soce::log4rel::LoggerRecorder(level, __FILE__, __LINE__)

#define SOCE_DEBUG                              \
    SOCE_LOG(soce::log4rel::kLogLevelDebug)

#define SOCE_INFO                               \
    SOCE_LOG(soce::log4rel::kLogLevelInfo)

#define SOCE_WARN                               \
    SOCE_LOG(soce::log4rel::kLogLevelWarn)

#define SOCE_ERROR                              \
    SOCE_LOG(soce::log4rel::kLogLevelError)

#define SOCE_FATAL                              \
    SOCE_LOG(soce::log4rel::kLogLevelFatal)

#define KEY_DEBUG_IF(key, check)                                        \
    if (!soce::log4rel::Logger::is_enable()                             \
        || SOCE_CUR_LOGGER->get_log_level() > soce::log4rel::kLogLevelDebug \
        || [&]()->bool{return !(check);}()                              \
        || (!SOCE_CUR_LOGGER->match_key(soce::log4rel::StringStream() << key)){} \
    else                                                                \
        soce::log4rel::LoggerRecorder(soce::log4rel::kLogLevelDebug, __FILE__, __LINE__)

#define SOCE_LOG_IF(level, check)                                       \
    if (!soce::log4rel::Logger::is_enable()                             \
        || SOCE_CUR_LOGGER->get_log_level() > level                     \
        || [&]()->bool{return !(check);}()){}                           \
    else soce::log4rel::LoggerRecorder(level, __FILE__, __LINE__)

#define SOCE_DEBUG_IF(check)                             \
    SOCE_LOG_IF(soce::log4rel::kLogLevelDebug, check)

#define SOCE_INFO_IF(check)                             \
    SOCE_LOG_IF(soce::log4rel::kLogLevelInfo, check)

#define SOCE_WARN_IF(check)                             \
    SOCE_LOG_IF(soce::log4rel::kLogLevelWarn, check)

#define SOCE_ERROR_IF(check)                            \
    SOCE_LOG_IF(soce::log4rel::kLogLevelError, check)

#define SOCE_FATAL_IF(check)                            \
    SOCE_LOG_IF(soce::log4rel::kLogLevelFatal, check)

} // namespace log4rel
} // namespace soce

#endif
