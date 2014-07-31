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
#include <cstdlib>
#include <iostream>
#include "commandline.hpp"
#include "ftw7_core/demo.hpp"
#include "ftw7_core/wexcept.hpp"

template <typename char_type>
void print_error(const char* program_name, const char_type* message)
{
    std::wcerr << program_name << L": " << message << std::endl;
}

template <typename char_type>
void print_error(const char* program_name, const std::basic_string<char_type>& message)
{
    print_error(program_name, message.c_str());
}

int main(int argc, char *argv[])
{
    try
    {
        auto args = ftw7::parse_command_line(argc, argv);
        ftw7_core::run_demo(args.demo_command_line, args.demo_settings);
        return EXIT_SUCCESS;
    }
    catch (const std::logic_error& e)
    {
        print_error(argv[0], std::string("bug: ") + e.what());
    }
    catch (const ftw7_core::wruntime_error& e)
    {
        print_error(argv[0], e.wwhat());
    }
    catch (const std::exception& e)
    {
        print_error(argv[0], e.what());
    }
    return EXIT_FAILURE;
}
