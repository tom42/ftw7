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
#ifndef FTW7_CORE_WINDOWS_UNIQUE_HDC_HPP_INCLUDED
#define FTW7_CORE_WINDOWS_UNIQUE_HDC_HPP_INCLUDED

#include <Windows.h>
#include "ftw7_core/windows/windows_error.hpp"

namespace ftw7_core
{
namespace windows
{

class unique_hdc
{
public:
    unique_hdc(HWND hwnd, HDC hdc) : m_hwnd(hwnd), m_hdc(hdc) {}

    unique_hdc(HWND hwnd) : m_hwnd(hwnd), m_hdc(get_dc(hwnd)) {}

    ~unique_hdc()
    {
        if (m_hdc)
        {
            ReleaseDC(m_hwnd, m_hdc);
        }
    }

    HDC get() const
    {
        return m_hdc;
    }

private:
    unique_hdc(const unique_hdc&) = delete;
    unique_hdc& operator = (const unique_hdc&) = delete;

    static HDC get_dc(HWND hwnd)
    {
        auto hdc = GetDC(hwnd);
        if (!hdc)
        {
            const auto error = GetLastError();
            throw ftw7_core::windows::windows_error(L"GetDC failed", error);
        }
        return hdc;
    }

    const HWND m_hwnd;
    const HDC m_hdc;
};

}
}

#endif
