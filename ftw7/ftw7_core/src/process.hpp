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
#ifndef FTW7_CORE_PROCESS_HPP_INCLUDED
#define FTW7_CORE_PROCESS_HPP_INCLUDED

#include <Windows.h>
#include <string>
#include "ftw7_core/wexcept.hpp"
#include "handle.hpp"

namespace ftw7_core
{

class process_error : public wruntime_error
{
public:
    process_error(const std::wstring& message) : wruntime_error(message) {}
    virtual ~process_error() {}
};

class process
{
public:
    process(const std::wstring& application_name, const std::wstring& cmdline,
        const std::wstring& working_directory);
    ~process();
    DWORD process_id() const { return m_process_id; }
private:
    process(const process&) = delete;
    process& operator = (const process&) = delete;
    void create_process(const std::wstring& application_name,
        const std::wstring& cmdline, const std::wstring& working_directory);
    void kill_if_suspended();

    bool m_is_resumed;
    handle m_process_handle;
    handle m_thread_handle;
    DWORD m_process_id;
};

}

#endif
