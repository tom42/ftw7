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
#include <Windows.h>
#include <boost/test/unit_test.hpp>
#include "ftw7_core/windows/format_message.hpp"
#include "test_asm86.hpp"

namespace
{

class program_runner
{
public:
    program_runner(const ftw7_core::assembler::bytevector& program)
    {
        mem = VirtualAlloc(nullptr, program.size(), MEM_COMMIT,
            PAGE_EXECUTE_READWRITE);
        if (!mem)
        {
            DWORD error = GetLastError();
            throw std::runtime_error("VirtualAlloc failed: " +
                ftw7_core::windows::format_message_from_system(error));
        }

        memcpy(mem, &program[0], program.size());
    }

    ~program_runner()
    {
        if (mem)
        {
            VirtualFree(mem, 0, MEM_RELEASE);
        }
    }

    void run() const
    {
        typedef void (*entry_point)();
        entry_point ep = static_cast<entry_point>(mem);
        ep();
    }

private:
    void* mem;
};

BOOST_AUTO_TEST_CASE(asm86_opcodes_test)
{
    using namespace ftw7_core::assembler;
    test_asm86 a(1, 0);

    // Place a breakpoint followed by a return instruction right at the
    // beginning, so that we can safely step into and later out of the code.
    // This is useful in case we want to examine the code using the debugger.
    A(int3(), 0xcc);
    A(ret(), 0xc3);

    // call r32
    A(call(eax), 0xff, 0xd0);
    A(call(ecx), 0xff, 0xd1);
    A(call(edx), 0xff, 0xd2);
    A(call(ebx), 0xff, 0xd3);
    A(call(esp), 0xff, 0xd4);
    A(call(ebp), 0xff, 0xd5);
    A(call(esi), 0xff, 0xd6);
    A(call(edi), 0xff, 0xd7);

    // cmp r32, imm32
    A(cmp(eax, 0x12345678), 0x81, 0xf8, 0x78, 0x56, 0x34, 0x12);
    A(cmp(ecx, 0x12345678), 0x81, 0xf9, 0x78, 0x56, 0x34, 0x12);
    A(cmp(edx, 0x12345678), 0x81, 0xfa, 0x78, 0x56, 0x34, 0x12);
    A(cmp(ebx, 0x12345678), 0x81, 0xfb, 0x78, 0x56, 0x34, 0x12);
    A(cmp(esp, 0x12345678), 0x81, 0xfc, 0x78, 0x56, 0x34, 0x12);
    A(cmp(ebp, 0x12345678), 0x81, 0xfd, 0x78, 0x56, 0x34, 0x12);
    A(cmp(esi, 0x12345678), 0x81, 0xfe, 0x78, 0x56, 0x34, 0x12);
    A(cmp(edi, 0x12345678), 0x81, 0xff, 0x78, 0x56, 0x34, 0x12);

    A(int3(), 0xcc);

    // jcc rel8
    A(jz(0u), 0x74, 0x00);
    A(je(1u), 0x74, 0x01);
    A(jnz(2u), 0x75, 0x02);
    A(jne(3u), 0x75, 0x03);

    // jmp rel32
    A(jmp(0u), 0xe9, 0x00, 0x00, 0x00, 0x00);
    A(jmp(0x12345678), 0xe9, 0x78, 0x56, 0x34, 0x12);
    A(label("1").jmp("1"), 0xe9, 0xfb, 0xff, 0xff, 0xff);
    A(jmp("2").label("2"), 0xe9, 0x00, 0x00, 0x00, 0x00);

    // mov r32, r32
    // Just check a few combinations, not all 64.
    A(mov(eax, edi), 0x8b, 0xc7);
    A(mov(ecx, esi), 0x8b, 0xce);
    A(mov(edx, ebp), 0x8b, 0xd5);
    A(mov(ebx, esp), 0x8b, 0xdc);
    A(mov(esp, ebx), 0x8b, 0xe3);
    A(mov(ebp, edx), 0x8b, 0xea);
    A(mov(esi, ecx), 0x8b, 0xf1);
    A(mov(edi, eax), 0x8b, 0xf8);

    // mov r32, imm32
    A(mov(eax, 0x12345678), 0xb8, 0x78, 0x56, 0x34, 0x12);
    A(mov(ecx, 0x12345678), 0xb9, 0x78, 0x56, 0x34, 0x12);
    A(mov(edx, 0x12345678), 0xba, 0x78, 0x56, 0x34, 0x12);
    A(mov(ebx, 0x12345678), 0xbb, 0x78, 0x56, 0x34, 0x12);
    A(mov(esp, 0x12345678), 0xbc, 0x78, 0x56, 0x34, 0x12);
    A(mov(ebp, 0x12345678), 0xbd, 0x78, 0x56, 0x34, 0x12);
    A(mov(esi, 0x12345678), 0xbe, 0x78, 0x56, 0x34, 0x12);
    A(mov(edi, 0x78563412), 0xbf, 0x12, 0x34, 0x56, 0x78);

    A(nop(), 0x90);

    // or r32, r32
    // Just check a few combinations, not all 64.
    A(or(eax, edi), 0x0b, 0xc7);
    A(or(ecx, esi), 0x0b, 0xce);
    A(or(edx, ebp), 0x0b, 0xd5);
    A(or(ebx, esp), 0x0b, 0xdc);
    A(or(esp, ebx), 0x0b, 0xe3);
    A(or(ebp, edx), 0x0b, 0xea);
    A(or(esi, ecx), 0x0b, 0xf1);
    A(or(edi, eax), 0x0b, 0xf8);

    // pop r32
    A(pop(eax), 0x58);
    A(pop(ecx), 0x59);
    A(pop(edx), 0x5a);
    A(pop(ebx), 0x5b);
    A(pop(esp), 0x5c);
    A(pop(ebp), 0x5d);
    A(pop(esi), 0x5e);
    A(pop(edi), 0x5f);

    A(popa(), 0x61);
    A(popf(), 0x9d);

    // push r32
    A(push(eax), 0x50);
    A(push(ecx), 0x51);
    A(push(edx), 0x52);
    A(push(ebx), 0x53);
    A(push(esp), 0x54);
    A(push(ebp), 0x55);
    A(push(esi), 0x56);
    A(push(edi), 0x57);

    // push imm32
    A(push(0x12345678), 0x68, 0x78, 0x56, 0x34, 0x12);
    A(push(0x78563412), 0x68, 0x12, 0x34, 0x56, 0x78);

    A(pusha(), 0x60);
    A(pushf(), 0x9c);
    A(ret(), 0xc3);

    // xor r32, r32
    // Just check a few combinations, not all 64.
    A(xor(eax, edi), 0x33, 0xc7);
    A(xor(ecx, esi), 0x33, 0xce);
    A(xor(edx, ebp), 0x33, 0xd5);
    A(xor(ebx, esp), 0x33, 0xdc);
    A(xor(esp, ebx), 0x33, 0xe3);
    A(xor(ebp, edx), 0x33, 0xea);
    A(xor(esi, ecx), 0x33, 0xf1);
    A(xor(edi, eax), 0x33, 0xf8);

    // Executing the generated code will trigger a breakpoint and thus fail
    // the unit test. Uncomment this for debugging purposes only.
    //program_runner(a.a.link(0)).run();
}

}
