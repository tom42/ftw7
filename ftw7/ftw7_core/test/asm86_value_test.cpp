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
#include <boost/test/unit_test.hpp>
#include "check_what.hpp"
#include "test_asm86.hpp"

namespace
{

using namespace ftw7_core::assembler;

BOOST_AUTO_TEST_SUITE(asm86_value_test)

BOOST_AUTO_TEST_CASE(asm86_32bit_constant_value_test)
{
    const asm86::address_type origins[] = {0, 0xaaaaaaaa};
    for (const auto origin : origins)
    {
        test_asm86 a(1, origin);
        A(mov(eax, 0x01234567), 0xb8, 0x67, 0x45, 0x23, 0x01);
        A(push(0x89abcdef), 0x68, 0xef, 0xcd, 0xab, 0x89);
    }
}

BOOST_AUTO_TEST_CASE(asm86_32bit_reference_value_test)
{
    {
        test_asm86 a(1, 0);
        A(label("1"));
        A(mov(eax, "1"), 0xb8, 0x00, 0x00, 0x00, 0x00);
        A(push("1"), 0x68, 0x00, 0x00, 0x00, 0x00);
        A(label("2"));
        A(mov(eax, "2"), 0xb8, 0x0a, 0x00, 0x00, 0x00);
        A(push("2"), 0x68, 0x0a, 0x00, 0x00, 0x00);
    }
    {
        test_asm86 a(1, 0x7ffffff7);
        A(label("1"));
        A(mov(eax, "1"), 0xb8, 0xf7, 0xff, 0xff, 0x7f);
        A(push("1"), 0x68, 0xf7, 0xff, 0xff, 0x7f);
        A(label("2"));
        A(mov(eax, "2"), 0xb8, 0x01, 0x00, 0x00, 0x80);
        A(push("2"), 0x68, 0x01, 0x00, 0x00, 0x80);
    }
}

BOOST_AUTO_TEST_CASE(asm86_8bit_constant_value_test)
{
    for (int i = 0; i < 255; ++i)
    {
        test_asm86 a(1, 0);
        A(jz(i), 0x74, (byte_t)i);
    }
}

BOOST_AUTO_TEST_CASE(asm_out_of_range_8bit_constant_value_test)
{
    BOOST_CHECK_THROW(asm86(1).jz(dword_t(-1)), std::out_of_range);
    BOOST_CHECK_THROW(asm86(1).jz(256), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(asm86_8bit_relative_label_simple_test)
{
    test_asm86 a(1, 0);
    A(label("2").nop().jz("2"), 0x90, 0x74, 0xfd);
    A(label("1").jz("1"), 0x74, 0xfe);
    A(jz("3").label("3"), 0x74, 0x00);
    A(jz("4").nop().label("4"), 0x74, 0x01, 0x90);
}

BOOST_AUTO_TEST_CASE(asm86_8bit_relative_label_with_negative_displacement_test)
{
    // Negative displacements:
    // 126..0 bytes between label and relative jump instruction, yields
    // displacements 0x80..0xfe (-128..-2).
    //
    // Note that we can't generate a displacement of -1, since that would
    // require us to jump onto the second byte of the relative jump itself,
    // which is not supported by the assembler.
    int n_iterations = 0;
    for (int i = 126; i >= 0; --i, ++n_iterations)
    {
        test_asm86 a(1, 0);
        A(label("1"));
        for (int j = 0; j < i; ++j)
        {
            A(nop(), 0x90);
        }
        A(jz("1"), 0x74, byte_t(0x80 + 126u - i));
    }
    BOOST_CHECK_EQUAL(n_iterations, 127);
}

BOOST_AUTO_TEST_CASE(asm86_8bit_relative_label_with_positive_displacement_test)
{
    // Positive displacements:
    // 0..127 bytes between relative jump instruction, yields
    // displacements 0x00..0x7f (0..127).
    int n_iterations = 0;
    for (unsigned int i = 0; i < 128; ++i, ++n_iterations)
    {
        test_asm86 a(1, 0);
        A(jz("1"), 0x74, byte_t(i));
        for (unsigned int j = 0; j < i; ++j)
        {
            A(nop(), 0x90);
        }
        A(label("1"));
    }
    BOOST_CHECK_EQUAL(n_iterations, 128);
}

BOOST_AUTO_TEST_CASE(asm86_8bit_relative_label_displacement_too_small_test)
{
    asm86 a(1);
    a.label("1");
    for (int i = 0; i < 127; ++i)
    {
        a.nop();
    }
    a.jz("1");
    BOOST_CHECK_EXCEPTION(a.link(0), std::out_of_range,
        check_what_equals("displacement out of range"));
}

BOOST_AUTO_TEST_CASE(asm86_8bit_relative_label_displacement_too_large_test)
{
    asm86 a(1);
    a.jz("1");
    for (int i = 0; i < 128; ++i)
    {
        a.nop();
    }
    a.label("1");
    BOOST_CHECK_EXCEPTION(a.link(0), std::out_of_range,
        check_what_equals("displacement out of range"));
}

BOOST_AUTO_TEST_SUITE_END()

}
