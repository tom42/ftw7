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
#include "ftw7_core/wexcept.hpp"
#include "ftw7_core/windows/bitness.hpp"
#include "ftw7_core/windows/format_message.hpp"
#include "ftw7_core/windows/module.hpp"

namespace ftw7_core
{
namespace windows
{

bool is_wow64_process()
{
    return is_wow64_process(GetCurrentProcess());
}

bool is_wow64_process(HANDLE process)
{
    // TODO: use decltype and get a correct function pointer automatically?
    typedef BOOL (WINAPI *IsWow64ProcessPtr)(HANDLE, PBOOL);
    HMODULE kernel32 = get_module_handle(L"kernel32.dll");

    IsWow64ProcessPtr isWow64Process = 
        reinterpret_cast<IsWow64ProcessPtr>(GetProcAddress(kernel32,
            "IsWow64Process"));
    if (!isWow64Process)
    {
        // IsWow64Process is not available, so Wow64 can't be available either.
        return false;
    }

    BOOL iswow64 = FALSE;
    if (!isWow64Process(process, &iswow64))
    {
        const DWORD error = GetLastError();
        // TODO: why not windows_error?
        throw wruntime_error(L"IsWow64Process failed: " +
            wformat_message_from_system(error));
    }
    return iswow64 ? true : false;
}

bool is_64bit_os()
{
    if (is_64bit_process())
    {
        return true;
    }

    // If we're not a 64 bit process and we're a Wow64 process, then we're
    // running on a 64 bit OS.
    return is_wow64_process();
}

bool is_64bit_process()
{
#if defined(_WIN64)
    return true;
#else
    return false;
#endif
}

bool is_64bit_process(HANDLE process)
{
    if (!is_64bit_os())
    {
        return false;
    }
    return !is_wow64_process(process);
}

}
}
