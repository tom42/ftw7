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
#include <Windows.h>
#include <algorithm>
#include <atomic>
#include <ctime>
#include "ftw7_core/log/log.hpp"

namespace ftw7_core
{
namespace log
{
namespace detail
{

const char BAD_LOG_LEVEL[] = "?????";

// Order of entries must match log_level member values,
// since values of type log_level are used as array index.
const log_level_info log_levels[] =
{
    { log_level::off,   "off",   BAD_LOG_LEVEL },
    { log_level::error, "error", "ERROR" },
    { log_level::warn,  "warn",  "WARN " },
    { log_level::info,  "info",  "INFO " },
    { log_level::debug, "debug", "DEBUG" },
};
const int n_log_levels = sizeof(log_levels) / sizeof(log_levels[0]);
log_level current_log_level = log_level::off;

const char* to_string(log_level level)
{
    if ((level < 0) || (level >= n_log_levels))
    {
        return BAD_LOG_LEVEL;
    }
    return log_levels[level].log_name;
}

void get_timestamp(char* buf, size_t bufsize)
{
    static const char time_format[] = "%Y-%m-%d %H:%M:%S";
    static const char unknown_time[] = "????-??-?? ??:??:??";

    time_t timestamp = time(nullptr);
    if (timestamp != -1)
    {
        tm local_timestamp;
        if (!localtime_s(&local_timestamp, &timestamp))
        {
            if (strftime(buf, bufsize, time_format, &local_timestamp))
            {
                return;
            }
        }
    }

    // Could not obtain and/or convert time. Copy unknown_time to buf.
    const auto nchars = std::min(bufsize, sizeof(unknown_time) / sizeof(unknown_time[0]));
    strncpy(buf, unknown_time, nchars);
    buf[nchars - 1] = '\0';
}

log_message::~log_message()
{
    try
    {
        // Finalize log message.
        // Call str() only once since it returns a copy of the stream's content.
        m_buffer << std::endl;
        const auto message = m_buffer.str();

        // The Visual Studio 2013 documentation states for both fwrite and fflush
        // that they lock the calling thread and are therefore thread-safe.
        // We only assume here that this is the case for fwprintf too.
        fwprintf(stdout, L"%s", message.c_str());
        fflush(stdout);
    }
    catch (...)
    {
    }
}

std::wostringstream& log_message::buffer(log_level level)
{
    const size_t timestamp_bufsize = 128;
    char timestamp_buf[timestamp_bufsize];
    get_timestamp(timestamp_buf, timestamp_bufsize);

    m_buffer << '[' << to_string(level) << "] "
        << timestamp_buf << ' '
        << GetCurrentThreadId() << ' ';

    return m_buffer;
}

}

const log_level_info* log_level_info_begin()
{
    return &detail::log_levels[0];
}

const log_level_info* log_level_info_end()
{
    return &detail::log_levels[detail::n_log_levels];
}

void initialize(log_level level)
{
    static std::atomic<bool> is_initialized(false);
    if (!is_initialized.exchange(true))
    {
        detail::current_log_level = level;
    }
}

}
}
