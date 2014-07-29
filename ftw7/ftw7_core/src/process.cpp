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
#include <vector>
#include "ftw7_core/windows/bitness.hpp"
#include "ftw7_core/windows/format_message.hpp"
#include "ftw7_core/windows/inserts.hpp"
#include "process.hpp"

namespace ftw7_core
{

namespace
{

std::vector<wchar_t> to_vector(const std::wstring& s)
{
    std::vector<wchar_t> v;
    v.reserve(s.size() + 1);
    v.insert(v.end(), s.begin(), s.end());
    v.push_back(0);
    return v;
}

std::wstring create_error_message_for_create_process(const DWORD errorcode,
    const std::wstring& application_name)
{
    std::wstring message = windows::wformat_message_from_system(errorcode);

    // Replace inserts for known error codes.
    switch (errorcode)
    {
    case ERROR_BAD_EXE_FORMAT:
        message = ftw7_core::windows::replace_inserts(message, application_name);
        break;
    }

    return message;
}

}

process::process(const std::wstring& application_name, const std::wstring& cmdline,
    const std::wstring& working_directory)
    : m_is_resumed(false),
    m_process_handle("process"),
    m_thread_handle("thread"),
    m_process_id(0)
{
    try
    {
        create_process(application_name, cmdline, working_directory);
    }
    catch (...)
    {
        kill_if_suspended();
        throw;
    }
}

process::~process()
{
    kill_if_suspended();
}

bool process::is_64bit()
{
    return windows::is_64bit_process(m_process_handle.get());
}

void process::create_process(const std::wstring& application_name,
    const std::wstring& cmdline, const std::wstring& working_directory)
{
    // CreateProcess needs the command line in a writable buffer.
    auto cmdline_v = to_vector(cmdline);

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};

    const BOOL create_process_result = CreateProcessW(
        application_name.c_str(),   // lpApplicationName
        &cmdline_v[0],              // lpCommandLine
        nullptr,                    // lpProcessAttributes
        nullptr,                    // lpThreadAttributes
        FALSE,                      // bInheritHandles
        CREATE_SUSPENDED,           // dwCreationFlags
        nullptr,                    // lpEnvironment
        working_directory.c_str(),  // lpCurrentDirectory
        &si,                        // lpStartupInfo
        &pi                         // lpProcessInformation
    );

    // Get error code before doing anything else.
    const DWORD error = GetLastError();

    // Stash handles in wrappers so that they're guaranteed to get closed.
    // Also stash additional information from the PROCESS_INFORMATION structure
    // we might want to have around later.
    m_process_handle.set(pi.hProcess);
    m_thread_handle.set(pi.hThread);
    m_process_id = pi.dwProcessId;

    if (!create_process_result)
    {
        throw process_error(
            create_error_message_for_create_process(error, application_name));
    }
}

void process::kill_if_suspended()
{
    // This function gets called by the destructor and thus must not throw.
    if (!m_is_resumed && m_process_handle.is_available())
    {
        TerminateProcess(m_process_handle.get(), EXIT_FAILURE);
    }
}

}
