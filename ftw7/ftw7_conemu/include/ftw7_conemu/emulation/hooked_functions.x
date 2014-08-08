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

 // xheader to generate hook related types, variables etc.
 // This file intentionally doesn't have a header guard.
 FTW7_CONEMU_XHOOKED_FUNCTION(kernel32, AllocConsole)
 FTW7_CONEMU_XHOOKED_FUNCTION(kernel32, SetConsoleActiveScreenBuffer)
 FTW7_CONEMU_XHOOKED_FUNCTION(kernel32, SetConsoleTitleA)
 FTW7_CONEMU_XHOOKED_FUNCTION(kernel32, SetConsoleTitleW)
 FTW7_CONEMU_XHOOKED_FUNCTION(kernel32, WriteConsoleOutputA)
 FTW7_CONEMU_XHOOKED_FUNCTION(user32, ShowCursor)
 FTW7_CONEMU_XHOOKED_FUNCTION(user32, ShowWindow)
