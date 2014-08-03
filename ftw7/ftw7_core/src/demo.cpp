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
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <boost/io/ios_state.hpp>
#include "ftw7_core/assembler/asm86.hpp"
#include "ftw7_core/demo.hpp"
#include "ftw7_core/emulation/emulation.hpp"
#include "ftw7_core/windows/module.hpp"
#include "ftw7_core/ptr_to_int.hpp"
#include "process.hpp"

namespace ftw7_core
{

namespace
{

// Quote a command line argument, so that it can be properly processed by CreateProcessW.
// Straight from here:
// http://blogs.msdn.com/b/twistylittlepassagesallalike/archive/2011/04/23/everyone-quotes-arguments-the-wrong-way.aspx
//
// argument: argument to quote
// command_line: string to append the quoted argument to
// force: if true, arguments are quoted even if not necessary
void argv_quote(const std::wstring& argument, std::wstring& command_line, bool force)
{
    // Unless we're told otherwise, don't quote unless we actually
    // need to do so --- hopefully avoid problems if programs won't
    // parse quotes properly.
    if ((force == false) && !argument.empty() && (argument.find_first_of(L" \t\n\v\"") == argument.npos))
    {
        command_line.append(argument);
        return;
    }

    command_line.push_back(L'"');
    for (auto i = argument.begin();; ++i)
    {
        unsigned n_backslashes = 0;
        while ((i != argument.end()) && (*i == L'\\'))
        {
            ++i;
            ++n_backslashes;
        }

        if (i == argument.end())
        {
            // Escape all backslashes, but let the terminating double quotation
            // mark we add below be interpreted as a metacharacter.
            command_line.append(n_backslashes * 2, L'\\');
            break;
        }
        else if (*i == L'"')
        {
            // Escape all backslashes and the following double quotation mark.
            command_line.append(n_backslashes * 2 + 1, L'\\');
            command_line.push_back(*i);
        }
        else
        {
            // Backslashes aren't special here.
            command_line.append(n_backslashes, L'\\');
            command_line.push_back(*i);
        }
    }
    command_line.push_back(L'"');
}

std::wstring build_command_line_string(const std::vector<std::wstring>& demo_command_line)
{
    std::wstring command_line_string;
    bool is_first_argument = true;
    for (const auto& arg : demo_command_line)
    {
        if (!is_first_argument)
        {
            command_line_string.push_back(L' ');
        }
        is_first_argument = false;
        argv_quote(arg, command_line_string, false);
    }
    return command_line_string;
}

std::wstring get_working_directory(const std::wstring& exe_path)
{
    return std::tr2::sys::wpath(exe_path).parent_path().external_directory_string();
}

DWORD get_creation_flags(const demo_settings& settings)
{
    DWORD flags = 0;
    if (settings.separate_console)
    {
        flags |= CREATE_NEW_CONSOLE;
    }
    return flags;
}

std::wstring get_emulation_dll_path()
{
    auto directory = std::tr2::sys::wpath(windows::get_module_filename(nullptr)).parent_path();
    directory /= L"ftw7_conemu.dll";
    return directory.external_directory_string();
}

void create_injection_code(assembler::asm86& a, DWORD return_address)
{
    using namespace ftw7_core::assembler;
    using namespace ftw7_core::windows;

    const auto kernel32 = get_module_handle(L"kernel32.dll");
    const auto ExitProcess_ptr = ptr_to_int<dword_t>(get_proc_address(kernel32, "ExitProcess"));
    const auto GetProcAddress_ptr = ptr_to_int<dword_t>(get_proc_address(kernel32, "GetProcAddress"));
    const auto LoadLibraryW_ptr = ptr_to_int<dword_t>(get_proc_address(kernel32, "LoadLibraryW"));

    // TODO: not here (testcode). This needs to be passed all the way from run_demo, which gets it from the application.
    emulation::settings settings;
    emulation::settings::initialize(settings);

    // Push return address onto stack and save all registers.
    a.push(return_address);
    a.pushf();
    a.pusha();

    // Load the emulation DLL.
    a.push("emulation_dll_path");
    a.mov(eax, LoadLibraryW_ptr);
    a.call(eax);
    a.mov(ebx, emulation::COULD_NOT_LOAD_EMULATION_DLL);
    a.or(eax, eax);
    a.jz("exit_error");

     // Get address of ftw7_conemu_initialize()
    a.push("ftw7_conemu_initialize_name");  // Push address of function name
    a.push(eax);                            // Push emulation DLL's module handle
    a.mov(eax, GetProcAddress_ptr);
    a.call(eax);
    a.mov(ebx, emulation::COULD_NOT_GET_ADDRESS_OF_INIT);
    a.or(eax, eax);
    a.jz("exit_error");

    // Call ftw7_conemu_initialize()
    a.push("emulation_settings");
    a.call(eax);                            // Call ftw7_conemu_initialize
    a.mov(ebx, eax);                        // EBX = error code from ftw7_conemu_initialize (if any)
    a.or(eax, eax);
    a.jnz("exit_error");

    // Success:
    // Restore all registers and return to main thread's original entry point.
    a.popa();
    a.popf();
    a.ret();

    // Error: call ExitProcess.
    // When jumping to here, EBX must contain the exit status.
    a.label("exit_error");
    a.push(ebx);
    a.mov(eax, ExitProcess_ptr);
    a.call(eax);

    // Data. Wide character strings must be aligned to even addresses.
    a.align(2).label("emulation_dll_path").wstring_z(get_emulation_dll_path());
    a.align(2).label("ftw7_conemu_initialize_name").string_z("ftw7_conemu_initialize");
    a.align(4).label("emulation_settings").data(&settings, sizeof(settings));
}

void inject_emulation(process& process)
{
    auto ctx = process.get_thread_context(CONTEXT_CONTROL);
    assembler::asm86 a(4);
    create_injection_code(a, ctx.Eip);

    // Allocate memory for injection code in the injectee.
    auto code_address = process.virtual_alloc(a.program_size());

    // Relocate injection code to address in injectee's address space.
    auto code = a.link(ptr_to_int<assembler::asm86::address_type>(code_address));

    // Copy injection code into the injectee's address space.
    process.write_process_memory(code_address, &code[0], code.size());
    process.flush_instruction_cache(code_address, code.size());

    // Set thread's instruction pointer to the injected code.
    ctx.Eip = ptr_to_int<DWORD>(code_address);
    process.set_thread_context(ctx);
}

std::wstring format_demo_duration(double duration_s)
{
    auto d = int64_t(duration_s);

    const auto seconds = d % 60;
    d /= 60;
    const auto minutes = d % 60;
    d /= 60;
    const auto hours = d;

    std::wstringstream s;
    s.fill(L'0');
    s << std::setw(2) << hours << L':' <<
        std::setw(2) << minutes << L':' <<
        std::setw(2) << seconds;
    return s.str();
}

}

void run_demo(const std::vector<std::wstring>& demo_command_line, const demo_settings& settings)
{
    // TODO: more stability? (A lot of this depends on how useful CreateProcess behaves)
    // * Check whether we can find demo_executable at all
    // * Check for working directory existance?
    // * If the .exe extension is missing, try appending it ourselves?
    // * If so we can build an absolute path ourselves
    // * From that we can derive a working directory
    // * If the working directory is empty (because there's no parent for demo_executable_path)
    //   this is not yet an error, but we should apparently not throw empty strings at
    //   CreateProcess but use NULL instead. Need to investigate this.
    // * Ensure command line building is done correcty (quoting and that)
    // * Apparently CreateProcess returns before the image is fully loaded.
    //   Not sure this can become a problem.
    // * Should explicitly check demo_command_line contains at least one arg (the demo exe)
    auto demo_executable_path = demo_command_line.at(0);
    auto command_line_string = build_command_line_string(demo_command_line);
    auto working_directory = get_working_directory(demo_executable_path);

    std::wcout << L"Attempting to run: " << demo_executable_path << std::endl;
    std::wcout << L"Working directory: " << working_directory << std::endl;
    std::wcout << L"Command line: " << command_line_string << std::endl;

    process process(demo_executable_path, command_line_string, get_creation_flags(settings), working_directory);
    std::wcout << L"Successfully created process (PID=" << process.process_id() << L')' << std::endl;

    if (process.is_64bit())
    {
        throw wruntime_error(L"cannot run 64 bit program '" + demo_executable_path + L"'");
    }

    inject_emulation(process);
    std::wcout << L"Successfully injected console emulation library" << std::endl;

    if (settings.wait_for_process)
    {
        std::wcout << L"Running demo and waiting for it to terminate" << std::endl;
        auto start_time = time(nullptr);
        const auto exitcode = process.run_and_wait();
        auto end_time = time(nullptr);
        auto duration_s = difftime(end_time, start_time);
        boost::io::ios_flags_saver ifs(std::wcout);
        std::wcout << L"Demo ran for " << duration_s << L" seconds (" << format_demo_duration(duration_s) << L')' << std::endl;
        std::wcout << L"Demo terminated with exit code 0x" << std::hex << exitcode << std::endl;
    }
    else
    {
        std::wcout << L"Running demo without waiting for it to terminate" << std::endl;
        process.run();
    }
}

}
