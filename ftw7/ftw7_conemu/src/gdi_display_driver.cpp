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
#include "resource.h"

namespace ftw7_conemu
{
namespace display
{
namespace
{

using ftw7_core::windows::unique_hwnd;

const wchar_t wndclass_name[] = L"ftw7 GDI display driver window";

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
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // TODO: no brush at all? We paint shit ourselves anyway?
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = wndclass_name;
    wc.hIconSm = nullptr;

    return wc;
}

unique_hwnd create_window()
{
    // TODO: testcode, revisit all args
    unique_hwnd hwnd(CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        wndclass_name,
        L"foo",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        640, 400,
        HWND_DESKTOP,
        nullptr,
        nullptr,
        nullptr));
    if (!hwnd)
    {
        const auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"CreateWindowEx failed", error);
    }
    return hwnd;
}

}

gdi_display_driver::gdi_display_driver(HINSTANCE emulation_dll_module_handle)
    : m_wc(create_wndclassexw(emulation_dll_module_handle)),
    m_hwnd(create_window())
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

}
}
