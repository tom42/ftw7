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
#include <vector>
#include "ftw7_core/windows/string.hpp"
#include "ftw7_core/windows/windows_error.hpp"

namespace ftw7_core
{
namespace windows
{

std::wstring multibyte_to_wstring(const char* s)
{
    // Find length of string
    auto n_wchars = MultiByteToWideChar(CP_ACP, 0, s, -1, nullptr, 0);
    if (n_wchars <= 0)
    {
        auto error = GetLastError();
        throw windows_error(L"MultiByteToWideChar failed", error);
    }

    // Allocate buffer and perform conversion
    std::vector<wchar_t> buf(n_wchars);
    auto result = MultiByteToWideChar(CP_ACP, 0, s, -1, &buf[0], buf.size());
    if (result <= 0)
    {
        auto error = GetLastError();
        throw windows_error(L"MultiByteToWideChar failed", error);
    }

    return std::wstring(&buf[0]);
}

std::string wstring_to_multibyte(const wchar_t* s)
{
    // Find number of bytes required in output buffer
    int n_bytes = WideCharToMultiByte(CP_ACP, 0, s, -1, nullptr, 0, nullptr, nullptr);
    if (n_bytes <= 0)
    {
        auto error = GetLastError();
        throw windows_error(L"WideCharToMultiByte failed", error);
    }

    // Allocate buffer and perform conversion
    std::vector<char> buf(n_bytes);
    int result = WideCharToMultiByte(CP_ACP, 0, s, -1, &buf[0], n_bytes, nullptr, nullptr);
    if (result <= 0)
    {
        auto error = GetLastError();
        throw windows_error(L"WideCharToMultiByte failed", error);
    }

    return std::string(&buf[0]);
}

}
}
