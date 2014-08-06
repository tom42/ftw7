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
#include "ftw7_core/wexcept.hpp"
#include "ftw7_conemu/emulation/emulation.hpp"

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

    // TODO: initialize display/emulation
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

        ftw7_core::log::initialize(settings->log_level);

        FTW7_LOG_INFO << L"Starting up ftw7 console emulation";
        FTW7_LOG_DEBUG << L"Emulation DLL's module handle: " << emulation_dll_module_handle;
        FTW7_LOG_DEBUG << L"Main thread ID: " << GetCurrentThreadId();

        ftw7_conemu::emulation::initialize(emulation_dll_module_handle, *settings);

        return no_error;
    }
    catch (const ftw7_core::wruntime_error& e)
    {
        FTW7_LOG_ERROR << __FUNCTIONW__ << L": " << e.wwhat();
    }
    catch (const std::exception& e)
    {
        FTW7_LOG_ERROR << __FUNCTIONW__ << L": " << e.what();
    }
    catch (...)
    {
        FTW7_LOG_ERROR << __FUNCTIONW__ << L": unknown exception";
    }
    return error_during_initialization;
}
