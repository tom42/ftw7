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

WNDCLASSEXW create_wndclassexw()
{
    WNDCLASSEXW wc;
    memset(&wc, 0, sizeof(wc));

    // TODO: REALLY initialize structure (see MSDN...)
    wc.cbSize = sizeof(wc);
    wc.lpszClassName = L"ftw7 gdi display driver";

    return wc;
}

}

gdi_display_driver::gdi_display_driver()
    : m_wc(create_wndclassexw())
{
}

gdi_display_driver::~gdi_display_driver()
{
}

}
}
