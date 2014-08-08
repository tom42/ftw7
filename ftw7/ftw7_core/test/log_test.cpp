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
#include "ftw7_core/log/log.hpp"

using namespace ftw7_core;

namespace
{

BOOST_AUTO_TEST_SUITE(log_test)

BOOST_AUTO_TEST_CASE(smoke_test)
{
    log::initialize(log::log_level::trace);
    FTW7_LOG_INFO << "Info Message";
    FTW7_LOG_DEBUG << L"Debug Message";
    FTW7_TRACE_API_CALL(1, 'a', L'b', "x", "y");
}

BOOST_AUTO_TEST_SUITE_END()

}
