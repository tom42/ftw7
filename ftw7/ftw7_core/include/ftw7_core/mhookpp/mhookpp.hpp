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
#ifndef FTW7_CORE_MHOOKPP_MHOOKPP_HPP_INCLUDED
#define FTW7_CORE_MHOOKPP_MHOOKPP_HPP_INCLUDED

#include <Windows.h>
#include <type_traits>
#include "ftw7_core/pointer.hpp"
#include "mhook-lib/mhook.h"

namespace ftw7_core
{
namespace mhookpp
{

// Type-safe wrapper around Mhook_SetHook.
template <typename TPSystemFunction, typename TPHookFunction>
bool set_hook(TPSystemFunction* pp_system_function, TPHookFunction p_hookfunction)
{
    static_assert(ftw7_core::is_function_pointer<typename std::remove_pointer<decltype(pp_system_function)>::type>::value,
        "Argument pp_system_function must be a pointer to a function pointer");
    static_assert(ftw7_core::is_function_pointer<decltype(p_hookfunction)>::value,
        "Argument p_hookfunction must be a function pointer");
    static_assert(std::is_same<TPSystemFunction, TPHookFunction>::value,
        "Function pointer types don't match");
    return Mhook_SetHook(reinterpret_cast<PVOID*>(pp_system_function), p_hookfunction) ? true : false;
}

}
}

#endif
