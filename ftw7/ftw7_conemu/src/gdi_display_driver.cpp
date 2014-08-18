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
#include "ftw7_version.h"
#include "ftw7_conemu/display/gdi_display_driver.hpp"
#include "ftw7_conemu/display/vga8x8.hpp"
#include "ftw7_conemu/emulation/hooked_functions.h"
#include "ftw7_core/windows/windows_error.hpp"
#include "resource.h"

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define FTW7_GDI_DISPLAY_DRIVER_NAME WIDEN(PACKAGE_STRING) L" GDI display driver"
#define CONRGB(r, g, b) ((((r) & 255) << 16) | (((g) & 255) << 8) | (((b) & 255)))

namespace ftw7_conemu
{
namespace display
{
namespace
{

using ftw7_core::windows::unique_hwnd;

const int RENDER_BUFFER_WIDTH = 640;
const int RENDER_BUFFER_HEIGHT = 400;
const int CHAR_WIDTH = 8;
const int CHAR_HEIGHT = 8;
const uint32_t palette[] =
{
    CONRGB(0x00, 0x00, 0x00),
    CONRGB(0x00, 0x00, 0xaa),
    CONRGB(0x00, 0xaa, 0x00),
    CONRGB(0x00, 0xaa, 0xaa),
    CONRGB(0xaa, 0x00, 0x00),
    CONRGB(0xaa, 0x00, 0xaa),
    CONRGB(0xaa, 0x55, 0x00),
    CONRGB(0xaa, 0xaa, 0xaa),
    CONRGB(0x55, 0x55, 0x55),
    CONRGB(0x55, 0x55, 0xff),
    CONRGB(0x55, 0xff, 0x55),
    CONRGB(0x55, 0xff, 0xff),
    CONRGB(0xff, 0x55, 0x55),
    CONRGB(0xff, 0x55, 0xff),
    CONRGB(0xff, 0xff, 0x55),
    CONRGB(0xff, 0xff, 0xff)
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}

WNDCLASSEXW create_wndclassexw(HINSTANCE emulation_dll_module_handle)
{
    WNDCLASSEXW wc;
    memset(&wc, 0, sizeof(wc));

    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = emulation_dll_module_handle;
    wc.hIcon = LoadIconW(emulation_dll_module_handle, MAKEINTRESOURCEW(IDI_FTW7));
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = FTW7_GDI_DISPLAY_DRIVER_NAME;
    wc.hIconSm = nullptr;

    return wc;
}

DEVMODEW get_current_display_settings()
{
    DEVMODEW dm;
    memset(&dm, 0, sizeof(dm));
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &dm))
    {
        // EnumDisplaySettingsW doesn't use SetLastError().
        // Therefore throw a wruntime_error, not a windows_error.
        throw ftw7_core::wruntime_error(L"could not query current display settings");
    }
    return dm;
}

unique_hwnd create_fullscreen_window(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
{
    const DWORD ex_style = WS_EX_TOPMOST;
    const DWORD style = WS_POPUP | WS_VISIBLE;

    auto current_settings = get_current_display_settings();
    FTW7_LOG_DEBUG << L"Current display resolution: " << current_settings.dmPelsWidth << L'x' << current_settings.dmPelsHeight;
    if ((current_settings.dmPelsWidth != static_cast<DWORD>(settings.screen_width)) ||
        (current_settings.dmPelsHeight != static_cast<DWORD>(settings.screen_height)))
    {
        FTW7_LOG_DEBUG << "Changing display resolution to " << settings.screen_width << L'x' << settings.screen_height;

        DEVMODEW dm;
        memset(&dm, 0, sizeof(dm));
        dm.dmSize = sizeof(dm);
        dm.dmPelsWidth = settings.screen_width;
        dm.dmPelsHeight = settings.screen_height;
        dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
        if (DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettingsW(&dm, CDS_FULLSCREEN))
        {
            // ChangeDisplaySettingsW doesn't use SetLastError().
            // Therefore throw a wruntime_error, not a windows_error.
            throw ftw7_core::wruntime_error(L"Could not set display mode");
        }
    }

    // TODO: also need to adjust blitting code!

    unique_hwnd hwnd(CreateWindowEx(
        ex_style,
        FTW7_GDI_DISPLAY_DRIVER_NAME,
        FTW7_GDI_DISPLAY_DRIVER_NAME,
        style,
        0,
        0,
        settings.screen_width,
        settings.screen_height,
        HWND_DESKTOP,
        nullptr,
        emulation_dll_module_handle,
        nullptr));
    if (!hwnd)
    {
        const auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"CreateWindowEx failed", error);
    }
    true_ShowCursor(FALSE);
    return hwnd;
}

unique_hwnd create_windowed_window(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
{
    const DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;
    const DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    RECT r;
    r.left = r.top = 0;
    r.right = settings.window_width;
    r.bottom = settings.window_height;

    if (!AdjustWindowRectEx(&r, style, FALSE, ex_style))
    {
        const auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"AdjustWindowRectEx failed", error);
    }

    // Note: we may get a window smaller than requested if Windows thinks it
    // can't fit the window onto the desktop.
    unique_hwnd hwnd(CreateWindowEx(
        ex_style,
        FTW7_GDI_DISPLAY_DRIVER_NAME,
        FTW7_GDI_DISPLAY_DRIVER_NAME,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        r.right - r.left,
        r.bottom - r.top,
        HWND_DESKTOP,
        nullptr,
        emulation_dll_module_handle,
        nullptr));
    if (!hwnd)
    {
        const auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"CreateWindowEx failed", error);
    }
    return hwnd;
}

