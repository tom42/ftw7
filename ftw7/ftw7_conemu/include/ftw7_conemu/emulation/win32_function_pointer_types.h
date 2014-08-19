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
#ifndef FTW7_CONEMU_EMULATION_WIN32_FUNCTION_POINTER_TYPES_H_INCLUDED
#define FTW7_CONEMU_EMULATION_WIN32_FUNCTION_POINTER_TYPES_H_INCLUDED

/*
* Hack for GLFW:
*
* GLFW insists in not including Windows.h but defines its own symbols and
* messes around with WIN32_LEAN_AND_MEAN. This leads of course to the usual
* mess, in this case a ton of annoying warnings about redefinitions of
* WIN32_LEAN_AND_MEAN, and thus we came up with the following hack:
*
* When GLFW is compiled we include its internal.h rather than Windows.h,
* and everything compiles without any warnings.
*
* This should be pretty much OK, since the first thing in the GLFW sources
* is the inclusion of internal.h. So if we introduce a prologue header
* that includes internal.h before doing anything else, then we shouldn't
* have changed anything from the compiler's point of view.
*/
#ifdef FTW7_COMPILING_GLFW
#include "internal.h"
#else
#include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* kernel32 */
typedef BOOL(WINAPI* AllocConsole_ptr_t)(void);
typedef BOOL(WINAPI* SetConsoleActiveScreenBuffer_ptr_t)(HANDLE);
typedef BOOL(WINAPI* SetConsoleCursorInfo_ptr_t)(HANDLE, const CONSOLE_CURSOR_INFO*);
typedef BOOL(WINAPI* SetConsoleTitleA_ptr_t)(LPCSTR);
typedef BOOL(WINAPI* SetConsoleTitleW_ptr_t)(LPCWSTR);
typedef BOOL(WINAPI* WriteConsoleOutputA_ptr_t)(HANDLE, const CHAR_INFO*, COORD, COORD, PSMALL_RECT);

/* user32 */
typedef int (WINAPI* ShowCursor_ptr_t)(BOOL);
typedef BOOL(WINAPI* ShowWindow_ptr_t)(HWND, int);

#ifdef __cplusplus
}
#endif

#endif
