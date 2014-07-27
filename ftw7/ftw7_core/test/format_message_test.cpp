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
#include "ftw7_core/format_message.hpp"
#include "wstring_ostream.hpp"

namespace
{

using namespace ftw7_core;

BOOST_AUTO_TEST_SUITE(format_message_test)

BOOST_AUTO_TEST_CASE(format_message_from_system_test)
{
    BOOST_CHECK_EQUAL("The operation completed successfully.",
        format_message_from_system(ERROR_SUCCESS));
    BOOST_CHECK_EQUAL("The handle is invalid.",
        format_message_from_system(ERROR_INVALID_HANDLE));
    BOOST_CHECK_EQUAL("unknown error 4294967295: FormatMessage failed with error code 317",
        format_message_from_system(0xffffffff));
}

BOOST_AUTO_TEST_CASE(wformat_message_from_system_test)
{
    BOOST_CHECK_EQUAL(L"The operation completed successfully.",
        wformat_message_from_system(ERROR_SUCCESS));
    BOOST_CHECK_EQUAL(L"The handle is invalid.",
        wformat_message_from_system(ERROR_INVALID_HANDLE));
    BOOST_CHECK_EQUAL(L"unknown error 4294967295: FormatMessage failed with error code 317",
        wformat_message_from_system(0xffffffff));

    // With message prefix
    BOOST_CHECK_EQUAL(L"Info: The operation completed successfully.",
        wformat_message_from_system(L"Info", ERROR_SUCCESS));
    BOOST_CHECK_EQUAL(L"Info: The operation completed successfully.",
        wformat_message_from_system(std::wstring(L"Info"), ERROR_SUCCESS));
}

BOOST_AUTO_TEST_SUITE_END()

}
