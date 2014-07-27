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
#include "ftw7_core/assembler/gpreg32.hpp"

namespace ftw7_core
{
namespace assembler
{

const gpreg32 gpreg32::eax(0);
const gpreg32 gpreg32::ecx(1);
const gpreg32 gpreg32::edx(2);
const gpreg32 gpreg32::ebx(3);
const gpreg32 gpreg32::esp(4);
const gpreg32 gpreg32::ebp(5);
const gpreg32 gpreg32::esi(6);
const gpreg32 gpreg32::edi(7);

const gpreg32& eax = gpreg32::eax;
const gpreg32& ecx = gpreg32::ecx;
const gpreg32& edx = gpreg32::edx;
const gpreg32& ebx = gpreg32::ebx;
const gpreg32& esp = gpreg32::esp;
const gpreg32& ebp = gpreg32::ebp;
const gpreg32& esi = gpreg32::esi;
const gpreg32& edi = gpreg32::edi;

}
}
