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
#include "ftw7_conemu/emulation/hooks.hpp"
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

// Define the variables that hold the addresses of the true API functions.
// This yields definitions such as: SetConsoleTitleA_ptr_t true_SetConsoleTitleA;
#define FTW7_CONEMU_XHOOKED_FUNCTION(dllname, procname) procname##_ptr_t true_##procname;
#include "ftw7_conemu/emulation/hooked_functions.x"
#undef FTW7_CONEMU_XHOOKED_FUNCTION

display::display_driver* display_driver;

template <typename T, typename L>
auto doit(T, const L& l) -> decltype(l())
{
    // TODO: try/catch
    // TODO: supply lambda and function name first
    // TODO: variadic template, do tracing with that. This is ossom.
    return l();
}

BOOL WINAPI ftw7_SetConsoleTitleA(LPCSTR lpConsoleTitle)
{
    // TODO: trace, catch exceptions and all that
    // I think this might be something halfways useful
    // The doit() function template can now go and have a generic try/catch
    // around the call to the lambda
    return doit(lpConsoleTitle, [=]()
    {
        auto wide_title = ftw7_core::windows::multibyte_to_wstring(lpConsoleTitle);
        display_driver->set_title(wide_title.c_str());
        return TRUE;
    });
}

BOOL WINAPI ftw7_WriteConsoleOutputA(HANDLE, const CHAR_INFO*, COORD, COORD, PSMALL_RECT)
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
    using ftw7_core::mhookpp::set_hook;

    // The DLLs we're intercepting functions from are all used by the emulation DLL itself.
    // Therefore we can just get their module handles using get_module_handle.
    // This funcion throws on error, but that should not happen.
    const auto kernel32 = get_module_handle(L"kernel32");

    // TODO: possibly need to divide up hooking into an early stage where
    //       we hook a small subset of functions only (e.g. WriteConsoleA and WriteConsoleW,
    //       those we need for logging). We can then hook those, which should definitely
    //       be around. We can then go on set up the logging already with the hooks in place
    //       and know that if some logging code uses WriteConsoleOutputA it can use the true version of it)

    // Hook all functions listed in the xheader.
#define FTW7_CONEMU_XHOOKED_FUNCTION(dllname, procname)                                     \
    {                                                                                       \
        FTW7_LOG_DEBUG << L"Hooking function " << #procname << L" (" << #dllname << L')';   \
        set_hook(dllname, #procname, &true_##procname, ftw7_##procname);                    \
    }
#include "ftw7_conemu/emulation/hooked_functions.x"
#undef FTW7_CONEMU_XHOOKED_FUNCTION
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
