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
#include <atomic>
#include <iostream>
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

// Allow tracing of selected data types.  We keep the operator << overloads
// inside this anonymous namespace to ensure they don't disturb anybody else.
std::wostream& operator << (std::wostream& os, COORD coord)
{
    os << L'(' << coord.X << L',' << coord.Y << ')';
    return os;
}
std::wostream& operator << (std::wostream& os, const SMALL_RECT& small_rect)
{
    os << L'(' << small_rect.Left
        << L',' << small_rect.Top
        << L',' << small_rect.Right
        << L',' << small_rect.Bottom
        << L')';
    return os;
}
std::wostream& operator << (std::wostream& os, const SMALL_RECT* p_small_rect)
{
    // For the tracing messages we'd like to print as much data as possible
    // before crashing, so we use ReadProcessMemory here in case the caller
    // gave us a bad pointer.
    SMALL_RECT sr;
    if (ReadProcessMemory(GetCurrentProcess(), p_small_rect, &sr, sizeof(sr), nullptr))
    {
        os << sr;
    }
    else
    {
        os << L"(?)";
    }
    os << static_cast<const void*>(p_small_rect);
    return os;
}

}


////////////////////////////////////////////////////////////////////////////////
// Replacement functions.
// __FUNCTION__and __FUNCTIONW__ return a fully qualified function name.
// We don't want to clutter our logs with this, so we keep the replacement
// functions outside of any namespace, so that the log will contain function
// names only. There are other ways to achieve this, but this is by far
// the simplest way.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// kernel32
////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI ftw7_AllocConsole()
{
    try
    {
        // Magrathea is a Windows application and has its fullscreen emulation
        // built in. When run with command line arguments "outp 0", the demo
        // creates a console and uses that for output.
        // While harmless, this would result in yet another annoying console
        // window lingering around, so we suppress this.
        FTW7_TRACE_API_CALL();
        FTW7_LOG_DEBUG << __FUNCTIONW__ << L": faked. This is a no-op.";
    }
    catch (...)
    {
        FTW7_HANDLE_API_EXCEPTION();
    }
    return TRUE;
}

BOOL WINAPI ftw7_SetConsoleActiveScreenBuffer(HANDLE hConsoleOutput)
{
    try
    {
        // Some demos initialize the console by creating a new screen buffer and
        // setting it as the active one. This is causes our console log output
        // to disappear, so we suppress this function altogether.
        FTW7_TRACE_API_CALL(hConsoleOutput);
        FTW7_LOG_DEBUG << __FUNCTIONW__ << L": faked. This is a no-op.";
    }
    catch (...)
    {
        FTW7_HANDLE_API_EXCEPTION();
    }
    return TRUE;
}

BOOL WINAPI ftw7_SetConsoleTitleA(LPCSTR lpConsoleTitle)
{
    try
    {
        // TODO: also, what happens if setwindowtext fails? I mean...not sure we should throw because of this.
        //       We should probably return FALSE then, no?
        FTW7_TRACE_API_CALL(lpConsoleTitle);
        if (!lpConsoleTitle)
        {
            // Invalid argument. Let Windows deal with it.
            return ftw7_conemu::emulation::true_SetConsoleTitleA(lpConsoleTitle);
        }

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

BOOL WINAPI ftw7_SetConsoleTitleW(LPCWSTR lpConsoleTitle)
{
    try
    {
        FTW7_TRACE_API_CALL(lpConsoleTitle);
        if (!lpConsoleTitle)
        {
            // Invalid argument. Let Windows deal with it.
            return ftw7_conemu::emulation::true_SetConsoleTitleW(lpConsoleTitle);
        }
        // TODO: concurrency?
        display_driver->set_title(lpConsoleTitle);
    }
    catch (...)
    {
        FTW7_HANDLE_API_EXCEPTION();
    }
    return TRUE;
}

BOOL WINAPI ftw7_WriteConsoleOutputA(
    HANDLE hConsoleOutput, const CHAR_INFO* lpBuffer, COORD dwBufferSize, COORD dwBufferCoord, PSMALL_RECT lpWriteRegion)
{
    try
    {
        FTW7_TRACE_API_CALL(hConsoleOutput, lpBuffer, dwBufferSize, dwBufferCoord, lpWriteRegion);
        if (!hConsoleOutput || !lpBuffer || !lpWriteRegion)
        {
            // Invalid argument(s)
            return ftw7_conemu::emulation::true_WriteConsoleOutputA(hConsoleOutput, lpBuffer, dwBufferSize, dwBufferCoord, lpWriteRegion);
        }

        if (!((dwBufferSize.X == 80) &&
            (dwBufferSize.Y == 50) &&
            (dwBufferCoord.X == 0) &&
            (dwBufferCoord.Y == 0) &&
            (lpWriteRegion->Left == 0) &&
            (lpWriteRegion->Top == 0) &&
            (lpWriteRegion->Right == 80) &&
            (lpWriteRegion->Bottom == 50)))
        {
            std::wostringstream msg;
            msg << L"unsupported blit size. dwBufferSize=" << dwBufferSize
                << " dwBufferCoord=" << dwBufferCoord
                << " lpWriteRegion=" << lpWriteRegion;
            throw ftw7_core::wruntime_error(msg.str());
        }


        // TODO: actually do something
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
// user32
////////////////////////////////////////////////////////////////////////////////

int WINAPI ftw7_ShowCursor(BOOL bShow)
{
    static std::atomic<int> display_counter(0);
    int result = 0;
    try
    {
        // It's particularly annoying in windowed mode when demos disable the
        // mouse cursor, so we control cursor visibility inside the display
        // drivers and simply fake ShowCursor including its return value.
        FTW7_TRACE_API_CALL(bShow);
        result = bShow ? ++display_counter : --display_counter;
        FTW7_LOG_DEBUG << __FUNCTIONW__ << L": faked. New display counter: " << result;
    }
    catch (...)
    {
        FTW7_HANDLE_API_EXCEPTION();
    }
    return result;
}

BOOL WINAPI ftw7_ShowWindow(HWND hWnd, int nCmdShow)
{
    try
    {
        FTW7_TRACE_API_CALL(hWnd, nCmdShow);
        // TODO: check if they want to fiddle with our own window. If so, forbid id. If not, don't (currently we're always a no-op)
        // TODO: document what we're doing this for (trauma demos)        
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
    const auto user32 = get_module_handle(L"user32");

    // TODO: possibly need to divide up hooking into an early stage where
    //       we hook a small subset of functions only (e.g. WriteConsoleA and WriteConsoleW,
    //       those we need for logging). We can then hook those, which should definitely
    //       be around. We can then go on set up the logging already with the hooks in place
    //       and know that if some logging code uses WriteConsoleOutputA it can use the true version of it)

    // Hook all functions listed in the xheader.
#define FTW7_CONEMU_XHOOKED_FUNCTION(dllname, procname)                     \
    {                                                                       \
        FTW7_LOG_DEBUG << L"Hooking function " << #procname                 \
            << L" (" << #dllname << L", " << dllname << L')';               \
        set_hook(dllname, #procname, &true_##procname, ftw7_##procname);    \
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
