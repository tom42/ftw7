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
#ifndef FTW7_CORE_ASSEMBLER_BASIC_DATA_TYPES_HPP_INCLUDED
#define FTW7_CORE_ASSEMBLER_BASIC_DATA_TYPES_HPP_INCLUDED

#include <cstdint>
#include <vector>

namespace ftw7_core
{
namespace assembler
{

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t dword_t;

typedef std::vector<byte_t> bytevector;

}
}

#endif
