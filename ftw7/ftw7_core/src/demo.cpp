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
#include <filesystem>
#include <iostream>
#include <sstream>
#include "ftw7_core/demo.hpp"
#include "process.hpp"

namespace ftw7_core
{

namespace
{

std::wstring build_command_line(const std::wstring& exe_path)
{
    // TODO: quoting required here?
    std::wostringstream s;
    s << L'"' << exe_path << L'"';
    return s.str();
}

std::wstring get_working_directory(const std::wstring& exe_path)
{
    return std::tr2::sys::wpath(exe_path).parent_path().external_directory_string();
}

}

void run_demo(const std::wstring& demo_executable_path)
{
    // TODO: more stability?
    // * Check whether we can find demo_executable at all
    // * If so we can build an absolute path ourselves
    // * From that we can derive a working directory
    // * If the working directory is empty (because there's no parent for demo_executable_path)
    //   this is not yet an error, but we should apparently not throw empty strings at
    //   CreateProcess but use NULL instead. Need to investigate this.
    // * Ensure command line building is done correcty (quoting and that)
    auto command_line = build_command_line(demo_executable_path);
    auto working_directory = get_working_directory(demo_executable_path);

    std::wcout << L"Attempting to run: " << demo_executable_path << std::endl;
    std::wcout << L"Working directory: " << working_directory << std::endl;
    std::wcout << L"Command line: " << command_line << std::endl;

    process process(demo_executable_path, command_line, working_directory);
    std::wcout << L"Successfully created process (PID=" << process.process_id() << L')' << std::endl;
}

}
