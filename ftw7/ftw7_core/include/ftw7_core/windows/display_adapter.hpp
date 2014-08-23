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
#include "ftw7_core/wexcept.hpp"

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

inline std::vector<DISPLAY_DEVICEW> get_active_physical_display_adapters()
{
    return get_display_adapters(
        [](const DISPLAY_DEVICEW& d){ return !(d.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) && (d.StateFlags & DISPLAY_DEVICE_ACTIVE); });
}

inline DISPLAY_DEVICEW get_primary_display_adapter()
{
    auto adapters = get_display_adapters([](const DISPLAY_DEVICEW& d){ return d.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE; });
    if (adapters.size() != 1)
    {
        throw ftw7_core::wruntime_error(L"could not get primary display adapter");
    }
    return adapters[0];
}

}
}

#endif
