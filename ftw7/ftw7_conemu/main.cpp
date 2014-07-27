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

// This function needs to be called by the injected code to bring up the console
// emulation. Note that it mustn't propagate any C++ exceptions up to the caller;
// The caller is our injected code and can't handle C++ exceptions.
//
// For convenience this function is using the __stdcall calling convention.
//
// __stdcall functions get a decorated name by default (e.g. _get_version@0).
// The most convenient way to get rid of name decoration for __stdcall functions
// is through a .def file.
//
// Because we're using a .def file anyway we can also omit the dllexport bit
// here.
extern "C" int __stdcall ftw7_conemu_initialize()
{
    // TODO: initialize (logging, display)
    // TODO: catch all exceptions
    return 0;
}
