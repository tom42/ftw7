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
#include <memory>
#include <sstream>
#include "ftw7_core/format_message.hpp"

namespace ftw7_core
{

namespace
{

class local_free_deleter
{
public:
    template <typename T>
    void operator ()(T* p)
    {
        LocalFree(p);
    }
};

// Overload to call FormatMessageA
DWORD format_message_win32(DWORD dwFlags, LPCVOID lpSource,
    DWORD dwMessageId, DWORD dwLanguageId, char* lpBuffer, DWORD nSize,
    va_list *Arguments)
{
    return FormatMessageA(dwFlags, lpSource, dwMessageId, dwLanguageId,
        lpBuffer, nSize, Arguments);
}

// Overload to call FormatMessageW
DWORD format_message_win32(DWORD dwFlags, LPCVOID lpSource,
    DWORD dwMessageId, DWORD dwLanguageId, wchar_t* lpBuffer, DWORD nSize,
    va_list *Arguments)
{
    return FormatMessageW(dwFlags, lpSource, dwMessageId, dwLanguageId,
        lpBuffer, nSize, Arguments);
}

template <typename char_type>
std::basic_string<char_type> format_message_allocate_buffer(DWORD dwFlags,
    LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, va_list *Arguments)
{
    // Call FormatMessage and immediately stash the memory it allocated for
    // us in a unique_ptr before doing anything else.
    char_type* buf = 0;
    const DWORD result = format_message_win32(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | dwFlags,
        lpSource, dwMessageId, dwLanguageId, reinterpret_cast<char_type*>(&buf),
        0, Arguments);
    std::unique_ptr<char_type, local_free_deleter> p(buf);

    if (!result)
    {
        const DWORD format_message_error = GetLastError();
        std::basic_ostringstream<char_type> message;
        message << "unknown error " << dwMessageId <<
            ": FormatMessage failed with error code " << format_message_error;
        return message.str();
    }

    // If FormatMessage succeeds, the return value specifies the number of
    // TCHARs excluding the terminating zero stored in the output buffer.
    // Construct a string from that and return it.
    std::basic_string<char_type> message(buf, buf + result);
    return message;
}

template <typename char_type>
std::basic_string<char_type> format_message_from_system(DWORD errorcode)
{
    return format_message_allocate_buffer<char_type>(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorcode, 0, nullptr);
}

template <typename char_type>
std::basic_string<char_type>
format_message_from_system(const char_type* message_prefix, DWORD errorcode)
{
    std::basic_string<char_type> message(message_prefix);
    message += ':';
    message += ' ';
    message += format_message_from_system<char_type>(errorcode);
    return message;
}

}

std::string format_message_from_system(DWORD errorcode)
{
    return format_message_from_system<char>(errorcode);
}

std::wstring wformat_message_from_system(DWORD errorcode)
{
    return format_message_from_system<wchar_t>(errorcode);
}

std::wstring wformat_message_from_system(const wchar_t* message_prefix,
    DWORD errorcode)
{
    return format_message_from_system<wchar_t>(message_prefix, errorcode);
}

std::wstring wformat_message_from_system(const std::wstring& message_prefix,
    DWORD errorcode)
{
    return format_message_from_system<wchar_t>(message_prefix.c_str(),
        errorcode);
}

}
