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

}

const log_level_info* log_level_info_begin();
const log_level_info* log_level_info_end();
void initialize(log_level level);

}
}

// Generic logging macro with log level as argument.
#define FTW7_LOG(level) \
    if (!ftw7_core::log::detail::is_enabled((level))); \
        else ftw7_core::log::detail::log_message().buffer((level))

// Convenience logging macros for all the different log levels.
#define FTW7_LOG_ERROR FTW7_LOG(::ftw7_core::log::log_level::error)
#define FTW7_LOG_WARN  FTW7_LOG(::ftw7_core::log::log_level::warn)
#define FTW7_LOG_INFO  FTW7_LOG(::ftw7_core::log::log_level::info)
#define FTW7_LOG_DEBUG FTW7_LOG(::ftw7_core::log::log_level::debug)
#define FTW7_LOG_TRACE FTW7_LOG(::ftw7_core::log::log_level::trace)

#endif
