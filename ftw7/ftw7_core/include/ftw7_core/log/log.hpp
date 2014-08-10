/*
* Copyright 2012-2014 Thomas Mathys
*
* This file is part of ftw7.
*
* ftw7 is free software : you can redistribute it and / or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ftw7 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ftw7.If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef FTW7_CORE_LOG_LOG_HPP_INCLUDED
#define FTW7_CORE_LOG_LOG_HPP_INCLUDED

#include <sstream>

namespace ftw7_core
{
namespace log
{

// Log levels must start at zero and have continuous numbering.
enum log_level
{
    // Special level intended to disable logging.
    // Should not be used as level for actual log messages.
    off,
    error,
    warn,
    info,
    debug,
    trace
};

struct log_level_info
{
    log_level level;
    const char* console_display_name;
    const char* log_name;
};

class logger
{
public:
    virtual ~logger() = default;
    virtual void write(const wchar_t* s, size_t nchars) = 0;
};

class stdout_logger : public logger
{
public:
    virtual ~stdout_logger() = default;
    virtual void write(const wchar_t* s, size_t nchars) override;
};

namespace detail
{

extern log_level current_log_level;

inline bool is_enabled(log_level level)
{
    return level <= current_log_level;
}

class log_message
{
public:
    log_message() {}
    ~log_message();
    std::wostringstream& buffer(log_level level);
private:
    log_message(const log_message&) = delete;
    log_message& operator = (const log_message&) = delete;
    std::wostringstream m_buffer;
};

template <typename T>
void trace_arg(std::wostream& os, T arg)
{
    os << arg;
}

// Do not attempt to trace const/non-const char/wchar_t pointers as strings.
// Since the pointers are coming from arbitrary programs, they might be null,
// be non-terminated or point to non-readable memory.
inline void trace_arg(std::wostream& os, char* s)
{
    os << static_cast<const void*>(s);
}
inline void trace_arg(std::wostream& os, const char* s)
{
    os << static_cast<const void*>(s);
}
inline void trace_arg(std::wostream& os, wchar_t* s)
{
    os << static_cast<const void*>(s);
}
inline void trace_arg(std::wostream& os, const wchar_t* s)
{
    os << static_cast<const void*>(s);
}

inline void trace_args(std::wostream&)
{
    // Helper function to stop recursion.
}

template <typename T, typename... Args>
void trace_args(std::wostream& os, T value, Args... args)
{
    os << ' ';
    trace_arg(os, value);
    trace_args(os, args...);
}

template <typename... Args>
void trace_api_call(const wchar_t* function, Args... args)
{
    // Create log message the normal way.
    log_message msg;
    auto& buf = msg.buffer(log_level::trace);

    // Log function name, then recursively print args.
    buf << function;
    trace_args(buf, args...);
}

}

const log_level_info* log_level_info_begin();
const log_level_info* log_level_info_end();
void initialize(log_level level);

}
}

// Generic logging macro with log level as argument.
#define FTW7_LOG(level)                                                 \
    if (!::ftw7_core::log::detail::is_enabled((level)));                \
        else ::ftw7_core::log::detail::log_message().buffer((level))

// Convenience logging macros for all the different log levels.
#define FTW7_LOG_ERROR FTW7_LOG(::ftw7_core::log::log_level::error)
#define FTW7_LOG_WARN  FTW7_LOG(::ftw7_core::log::log_level::warn)
#define FTW7_LOG_INFO  FTW7_LOG(::ftw7_core::log::log_level::info)
#define FTW7_LOG_DEBUG FTW7_LOG(::ftw7_core::log::log_level::debug)
#define FTW7_LOG_TRACE FTW7_LOG(::ftw7_core::log::log_level::trace)

// Special logging macro to trace API calls (calls to hooked functions)
#define FTW7_TRACE_API_CALL(...)                                                    \
    if (::ftw7_core::log::detail::is_enabled(::ftw7_core::log::log_level::trace))   \
        ::ftw7_core::log::detail::trace_api_call(__FUNCTIONW__, __VA_ARGS__)

#endif
