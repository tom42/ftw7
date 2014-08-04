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
#ifndef FTW7_CORE_TEST_CHECK_WHAT_HPP_INCLUDED
#define FTW7_CORE_TEST_CHECK_WHAT_HPP_INCLUDED

#include <stdexcept>
#include <string>
#include <boost/algorithm/string/predicate.hpp>
#include "ftw7_core/wexcept.hpp"

class check_what_equals
{
public:
    check_what_equals(const std::string& expected_message)
        : expected_message(expected_message) {}

    bool operator () (const std::exception& e) const
    {
        return !strcmp(expected_message.c_str(), e.what());
    }
private:
    std::string expected_message;
};

class check_wwhat_equals
{
public:
    check_wwhat_equals(const std::wstring& expected_message)
        : expected_message(expected_message) {}

    bool operator () (const ftw7_core::wruntime_error& e) const
    {
        return !wcscmp(expected_message.c_str(), e.wwhat());
    }
private:
    std::wstring expected_message;
};

class check_wwhat_starts_with
{
public:
    check_wwhat_starts_with(const std::wstring& prefix)
        : prefix(prefix) {}

    bool operator () (const ftw7_core::wruntime_error& e) const
    {
        return boost::algorithm::starts_with(e.wwhat(), prefix);
    }
private:
    std::wstring prefix;
};

#endif
