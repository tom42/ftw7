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

namespace
{

ftw7_conemu::display::display_driver* display_driver;

void handle_exception(const wchar_t* function)
{
    try
    {
        throw;
    }
    catch (const ftw7_core::wruntime_error& e)
    {
        FTW7_LOG_ERROR << function << L": " << e.wwhat();
    }
    catch (const std::exception& e)
    {
        FTW7_LOG_ERROR << function << L": " << e.what();
    }
    catch (...)
    {
        FTW7_LOG_ERROR << function << L": unknown exception";
    }
    FTW7_LOG_ERROR << "Error during API call. Exiting. Now.";
    ExitProcess(ftw7_core::emulation::error_code::error_during_api_call);
}

#define FTW7_HANDLE_API_EXCEPTION() handle_exception(__FUNCTIONW__)

}


////////////////////////////////////////////////////////////////////////////////
// Replacement functions.
// __FUNCTION__and __FUNCTIONW__ return a fully qualified function name.
// We don't want to clutter our logs with this, so we keep the replacement
// functions outside of any namespace, so that the log will contain function
// names only. There are other ways to achieve this, but this is by far
// the simplest way.
////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI ftw7_SetConsoleTitleA(LPCSTR lpConsoleTitle)
{
    try
    {
        // TODO: more convenient mechanism for api call tracing (macro->function. or whatever)
        // TODO: this is actually not THAT simple. An output stream happily prints pointers,
        //       put when you throw a character pointer at it, then it will attempt to print
        //       the string, making our application failbowl nastily.
        // TODO: also: lpConsoleTitle might in theory be null. In this case we really
        //       shouldn't attempt to set it. Rather we should set an error code.
        // TODO: also, what happens if setwindowtext fails? I mean...not sure we should throw because of this.
        //       We should probably return FALSE then, no?
        FTW7_LOG_TRACE << __FUNCTIONW__ << L": " << lpConsoleTitle;
        const auto wide_title = ftw7_core::windows::multibyte_to_wstring(lpConsoleTitle);
        // TODO: concurrency?
        display_driver->set_title(wide_title.c_str());
    }
    catch (...)
    {
        FTW7_HANDLE_API_EXCEPTION();
    }
    return TRUE;
}

BOOL WINAPI ftw7_WriteConsoleOutputA(HANDLE, const CHAR_INFO*, COORD, COORD, PSMALL_RECT)
{
    try
    {
        // TODO: trace API call
        // TODO: actually do something (take care not to choke on null pointers etc!)
        // TODO: concurrency?
        if (!display_driver->handle_messages())
        {
            FTW7_LOG_INFO << L"Exiting (exit requested by display driver)";
            ExitProcess(ftw7_core::emulation::error_code::no_error);
        }
    }
    catch (...)
    {
        FTW7_HANDLE_API_EXCEPTION();
    }
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// Initialization code
////////////////////////////////////////////////////////////////////////////////

namespace ftw7_conemu
{
namespace emulation
{
namespace
{

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
