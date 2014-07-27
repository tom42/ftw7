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
#include <string>
#include <boost/test/unit_test.hpp>
#include "test_asm86.hpp"

namespace
{

using namespace ftw7_core::assembler;

BOOST_AUTO_TEST_SUITE(asm86_directives_test);

BOOST_AUTO_TEST_CASE(asm86_data_directive_test)
{
    test_asm86 a(1, 0);
    const byte_t b = 0x23;
    const word_t w = 0x1337;
    const dword_t d = 0xbaadf00d;

    A(data(&b, sizeof(b)), 0x23);
    A(data(&w, sizeof(w)), 0x37, 0x13);
    A(data(&d, sizeof(d)), 0x0d, 0xf0, 0xad, 0xba);
}

BOOST_AUTO_TEST_CASE(asm86_string_z_directive_test)
{
    test_asm86 a(1, 0x00);
    A(string_z(""), 0x00);
    A(string_z("a"), 0x61, 0x00);
    A(string_z(std::string("hello")), 'h', 'e', 'l', 'l', 'o', 0x00);
}

BOOST_AUTO_TEST_CASE(asm86_wstring_z_directive_test)
{
    test_asm86 a(1, 0);
    A(wstring_z(L""), 0x00, 0x00);
    A(wstring_z(L"a"), 0x61, 0x00, 0x00, 0x00);
    A(wstring_z(std::wstring(L"hello")),
        'h', 0, 'e', 0, 'l', 0, 'l', 0, 'o', 0, 0, 0);
}

BOOST_AUTO_TEST_SUITE_END()

}
