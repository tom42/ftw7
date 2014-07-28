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
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include "ftw7_core/windows/inserts.hpp"
#include "wstring_ostream.hpp"

namespace
{

template <typename char_type>
class string_creator
{
public:
    std::basic_string<char_type> create(const char* s)
    {
        return std::basic_string<char_type>(s);
    }
};

template <>
class string_creator<wchar_t>
{
public:
    std::basic_string<wchar_t> create(const char* s)
    {
        // Use a wide stream to widen the string.
        std::wostringstream result;
        result << s;
        return result.str();
    }
};

template <typename T>
class test
{
public:
    void do_test()
    {
        BOOST_CHECK_EQUAL(replace_inserts("%1", "a"), s("a"));
        BOOST_CHECK_EQUAL(replace_inserts("%1%2", "a", "b"), s("ab"));
        BOOST_CHECK_EQUAL(replace_inserts("%1%2%1", "a", "b"), s("aba"));
        BOOST_CHECK_EQUAL(replace_inserts("%1", ""), s(""));
        BOOST_CHECK_EQUAL(replace_inserts("%1.", "He who foos last foos best"),
            s("He who foos last foos best."));

        // Excess insert arguments are ignored.
        BOOST_CHECK_EQUAL(replace_inserts("", "b"), s(""));
        BOOST_CHECK_EQUAL(replace_inserts("a", "b"), s("a"));
        BOOST_CHECK_EQUAL(replace_inserts("%1", "a", "b"), s("a"));
        BOOST_CHECK_EQUAL(replace_inserts("%2", "a", "b"), s("b"));

        // Inserts for which no arguments are provided are ignored.
        BOOST_CHECK_EQUAL(replace_inserts("%1%2", "a"), s("a%2"));
        BOOST_CHECK_EQUAL(replace_inserts("%1%2%3", "a", "b"), s("ab%3"));

        // Percent sign.
        BOOST_CHECK_EQUAL(replace_inserts("%%", "a"), s("%"));
        BOOST_CHECK_EQUAL(replace_inserts("%%%%", "a"), s("%%"));

        // Invalid escape sequences are ignored.
        BOOST_CHECK_EQUAL(replace_inserts("%0", "a"), s("%0"));
        BOOST_CHECK_EQUAL(replace_inserts("%x%yz", "a"), s("%x%yz"));

        // Lone percent sign at end of input is invalid and ignored.
        BOOST_CHECK_EQUAL(replace_inserts("%", "b"), s("%"));
        BOOST_CHECK_EQUAL(replace_inserts("a%", "b"), s("a%"));
    }

private:
    std::basic_string<T> replace_inserts(const char* format,
        const char* a1)
    {
        return ftw7_core::windows::replace_inserts(s(format), s(a1));
    }

    std::basic_string<T> replace_inserts(const char* format,
        const char* a1, const char* a2)
    {
        return ftw7_core::windows::replace_inserts(s(format), s(a1), s(a2));
    }

    std::basic_string<T> s(const char* s)
    {
        return sc.create(s);
    }

    string_creator<T> sc;
};

typedef boost::mpl::list<char, wchar_t> test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(inserts_test, T, test_types)
{
    test<T>().do_test();
}

}
