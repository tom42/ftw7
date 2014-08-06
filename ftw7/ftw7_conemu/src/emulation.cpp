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
#include "ftw7_conemu/display/gdi_display_driver.hpp"
#include "ftw7_conemu/emulation/emulation.hpp"
#include "ftw7_core/log/log.hpp"
#include "ftw7_core/mhookpp/mhookpp.hpp"
#include "ftw7_core/windows/module.hpp"
#include "ftw7_core/windows/string.hpp"

namespace ftw7_conemu
{
namespace emulation
{
namespace
{

// TODO: decide where to stash all of this and how to generate it
typedef decltype(SetConsoleTitleA)* SetConsoleTitleA_ptr_t;
typedef decltype(WriteConsoleOutputA)* WriteConsoleOutputA_ptr_t;
SetConsoleTitleA_ptr_t TrueSetConsoleTitleA;
WriteConsoleOutputA_ptr_t TrueWriteConsoleOutputA;

display::display_driver* display_driver;

BOOL WINAPI MySetConsoleTitleA(LPCSTR lpConsoleTitle)
{
    auto wide_title = ftw7_core::windows::multibyte_to_wstring(lpConsoleTitle);
    display_driver->set_title(wide_title.c_str());
    return TRUE;
}

BOOL WINAPI MyWriteConsoleOutputA(HANDLE, const CHAR_INFO*, COORD, COORD, PSMALL_RECT)
{
    // TODO: trace this
    // TODO: catch all exceptions (a dispatcher would be nice for this)
    // TODO: throw stuff at the driver
    // TODO: consider putting these functions out of any namespace, just to get
    //       the namespace out of them when logging...


    // TODO: this can get very nasty:
    // If we're called from a thread other than the main thread,
    // accessing the driver window might/will/whatever fail (don't know how windows behaves in that case, really)
    // This can happen anywhere the display_driver is accessed, so we might want to guard against this somehow.
    // Better to detect this and fail loud and noisily...
    if (!display_driver->handle_messages())
    {
        FTW7_LOG_INFO << L"Exiting (exit requested by display driver)";
        ExitProcess(0);
    }

    return TRUE;
}

void install_hooks()
{
    using ftw7_core::windows::get_module_handle;
    using ftw7_core::windows::get_proc_address;
    using ftw7_core::mhookpp::set_hook;

    // The DLLs we're intercepting functions from are all functions the emulation DLL is using itself.
    // Therefore we can just get their module handles using get_module_handle. This funcion throws on
    // error, but that should not happen.
    const auto kernel32 = get_module_handle(L"kernel32");

    // TODO: find out how to handle errors here.
    // In theory, get_proc_address might fail because we're attempting to hook a function
    // that simply does not exist on a particular version of windows...
    // TODO: possibly need to dive up hooking into an early stage where
    //       we hook a small subset of functions only (e.g. WriteConsoleA and WriteConsoleW,
    //       those we need for logging). We can then hook those, which should definitely
    //       be around. We can then go on set up the logging already with the hooks in place
    //       and know that if some logging code uses WriteConsoleOutputA it can use the true version of it)
    TrueWriteConsoleOutputA = reinterpret_cast<WriteConsoleOutputA_ptr_t>(get_proc_address(kernel32, "WriteConsoleOutputA"));
    set_hook(&TrueWriteConsoleOutputA, MyWriteConsoleOutputA);

    TrueSetConsoleTitleA = reinterpret_cast<SetConsoleTitleA_ptr_t>(get_proc_address(kernel32, "SetConsoleTitleA"));
    set_hook(&TrueSetConsoleTitleA, MySetConsoleTitleA);
}

}

void initialize(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
{
    install_hooks();
    // TODO: need to put away the display driver somewhere.
    // TODO: when do we clean up, btw?
    display_driver = new display::gdi_display_driver(emulation_dll_module_handle, settings);
}

}
}
