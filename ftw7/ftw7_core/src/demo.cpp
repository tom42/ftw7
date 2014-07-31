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
#include "ftw7_core/ptr_to_int.hpp"
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

DWORD get_creation_flags(const demo_settings& settings)
{
    DWORD flags = 0;
    if (settings.separate_console)
    {
        flags |= CREATE_NEW_CONSOLE;
    }
    return flags;
}

void create_injection_code(assembler::asm86& a, DWORD return_address)
{
    // TODO: real implementation

    // Push return address onto stack and save all registers.
    a.push(return_address);
    a.pushf();
    a.pusha();

    // Success:
    // Restore all registers and return to main thread's original entry point.
    a.popa();
    a.popf();
    a.ret();
}

void inject_emulation(process& process)
{
    auto ctx = process.get_thread_context(CONTEXT_CONTROL);
    assembler::asm86 a(4);
    create_injection_code(a, ctx.Eip);

    // Allocate memory for injection code in the injectee.
    auto code_address = process.virtual_alloc(a.program_size());

    // Relocate injection code to address in injectee's address space.
    // TODO: mrmpf: in principle, asm86 should do this conversion. Rationale: the assembler knows its address_type...
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

void run_demo(const std::wstring& demo_executable_path, const demo_settings& settings)
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
    auto command_line = build_command_line(demo_executable_path);
    auto working_directory = get_working_directory(demo_executable_path);

    std::wcout << L"Attempting to run: " << demo_executable_path << std::endl;
    std::wcout << L"Working directory: " << working_directory << std::endl;
    std::wcout << L"Command line: " << command_line << std::endl;

    process process(demo_executable_path, command_line, get_creation_flags(settings), working_directory);
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
