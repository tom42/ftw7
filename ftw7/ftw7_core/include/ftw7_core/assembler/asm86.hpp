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
#ifndef FTW7_CORE_ASSEMBLER_ASM86_HPP_INCLUDED
#define FTW7_CORE_ASSEMBLER_ASM86_HPP_INCLUDED

#include <stdexcept>
#include <string>
#include "ftw7_core/assembler/gpreg32.hpp"
#include "ftw7_core/assembler/imm32.hpp"
#include "ftw7_core/assembler/detail/object.hpp"
#include "ftw7_core/assembler/detail/reference.hpp"

namespace ftw7_core
{
namespace assembler
{

class asm86
{
public:
    typedef dword_t address_type;

    asm86(address_type alignment) : obj(alignment) {}

    // It may be necessary to know the program's size before the program is
    // linked. Consider the following scenario:
    //
    //   1. You generate some code to be injected into another process.
    //   2. Only after you've generated the code you know how much memory to
    //      allocate in the other process' address space.
    //   3. Only after you've allocated the memory you know which address to
    //      relocate your code to.
    address_type program_size() const
    {
        return obj.counter();
    }

    bytevector link(address_type origin)
    {
        obj.link(origin);
        return obj.to_bytevector();
    }

    #include "ftw7_core/assembler/detail/asm86_directives.ipp"
    #include "ftw7_core/assembler/detail/asm86_instructions.ipp"
private:
    asm86(const asm86&) = delete;
    asm86& operator = (const asm86&) = delete;

    #include "ftw7_core/assembler/detail/asm86_instruction_helpers.ipp"
    detail::object<address_type> obj;
};

}
}

#endif
