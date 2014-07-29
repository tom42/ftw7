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
#ifndef FTW7_CORE_WINDOWS_MODULE_HPP_INCLUDED
#define FTW7_CORE_WINDOWS_MODULE_HPP_INCLUDED

#include <Windows.h>
#include <string>

namespace ftw7_core
{
namespace windows
{

std::wstring get_module_filename(HMODULE module);
HMODULE get_module_handle(const wchar_t* module_name);
FARPROC get_proc_address(HMODULE module, const char* proc_name);
HMODULE load_library(const wchar_t* filename);

}
}

#endif
