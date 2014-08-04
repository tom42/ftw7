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
#ifndef FTW7_CORE_POINTER_HPP_INCLUDED
#define FTW7_CORE_POINTER_HPP_INCLUDED

#include <type_traits>

namespace ftw7_core
{

template <typename int_type>
int_type pointer_to_int(void* ptr)
{
    static_assert(std::is_integral<int_type>::value,
        "target type is non-integral");

    static_assert(sizeof(int_type) == sizeof(ptr),
        "size of target type differs from size of pointer");

    return reinterpret_cast<int_type>(ptr);
}

}

#endif
