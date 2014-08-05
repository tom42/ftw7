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

namespace ftw7_conemu
{
namespace display
{
namespace
{

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // TODO: real implementation.
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


WNDCLASSEXW create_wndclassexw()
{
    WNDCLASSEXW wc;
    memset(&wc, 0, sizeof(wc));

    // TODO: REALLY initialize structure (see MSDN...)
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = nullptr; // TODO: probably required?
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // TODO: no brush at all? We paint shit ourselves anyway?
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"ftw7 gdi display driver";
    wc.hIconSm = nullptr;

    return wc;
}

}

gdi_display_driver::gdi_display_driver()
    : m_wc(create_wndclassexw())
{
    // TODO: testcode, revisit all args. Also, RAII.
    auto hwnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        m_wc.classname().c_str(),
        L"foo",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        640, 400,
        HWND_DESKTOP,
        nullptr, nullptr, nullptr);
    if (!hwnd)
    {
        auto error = GetLastError();
        throw ftw7_core::windows::windows_error(L"CreateWindowEx failed", error);
    }
}

gdi_display_driver::~gdi_display_driver()
{
}

}
}
