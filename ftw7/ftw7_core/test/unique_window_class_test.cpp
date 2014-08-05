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
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <sstream>
#include "ftw7_core/windows/unique_window_class.hpp"
#include "wstring_ostream.hpp"

namespace
{

using namespace ftw7_core::windows;
typedef boost::mpl::list<unique_window_classa, unique_window_classexa, unique_window_classw, unique_window_classexw> unique_window_class_types;

// Create std::string or std::wstring from char*, based on T.
template <typename T>
std::basic_string<T> make_string(const char *s)
{
    std::basic_ostringstream<T> stream;
    stream << s;
    return stream.str();
}

BOOST_AUTO_TEST_SUITE(unique_window_class_test)

BOOST_AUTO_TEST_CASE_TEMPLATE(construction_test, unique_window_class_type, unique_window_class_types)
{
    unique_window_class_type::wndclass_type wc;
    memset(&wc, 0, sizeof(wc));

    const auto classname = make_string<unique_window_class_type::string_type::value_type>("x");
    // wc.cbSize = sizeof(wc); // TODO: WNDCLASSA doesn't have this...currently this is OK, since we don't really registerclass(ex) yet
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = nullptr;   // TODO: might have to supply a real wndproc
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = classname.c_str();

    unique_window_class_type uwc(wc);
    BOOST_CHECK_EQUAL(uwc.classname(), classname);
}

BOOST_AUTO_TEST_SUITE_END()

}
