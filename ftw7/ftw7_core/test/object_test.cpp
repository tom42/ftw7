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
#include <limits>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include "ftw7_core/assembler/detail/object.hpp"
#include "check_what.hpp"

namespace
{

using namespace ftw7_core::assembler;
using namespace ftw7_core::assembler::detail;

BOOST_AUTO_TEST_SUITE(object_test)

BOOST_AUTO_TEST_CASE(creation_test)
{
    object<dword_t> obj(1);
    BOOST_CHECK_EQUAL(obj.counter(), 0u);
    BOOST_CHECK_EQUAL(obj.to_bytevector().size(), 0u);
}

BOOST_AUTO_TEST_CASE(creation_with_invalid_alignment_test)
{
    BOOST_CHECK_EXCEPTION(object<dword_t>(0), std::invalid_argument,
        check_what_equals("alignment must not be 0"));
}

BOOST_AUTO_TEST_CASE(align_with_valid_values_test)
{
    object<dword_t> obj(16);

    // Test valid alignments. Because we're initially unaligned, counter()
    // will grow.
    obj.emit8(0);
    obj.align(1);
    BOOST_CHECK_EQUAL(obj.counter(), 1u);
    obj.align(2);
    BOOST_CHECK_EQUAL(obj.counter(), 2u);
    obj.align(4);
    BOOST_CHECK_EQUAL(obj.counter(), 4u);
    obj.align(8);
    BOOST_CHECK_EQUAL(obj.counter(), 8u);
    obj.align(16);
    BOOST_CHECK_EQUAL(obj.counter(), 16u);

    // Test valid alignments again. Since we're now aligned, counter() should
    // be constant.
    obj.align(1);
    BOOST_CHECK_EQUAL(obj.counter(), 16u);
    obj.align(2);
    BOOST_CHECK_EQUAL(obj.counter(), 16u);
    obj.align(4);
    BOOST_CHECK_EQUAL(obj.counter(), 16u);
    obj.align(8);
    BOOST_CHECK_EQUAL(obj.counter(), 16u);
    obj.align(16);
    BOOST_CHECK_EQUAL(obj.counter(), 16u);
}

BOOST_AUTO_TEST_CASE(align_with_invalid_values_test)
{
    object<dword_t> obj(16);

    // align 0
    BOOST_CHECK_EQUAL(obj.counter(), 0u);
    BOOST_CHECK_EXCEPTION(obj.align(0), std::invalid_argument,
        check_what_equals("alignment must not be 0"));
    BOOST_CHECK_EQUAL(obj.counter(), 0u);

    // Check alignments that don't fit evenly into the object alignment
    // or are too large.
    const dword_t invalid_alignments[] =
    {
        3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 17, 32, 64
    };
    for (const auto alignment : invalid_alignments)
    {
        BOOST_CHECK_EXCEPTION(obj.align(alignment), std::invalid_argument,
            check_what_equals("alignment does not match object alignment"));
        BOOST_CHECK_EQUAL(obj.counter(), 0u);
    }
}

typedef boost::mpl::list<byte_t, word_t> emitting_past_address_size_test_types;
BOOST_AUTO_TEST_CASE_TEMPLATE(emitting_past_address_size_test, address_type,
    emitting_past_address_size_test_types)
{
    // Create object and fill it up to its maximum size.
    object<address_type> obj(1);
    for (address_type i = 0; i < std::numeric_limits<address_type>::max(); ++i)
    {
        obj.emit8(0);
    }

    // Check whether the next emit throws.
    BOOST_CHECK_EXCEPTION(obj.emit8(0), std::length_error,
        [](const std::length_error& e)
        { return !strcmp(e.what(), "maximum object size reached"); });
}

BOOST_AUTO_TEST_CASE(link_valid_origin_test)
{
    object<dword_t> obj(8);
    const dword_t valid_origins[] = {0, 8, 16, 24, 32};
    for (const auto origin : valid_origins)
    {
        obj.link(origin);
    }
}

BOOST_AUTO_TEST_CASE(link_invalid_origin_test)
{
    object<dword_t> obj(8);
    const dword_t invalid_origins[] =
    {
        1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 17
    };
    for (const auto origin : invalid_origins)
    {
        BOOST_CHECK_EXCEPTION(obj.link(origin), std::invalid_argument,
            check_what_equals("origin does not match object alignment"));
    }
}

BOOST_AUTO_TEST_CASE(link_origin_moves_object_out_of_address_space_test)
{
    // Object is empty, can use whole address space as origin.
    object<byte_t> byte_obj(1);
    byte_obj.link(0);
    byte_obj.link(255);

    // Ensure we cannot move the object out of the address space.
    byte_obj.emit8(0);
    byte_obj.link(0);
    byte_obj.link(254);
    BOOST_CHECK_EXCEPTION(byte_obj.link(255), std::out_of_range,
        check_what_equals("origin too large"));

    // Same for word address size.
    object<word_t> word_obj(1);
    word_obj.link(0);
    word_obj.link(65535);

    word_obj.emit8(0);
    word_obj.link(0);
    word_obj.link(65534);
    BOOST_CHECK_EXCEPTION(word_obj.link(65535), std::out_of_range,
        check_what_equals("origin too large"));
}

BOOST_AUTO_TEST_CASE(fixup_test)
{
    // Test whether all bytes belonging to an address are fixed up and whether
    // no other bytes are overwritten.

    // Byte address size
    object<byte_t> byte_obj(1);
    byte_obj.emit8(23);                 // Guard
    byte_obj.add_reference(detail::reference<byte_t>("label", byte_obj.counter()));
    byte_obj.emit8(0);                  // Fixup location
    byte_obj.emit8(42);                 // Guard
    byte_obj.add_symbol("label");
    // Link and test fixup and guards.
    byte_obj.link(0);
    BOOST_CHECK_EQUAL(byte_obj.to_bytevector()[0], 23);
    BOOST_CHECK_EQUAL(byte_obj.to_bytevector()[1], 3);
    BOOST_CHECK_EQUAL(byte_obj.to_bytevector()[2], 42);
    // Link to different origin, test fixup and guards again.
    byte_obj.link(252);
    BOOST_CHECK_EQUAL(byte_obj.to_bytevector()[0], 23);
    BOOST_CHECK_EQUAL(byte_obj.to_bytevector()[1], 255);
    BOOST_CHECK_EQUAL(byte_obj.to_bytevector()[2], 42);

    // Same for word address size
    object<word_t> word_obj(1);
    word_obj.emit8(23);
    word_obj.add_reference(detail::reference<word_t>("label", word_obj.counter()));
    word_obj.emit16(0);
    word_obj.emit8(42);
    word_obj.add_symbol("label");
    word_obj.link(0);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[0], 23);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[1], 4);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[2], 0);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[3], 42);
    word_obj.link(0x1234 - 4);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[0], 23);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[1], 0x34);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[2], 0x12);
    BOOST_CHECK_EQUAL(word_obj.to_bytevector()[3], 42);

    // Same for dword address size
    object<dword_t> dword_obj(1);
    dword_obj.emit8(23);
    dword_obj.add_reference(detail::reference<dword_t>("label", dword_obj.counter()));
    dword_obj.emit32(0);
    dword_obj.emit8(42);
    dword_obj.add_symbol("label");
    dword_obj.link(0);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[0], 23);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[1], 6);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[2], 0);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[3], 0);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[4], 0);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[5], 42);
    dword_obj.link(0xc01dcafe - 6);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[0], 23);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[1], 0xfe);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[2], 0xca);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[3], 0x1d);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[4], 0xc0);
    BOOST_CHECK_EQUAL(dword_obj.to_bytevector()[5], 42);
}

typedef boost::mpl::list<byte_t, word_t, dword_t> fixup_past_end_of_object_types;
BOOST_AUTO_TEST_CASE_TEMPLATE(fixup_past_end_of_object, address_type,
    fixup_past_end_of_object_types)
{
    // Try with [0..sizeof(address_type)] bytes.
    // Only the last iteration must succeed.
    for (size_t n_bytes_to_emit = 0; n_bytes_to_emit <= sizeof(address_type);
        ++n_bytes_to_emit)
    {
        // Create object and add reference right at the beginning.
        object<address_type> obj(1);
        obj.add_reference(detail::reference<address_type>("end_of_object", obj.counter()));

        // Emit number of bytes for this iteration and add symbol.
        for (size_t j = 0; j < n_bytes_to_emit; ++j)
        {
            obj.emit8(0);
        }
        obj.add_symbol("end_of_object");

        if (n_bytes_to_emit == sizeof(address_type))
        {
            // Enough bytes, no fixup past end of object.
            // Linking should succeed.
            obj.link(0);
        }
        else
        {
            BOOST_CHECK_EXCEPTION(obj.link(0), std::out_of_range,
                check_what_equals("fixup past end of object"));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

}