unique_hwnd create_window(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
{
    if (settings.fullscreen)
    {
        return create_fullscreen_window(emulation_dll_module_handle, settings);
    }
    return create_windowed_window(emulation_dll_module_handle, settings);
}

}

gdi_display_driver::gdi_display_driver(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
    : m_wc(create_wndclassexw(emulation_dll_module_handle)),
    m_hwnd(create_window(emulation_dll_module_handle, settings)),
    m_hdc(m_hwnd.get()),
    m_renderbuffer(RENDER_BUFFER_WIDTH * RENDER_BUFFER_HEIGHT),
    m_fullscreen(settings.fullscreen)
{
}

gdi_display_driver::~gdi_display_driver()
{
}

bool gdi_display_driver::handle_messages()
{
    MSG msg;
    bool keep_running = true;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            keep_running = false;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return keep_running;
}

void gdi_display_driver::render(const CHAR_INFO* buffer)
{
    for (int row = 0; row < RENDER_BUFFER_HEIGHT / CHAR_HEIGHT; ++row)
    {
        for (int col = 0; col < RENDER_BUFFER_WIDTH / CHAR_WIDTH; ++col)
        {
            int x0 = col * CHAR_WIDTH;
            int y0 = row * CHAR_HEIGHT;
            uint32_t* dst = &m_renderbuffer[y0 * RENDER_BUFFER_WIDTH + x0];

            // Get foreground and background color.
            // Don't worry about the COMMON_LVB flags, this is CJK specific
            // and apparently not even supported on non-CJK Windows systems.
            const uint32_t bgcolor = palette[(buffer->Attributes >> 4) & 15];
            const uint32_t fgcolor = palette[buffer->Attributes & 15];

            const size_t c = static_cast<unsigned char>(buffer->Char.AsciiChar);
            const unsigned char* chardata = &vga8x8[c * CHAR_HEIGHT];

            // Render character
            for (int y = 0; y < CHAR_HEIGHT; ++y)
            {
                unsigned char byte = *chardata++;
                for (int x = 0; x < CHAR_WIDTH; ++x)
                {
                    *dst++ = (byte & 128) ? fgcolor : bgcolor;
                    byte <<= 1;
                }
                dst += RENDER_BUFFER_WIDTH - CHAR_WIDTH;
            }
            ++buffer;
        }
    }

    static const BITMAPINFO bmi =
    {
        {
            sizeof(bmi),
            RENDER_BUFFER_WIDTH,
            -RENDER_BUFFER_HEIGHT,
            1, 32, BI_RGB, 0, 0, 0, 0, 0
        },
        { 0, 0, 0, 0 }
    };

    RECT rect;
    if (!GetClientRect(m_hwnd.get(), &rect))
    {
        const auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"GetClientRect failed", error);
    }

    // Windowed mode: scale to fit
    int xdest = 0;
    int ydest = 0;
    int dest_width = rect.right;
    int dest_height = rect.bottom;
    if (m_fullscreen)
    {
        // Fullscreen mode: center on screen
        xdest = (rect.right - RENDER_BUFFER_WIDTH) / 2;
        ydest = (rect.bottom - RENDER_BUFFER_HEIGHT) / 2;
        dest_width = RENDER_BUFFER_WIDTH;
        dest_height = RENDER_BUFFER_HEIGHT;
    }

    if (!StretchDIBits(m_hdc.get(), xdest, ydest, dest_width, dest_height, 0, 0, RENDER_BUFFER_WIDTH, RENDER_BUFFER_HEIGHT, &m_renderbuffer[0], &bmi, DIB_RGB_COLORS, SRCCOPY))
    {
        const auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"StretchDIBits failed", error);
    }
}

void gdi_display_driver::set_title(const wchar_t* title)
{
    if (!SetWindowTextW(m_hwnd.get(), title))
    {
        const auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"SetWindowText failed", error);
    }
}

}
}
