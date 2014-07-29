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
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "ftw7_core/windows/format_message.hpp"
#include "ftw7_core/windows/module.hpp"
#include "ftw7_core/windows/windows_error.hpp"

namespace ftw7_core
{
namespace windows
{

std::wstring get_module_filename(HMODULE module)
{
    typedef std::vector<wchar_t> buf_type;
    buf_type filename(MAX_PATH);

    for (;;)
    {
        const auto size = GetModuleFileNameW(module, &filename[0], filename.size());
        if (size == 0)
        {
            const auto errorcode = GetLastError();
            std::wostringstream msg;
            msg << L"could not obtain module file name of module " << module;
            throw windows_error(msg.str(), errorcode);
        }
        if (size != filename.size())
        {
            // GetModuleFileName returns the number of characters written to
            // our buffer excluding the terminating zero. If that number is
            // smaller than our buffer's size we know that we got the entire
            // filename.
            return std::wstring(filename.begin(), filename.begin() + size);
        }

        // Buffer is too small.
        // Double its size, but check for overflow before doing so.
        if (std::numeric_limits<buf_type::size_type>::max() - filename.size()
            < filename.size())
        {
            throw std::overflow_error("get_module_filename: overflow while "
                "attempting to increase buffer capacity");
        }
        filename.resize(filename.size() + filename.size());;
    }
}

HMODULE get_module_handle(const wchar_t* module_name)
{
    HMODULE module = GetModuleHandleW(module_name);
    if (!module)
    {
        const auto error = GetLastError();
        std::wostringstream msg;
        msg << L"could not obtain module handle of ";
        if (module_name)
        {
            msg << L"`" << module_name << L"'";
        }
        else
        {
            msg << L".exe file";
        }
        throw windows_error(msg.str(), error);
    }
    return module;
}

FARPROC get_proc_address(HMODULE module, const char* proc_name)
{
    FARPROC proc = GetProcAddress(module, proc_name);
    if (!proc)
    {
        const auto error = GetLastError();
        std::wostringstream msg;
        msg << L"could not obtain address of `" << proc_name << "' from ";
        msg << L"module " << module;
        throw windows_error(msg.str(), error);
    }
    return proc;
}

HMODULE load_library(const wchar_t* filename)
{
    HMODULE module = LoadLibraryW(filename);
    if (!module)
    {
        const auto error = GetLastError();
        std::wstring msg = L"could not load module `";
        msg += filename;
        msg += L"'";
        throw windows_error(msg, error);
    }
    return module;
}

}
}
