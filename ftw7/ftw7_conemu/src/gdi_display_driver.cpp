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

unique_hwnd create_window(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
{
    // TODO: check what we really need for windowed mode...
    const DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
    const DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    RECT r;
    r.left = r.top = 0;
    r.right = settings.width;
    r.bottom = settings.height;

    AdjustWindowRectEx(&r, style, FALSE, exStyle); // TODO: failbowl lould and noisily on error!
    auto width = r.right - r.left;
    auto height = r.bottom - r.top;

    // TODO: must be prepared to get a smaller window than requested if windows thinks it can't fit it onto the desktop!
    // TODO: testcode, revisit all args
    unique_hwnd hwnd(CreateWindowEx(
        exStyle,
        FTW7_GDI_DISPLAY_DRIVER_NAME,
        FTW7_GDI_DISPLAY_DRIVER_NAME,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
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

}

gdi_display_driver::gdi_display_driver(HINSTANCE emulation_dll_module_handle, const ftw7_core::emulation::settings& settings)
    : m_wc(create_wndclassexw(emulation_dll_module_handle)),
    m_hwnd(create_window(emulation_dll_module_handle, settings)),
    m_renderbuffer(RENDER_BUFFER_WIDTH * RENDER_BUFFER_HEIGHT)
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

    // TODO: review, unhardcode stuff, find out how/when to allocate/deallocate the DC
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
    auto dc = GetDC(m_hwnd.get());
    RECT rect;
    GetClientRect(m_hwnd.get(), &rect); // TODO: handle errors, really

    StretchDIBits(dc, 0, 0, rect.right, rect.bottom, 0, 0, RENDER_BUFFER_WIDTH, RENDER_BUFFER_HEIGHT, &m_renderbuffer[0], &bmi, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(m_hwnd.get(), dc);
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
