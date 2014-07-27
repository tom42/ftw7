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
#ifndef FTW7_CORE_ASSEMBLER_GPREG32_HPP_INCLUDED
#define FTW7_CORE_ASSEMBLER_GPREG32_HPP_INCLUDED

#include "ftw7_core/assembler/basic_datatypes.hpp"

namespace ftw7_core
{
namespace assembler
{

class gpreg32
{
public:
    // General purpose registers, sorted by index.
    static const gpreg32 eax;
    static const gpreg32 ecx;
    static const gpreg32 edx;
    static const gpreg32 ebx;
    static const gpreg32 esp;
    static const gpreg32 ebp;
    static const gpreg32 esi;
    static const gpreg32 edi;

    byte_t number() const
    {
        return m_number;
    }

private:
    explicit gpreg32(byte_t number) : m_number(number) {}

    gpreg32(const gpreg32&);
    gpreg32& operator = (const gpreg32&);

    const byte_t m_number;
};

// Shortcuts.
// These allow writing 'push(eax)' rather than 'push(gpreg32::eax)'.
extern const gpreg32& eax;
extern const gpreg32& ecx;
extern const gpreg32& edx;
extern const gpreg32& ebx;
extern const gpreg32& esp;
extern const gpreg32& ebp;
extern const gpreg32& esi;
extern const gpreg32& edi;

}
}

#endif
