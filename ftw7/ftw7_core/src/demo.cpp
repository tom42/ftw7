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
#include <iostream>
#include <sstream>
#include "ftw7_core/demo.hpp"

namespace ftw7_core
{

namespace
{

std::wstring build_command_line(const std::wstring& exe_path)
{
    std::wostringstream s;
    s << L'"' << exe_path << L'"';
    return s.str();
}

}

void run_demo(const std::wstring& demo_executable_path)
{
    auto command_line = build_command_line(demo_executable_path);

    std::wcout << L"Starting: " << command_line << std::endl;
}

}
