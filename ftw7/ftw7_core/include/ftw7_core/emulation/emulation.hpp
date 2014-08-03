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
#ifndef FTW7_CORE_EMULATION_EMULATION_HPP_INCLUDED
#define FTW7_CORE_EMULATION_EMULATION_HPP_INCLUDED

#include <type_traits>

namespace ftw7_core
{
namespace emulation
{

enum error_codes
{
    NO_ERROR = 0,
    COULD_NOT_LOAD_EMULATION_DLL = 0x0f7f7000,
    COULD_NOT_GET_ADDRESS_OF_INIT,
    SETTINGS_POINTER_MUST_NOT_BE_NULL,
    SETTINGS_HAVE_BAD_SIZE,
    WRONG_MAGIC_STRING_IN_SETTINGS,
    ERROR_DURING_INITIALIZATION
};

struct settings
{
    size_t size;
    char magic_string[32];

    static void initialize(settings& settings);
    static error_codes check(const settings* settings);
};
static_assert(std::is_pod<settings>::value, "settings must be a POD type");

}
}

#endif
