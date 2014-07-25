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
#include "ftw7_core/string.hpp"
#include "ftw7_core/windows_error.hpp"

namespace ftw7_core
{

// TODO: optimize:
// * This code finds the input string's length twice, which is stupid.
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

    // TODO: this is the simplest but slowest way to exclude the terminating zero from vector.
    return std::wstring(&buf[0]);
}

}
