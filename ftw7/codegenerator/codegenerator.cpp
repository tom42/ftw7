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
    const char* function_pointer_typedef;
};

const hooked_function hooked_functions[] =
{
    { "kernel32",   "AllocConsole",                 "typedef BOOL(WINAPI* AllocConsole_ptr_t)(void);" },
    { "kernel32",   "SetConsoleActiveScreenBuffer", "typedef BOOL(WINAPI* SetConsoleActiveScreenBuffer_ptr_t)(HANDLE);" },
    { "kernel32",   "SetConsoleTitleA",             "typedef BOOL(WINAPI* SetConsoleTitleA_ptr_t)(LPCSTR);" },
    { "kernel32",   "SetConsoleTitleW",             "typedef BOOL(WINAPI* SetConsoleTitleW_ptr_t)(LPCWSTR);" },
    { "kernel32",   "WriteConsoleOutputA",          "typedef BOOL(WINAPI* WriteConsoleOutputA_ptr_t)(HANDLE, const CHAR_INFO*, COORD, COORD, PSMALL_RECT);" },
    { "user32",     "ShowCursor",                   "typedef int (WINAPI* ShowCursor_ptr_t)(BOOL);" },
    { "user32",     "ShowWindow",                   "typedef BOOL(WINAPI* ShowWindow_ptr_t)(HWND, int);" },
};
const size_t n_hooked_functions = sizeof(hooked_functions) / sizeof(hooked_functions[0]);

void generate_hooked_functions_header(std::ostream& os)
{
    // TODO: proper header guard (ideally generated from output filename...)
    os << "/* Generated source code. Do not modify. */" << std::endl;
    os << "#ifndef FTW7_RULEZ_H_INCLUDED" << std::endl;
    os << "#define FTW7_RULEZ_H_INCLUDED" << std::endl;
    os << std::endl;

    // Hack for GLFW:
    //
    // GLFW insists in not including Windows.h but defines its own symbols and
    // messes around with WIN32_LEAN_AND_MEAN. This leads of course to the usual
    // mess, in this case a ton of annoying warnings about redefinitions of
    // WIN32_LEAN_AND_MEAN, and thus we came up with the following hack:
    //
    // When GLFW is compiled we include its internal.h rather than Windows.h,
    // and everything compiles without any warnings.
    //
    // This should be pretty much OK, since the first thing in the GLFW sources
    // is the inclusion of internal.h. So if we introduce a prologue header
    // that includes internal.h before doing anything else, then we shouldn't
    // have changed anything from the compiler's point of view.
    os << "#ifdef FTW7_COMPILING_GLFW" << std::endl;
    os << "#include \"internal.h\"" << std::endl;
    os << "#else" << std::endl;
    os << "#include <Windows.h>" << std::endl;
    os << "#endif" << std::endl;
    os << std::endl;

    os << "#ifdef __cplusplus" << std::endl;
    os << "extern \"C\" {" << std::endl;
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

    // Generate function pointer typedefs.
    for (size_t i = 0; i < n_hooked_functions; ++i)
    {
        os << hooked_functions[i].function_pointer_typedef << std::endl;
    }
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
