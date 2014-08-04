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
#include <boost/test/unit_test.hpp>
#include "ftw7_core/pointer.hpp"

namespace
{

using ftw7_core::is_function_pointer;

BOOST_AUTO_TEST_SUITE(pointer_test)

BOOST_AUTO_TEST_CASE(is_function_pointer_test)
{
    BOOST_CHECK(!is_function_pointer<int>::value);
    BOOST_CHECK(!is_function_pointer<bool*>::value);
    BOOST_CHECK(!is_function_pointer<decltype(::qsort)>::value);

    typedef void(*some_function_pointer)();
    BOOST_CHECK(is_function_pointer<some_function_pointer>::value);
}

BOOST_AUTO_TEST_SUITE_END()

}
