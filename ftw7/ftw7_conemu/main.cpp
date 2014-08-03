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
#include <atomic>
#include "ftw7_core/emulation/emulation.hpp"
#include "ftw7_core/log/log.hpp"

namespace
{
HINSTANCE emulation_dll_module_handle;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        emulation_dll_module_handle = hinstDLL;
        return TRUE;
    default:
        return TRUE;
    }
}

// This function needs to be called by the injected code to bring up the console
// emulation. Note that it mustn't propagate any C++ exceptions up to the caller;
// The caller is our injected code and can't handle C++ exceptions.
//
// For convenience this function is using the __stdcall calling convention.
//
// __stdcall functions get a decorated name by default (e.g. _get_version@0).
// The most convenient way to get rid of name decoration for __stdcall functions
// is through a .def file.
//
// Because we're using a .def file anyway we can also omit the dllexport bit
// here.
extern "C" int __stdcall ftw7_conemu_initialize(const ftw7_core::emulation::settings* settings)
{
    using namespace ftw7_core::emulation;

    // TODO: initialize logging
    // TODO: initialize display/emulation
    // TODO: better error handling: catch known types of exceptions, log them, then exit.
    try
    {
        static std::atomic<bool> is_initialized(false);
        if (is_initialized.exchange(true))
        {
            return init_already_called;
        }

        const auto settings_check_result = settings::check(settings);
        if (settings_check_result != no_error)
        {
            return settings_check_result;
        }

        // TODO: initialize logging here (check for double initialization, btw? (perhaps, but don't fail...)

        FTW7_LOG_INFO << "Starting up ftw7 console emulation";
        FTW7_LOG_DEBUG << "Emulation DLL's module handle: " << emulation_dll_module_handle;
        FTW7_LOG_DEBUG << "Main thread ID: " << GetCurrentThreadId();
        return no_error;
    }
    catch (...)
    {
        // TODO: log this somehow, too.
        return error_during_initialization;
    }
}
