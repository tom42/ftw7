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
#ifndef FTW7_CORE_TEST_ASM86_HPP_INCLUDED
#define FTW7_CORE_TEST_ASM86_HPP_INCLUDED

#include <vector>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/test/unit_test.hpp>
#include "ftw7_core/assembler/asm86.hpp"

#define A(instruction, ...)                                         \
{                                                                   \
    /* Assemble instruction */                                      \
    a.a.instruction;                                                \
                                                                    \
    /* Create a temporary array containing the expected bytes.  */  \
    /* Add one byte more to avoid array of size 0 errors when   */  \
    /* no expected bytes are supplied.                          */  \
    const uint8_t opcode[] = { 0, __VA_ARGS__ };                    \
                                                                    \
    /* Create an expected_instruction and add it to the list.   */  \
    /* Skip the bogus byte we added to the beginning of opcode. */  \
    test_asm86::expected_instruction insn(__LINE__,                 \
        &opcode[1], _countof(opcode) - 1);                          \
    a.expected_instructions.push_back(insn);                        \
}

class test_asm86 : boost::noncopyable
{
private:
    size_t expected_number_of_bytes() const
    {
        size_t result = 0;
        BOOST_FOREACH(const expected_instruction& insn, expected_instructions)
        {
            result += insn.opcode.size();
        }
        return result;
    }

    void check_program(const ftw7_core::assembler::bytevector& program) const
    {
        BOOST_CHECK_EQUAL(program.size(), expected_number_of_bytes());

        // Compare expected against actual bytes.
        ftw7_core::assembler::bytevector::size_type actual_byte_index = 0;
        BOOST_FOREACH(const expected_instruction& insn, expected_instructions)
        {
            BOOST_FOREACH(auto expected_byte, insn.opcode)
            {
                // Use at() to get range checking for free.
                const auto actual_byte = program.at(actual_byte_index);

                BOOST_CHECK_MESSAGE(
                    actual_byte == expected_byte,
                    std::hex << std::showbase <<
                    "wrong byte at address " << actual_byte_index << ". " <<
                    "expected " << int(expected_byte) << ", " <<
                    "actual " << int(actual_byte) << ". " <<
                    "mnemonic is at line " << std::dec << insn.line << "."
                );

                ++actual_byte_index;
            }
        }
    }

public:
    class expected_instruction
    {
    public:
        expected_instruction(int line, const uint8_t* opcode, size_t nbytes)
            : line(line),
            opcode(opcode, &opcode[nbytes])
        {}

        const int line;
        const std::vector<uint8_t> opcode;

    private:
        // Silence MSVC C4512 warnings (assignment operator could not be generated).
        expected_instruction& operator = (const expected_instruction&);
    };

    typedef std::vector<test_asm86::expected_instruction> expected_instruction_list;

    test_asm86(ftw7_core::assembler::asm86::address_type alignment,
        ftw7_core::assembler::asm86::address_type origin)
        : a(alignment),
        origin(origin)
    {}

    ~test_asm86()
    {
        auto program = a.link(origin);
        check_program(program);
    }

    ftw7_core::assembler::asm86 a;
    const ftw7_core::assembler::asm86::address_type origin;
    expected_instruction_list expected_instructions;
};

#endif
