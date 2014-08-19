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
#include <stdexcept>

struct hooked_function
{
    const char* dllname;
    const char* procname;
};

const hooked_function hooked_functions[] =
{
    { "kernel32", "AllocConsole" },
    { "kernel32", "SetConsoleActiveScreenBuffer" },
    { "kernel32", "SetConsoleTitleA" },
    { "kernel32", "SetConsoleTitleW" },
    { "kernel32", "WriteConsoleOutputA" },
    { "user32", "ShowCursor" },
    { "user32", "ShowWindow" },
};
const size_t n_hooked_functions = sizeof(hooked_functions) / sizeof(hooked_functions[0]);

void generate_hooked_functions_header(std::ostream& os)
{
    // TODO: proper header guard (ideally generated from output filename...)
    os << "/* Generated source code. Do not modify. */" << std::endl;
    os << "#ifndef FTGW7_RULEZ_H_INCLUDED" << std::endl;
    os << "#define FTGW7_RULEZ_H_INCLUDED" << std::endl;
    os << std::endl;

    os << "#include \"win32_function_pointer_types.h\"" << std::endl;
    os << std::endl;

    os << "#ifdef __cplusplus" << std::endl;
    os << "#extern \"C\" {" << std::endl;
    os << "#endif" << std::endl;
    os << std::endl;

    // Generate hooked function names to true function pointer redirection macros.
    os << "#ifdef FTW7_COMPILING_GLFW" << std::endl;
    for (size_t i = 0; i < n_hooked_functions; ++i)
    {
        os << "#define " << hooked_functions[i].procname << " true_" <<
            hooked_functions[i].procname << ";" << std::endl;
    }
    os << "#endif" << std::endl;
    os << std::endl;

    // Generate extern declarations for true function pointers.
    for (size_t i = 0; i < n_hooked_functions; ++i)
    {
        os << "extern " << hooked_functions[i].procname << "_ptr_t true_" <<
            hooked_functions[i].procname << ";" << std::endl;
    }
    os << std::endl;

    os << "#ifdef __cplusplus" << std::endl;
    os << "}" << std::endl;
    os << "#endif" << std::endl;
    os << std::endl;

    os << "#endif" << std::endl;
}

int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        std::cout.exceptions(std::ostream::badbit | std::ostream::eofbit | std::ostream::failbit);
        generate_hooked_functions_header(std::cout);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
