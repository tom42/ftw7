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
#ifndef FTW7_CORE_WINDOWS_DISPLAY_ADAPTER_HPP_INCLUDED
#define FTW7_CORE_WINDOWS_DISPLAY_ADAPTER_HPP_INCLUDED

#include <Windows.h>
#include <cstring>
#include <vector>

namespace ftw7_core
{
namespace windows
{

template <typename UnaryPredicate>
std::vector<DISPLAY_DEVICEW> get_display_adapters(UnaryPredicate predicate)
{
    std::vector<DISPLAY_DEVICEW> adapters;
    DWORD adapter_index = 0;
    for (;;)
    {
        DISPLAY_DEVICEW adapter;
        memset(&adapter, 0, sizeof(adapter));
        adapter.cb = sizeof(adapter);
        if (!EnumDisplayDevicesW(nullptr, adapter_index, &adapter, 0))
        {
            break;
        }
        ++adapter_index;
        if (predicate(adapter))
        {
            adapters.push_back(adapter);
        }
    }
    return adapters;
}

}
}

#endif
