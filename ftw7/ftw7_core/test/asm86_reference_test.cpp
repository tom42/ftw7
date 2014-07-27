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
#include "ftw7_core/assembler/basic_datatypes.hpp"
#include "ftw7_core/assembler/detail/reference.hpp"

namespace
{

using namespace ftw7_core::assembler;
using namespace ftw7_core::assembler::detail;

BOOST_AUTO_TEST_SUITE(asm86_reference_test)

BOOST_AUTO_TEST_CASE(asm86_absolute_reference_creation_test)
{
    reference<dword_t> dword_reference("d", 23);
    BOOST_CHECK_EQUAL(dword_reference.label(), "d");
    BOOST_CHECK_EQUAL(dword_reference.fixup_location(), 23u);
    BOOST_CHECK_EQUAL(dword_reference.fixup_size_bytes(), 4u);
    BOOST_CHECK_EQUAL(dword_reference.base_address(), 0u);
    BOOST_CHECK_EQUAL(dword_reference.is_relative(), false);
    BOOST_CHECK_EQUAL(dword_reference.fixup_msb_mask(), 0x80000000);

    reference<word_t> word_reference("w", 42);
    BOOST_CHECK_EQUAL(word_reference.label(), "w");
    BOOST_CHECK_EQUAL(word_reference.fixup_location(), 42u);
    BOOST_CHECK_EQUAL(word_reference.fixup_size_bytes(), 2u);
    BOOST_CHECK_EQUAL(word_reference.base_address(), 0u);
    BOOST_CHECK_EQUAL(word_reference.is_relative(), false);
    BOOST_CHECK_EQUAL(word_reference.fixup_msb_mask(), 0x8000);

    reference<byte_t> byte_reference("b", 64);
    BOOST_CHECK_EQUAL(byte_reference.label(), "b");
    BOOST_CHECK_EQUAL(byte_reference.fixup_location(), 64u);
    BOOST_CHECK_EQUAL(byte_reference.fixup_size_bytes(), 1u);
    BOOST_CHECK_EQUAL(byte_reference.base_address(), 0u);
    BOOST_CHECK_EQUAL(byte_reference.is_relative(), false);
    BOOST_CHECK_EQUAL(byte_reference.fixup_msb_mask(), 0x80);
}

BOOST_AUTO_TEST_CASE(asm86_relative_reference_creation_test)
{
    auto dword_8 = reference<dword_t>::make_rel<byte_t>("dword_8", 1337, 23);
    BOOST_CHECK_EQUAL(dword_8.label(), "dword_8");
    BOOST_CHECK_EQUAL(dword_8.fixup_location(), 1337u);
    BOOST_CHECK_EQUAL(dword_8.fixup_size_bytes(), 1u);
    BOOST_CHECK_EQUAL(dword_8.base_address(), 23u);
    BOOST_CHECK_EQUAL(dword_8.is_relative(), true);
    BOOST_CHECK_EQUAL(dword_8.fixup_msb_mask(), 0x80u);

    auto dword_16 = reference<dword_t>::make_rel<word_t>("dword_16", 1234, 42);
    BOOST_CHECK_EQUAL(dword_16.label(), "dword_16");
    BOOST_CHECK_EQUAL(dword_16.fixup_location(), 1234u);
    BOOST_CHECK_EQUAL(dword_16.fixup_size_bytes(), 2u);
    BOOST_CHECK_EQUAL(dword_16.base_address(), 42u);
    BOOST_CHECK_EQUAL(dword_16.is_relative(), true);
    BOOST_CHECK_EQUAL(dword_16.fixup_msb_mask(), 0x8000u);

    auto dword_32 = reference<dword_t>::make_rel<dword_t>("dword_32", 0x12345678, 1337);
    BOOST_CHECK_EQUAL(dword_32.label(), "dword_32");
    BOOST_CHECK_EQUAL(dword_32.fixup_location(), 0x12345678u);
    BOOST_CHECK_EQUAL(dword_32.fixup_size_bytes(), 4u);
    BOOST_CHECK_EQUAL(dword_32.base_address(), 1337u);
    BOOST_CHECK_EQUAL(dword_32.is_relative(), true);
    BOOST_CHECK_EQUAL(dword_32.fixup_msb_mask(), 0x80000000u);
}

BOOST_AUTO_TEST_SUITE_END()

}
