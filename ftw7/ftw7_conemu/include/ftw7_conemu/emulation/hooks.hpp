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
#ifndef FTW7_CONEMU_EMULATION_HOOKS_HPP_INCLUDED
#define FTW7_CONEMU_EMULATION_HOOKS_HPP_INCLUDED

#include <Windows.h>

namespace ftw7_conemu
{
namespace emulation
{

// Create function pointer typedefs from functions listed in the xheader.
// This yields typedefs such as SetConsoleTitleA_ptr_t.
#define FTW7_CONEMU_XHOOKED_FUNCTION(dllname, procname) typedef decltype(procname)* procname##_ptr_t;
#include "ftw7_conemu/emulation/hooked_functions.x"
#undef FTW7_CONEMU_XHOOKED_FUNCTION

// Create declarations for the variables that hold the addresses of the true functions.
// This yields declarations such as: extern SetConsoleTitleA_ptr_t true_SetConsoleTitleA;
#define FTW7_CONEMU_XHOOKED_FUNCTION(dllname, procname) extern procname##_ptr_t true_##procname;
#include "ftw7_conemu/emulation/hooked_functions.x"
#undef FTW7_CONEMU_XHOOKED_FUNCTION

}
}

#endif
