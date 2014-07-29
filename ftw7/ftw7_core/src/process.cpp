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

CONTEXT process::get_thread_context(DWORD context_flags)
{
    check_is_not_resumed(__FUNCTION__);

    CONTEXT ctx = { context_flags };
    if (!GetThreadContext(m_thread_handle.get(), &ctx))
    {
        const DWORD error = GetLastError();
        throw process_error(L"could not get thread context: " +
            windows::wformat_message_from_system(error));
    }

    return ctx;
}

void* process::virtual_alloc(size_t nbytes)
{
    check_is_not_resumed(__FUNCTION__);

    // TODO: can a 32 bit process do this on a 64 bit process?
    //       And what about 64 -> 32 bit?
    void* address = VirtualAllocEx(m_process_handle.get(), nullptr, nbytes,
        MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!address)
    {
        const DWORD error = GetLastError();
        throw process_error(windows::wformat_message_from_system(
            L"could not allocate memory for injected code", error));
    }
    return address;
}

void process::write_process_memory(void* base_address, const void* buffer,
    const size_t nbytes)
{
    check_is_not_resumed(__FUNCTION__);

    // TODO: can a 32 bit process do this on a 64 bit process?
    //       And what about 64 -> 32 bit?
    SIZE_T nbytes_written = 0;
    if (!WriteProcessMemory(m_process_handle.get(), base_address, buffer, nbytes,
        &nbytes_written))
    {
        const DWORD error = GetLastError();
        throw process_error(L"WriteProcessMemory failed: " +
            windows::wformat_message_from_system(error));
    }

    // This should never happen if we write to memory we allocated ourself
    // and made writable. Being paranoid can't hurt, either.
    if (nbytes_written != nbytes)
    {
        throw process_error(L"WriteProcessMemory failed: "
            L"could not write specified number of bytes to remote process");
    }
}

void process::flush_instruction_cache(const void* base_address, size_t nbytes)
{
    check_is_not_resumed(__FUNCTION__);

    if (!FlushInstructionCache(m_process_handle.get(), base_address, nbytes))
    {
        const DWORD error = GetLastError();
        throw process_error(L"FlushInstructionCache failed: " +
            windows::wformat_message_from_system(error));
    }
}

void process::set_thread_context(const CONTEXT& ctx)
{
    check_is_not_resumed(__FUNCTION__);

    if (!SetThreadContext(m_thread_handle.get(), &ctx))
    {
        const DWORD error = GetLastError();
        throw process_error(L"SetThreadContext failed: " +
            windows::wformat_message_from_system(error));
    }
}

void process::run()
{
    check_is_not_resumed(__FUNCTION__);

    const DWORD result = ResumeThread(m_thread_handle.get());
    switch (result)
    {
    case 0:
        // Should not happen unless somebody else already resumed the thread.
        throw process_error(L"main thread was not suspended");
        break;
    case 1:
        // OK: thread was suspended and is now running.
        m_is_resumed = true;
        m_thread_handle.close();
        break;
    case (DWORD)-1:
        // TODO: shouldn't we supply a message prefix here?
        throw process_error(windows::wformat_message_from_system(GetLastError()));
        break;
    default:
        // Should not happen unless somebody else also suspended the thread.
        throw process_error(L"main thread is still suspended");
        break;
    }
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

void process::check_is_not_resumed(const char* calling_function) const
{
    if (m_is_resumed)
    {
        std::string message(calling_function);
        message += ": process already resumed";
        throw std::logic_error(message);
    }
}

}
