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

template <typename T>
struct string_literal
{
    static const char* make_literal(const char* narrow, const wchar_t* /*wide*/) { return narrow; }
};

template <>
struct string_literal<wchar_t>
{
    static const wchar_t* make_literal(const char* /*narrow*/, const wchar_t* wide) { return wide; }
};

#define STRING_LITERAL(T, x) string_literal<T>::make_literal(x, L##x)

// Set the cbSize member of a WNDCLASS structure.
// Or don't, in the case of the specializations for WNDCLASSA and WNDCLASSW,
// because these don't have a cbSize member.
template <typename T>
struct set_size
{
    static void set(T& wc) { wc.cbSize = sizeof(wc); }
};

template <>
struct set_size<WNDCLASSA>
{
    static void set(WNDCLASSA& /*wc*/) {}
};

template <>
struct set_size<WNDCLASSW>
{
    static void set(WNDCLASSW& /*wc*/) {}
};

BOOST_AUTO_TEST_SUITE(unique_window_class_test)

BOOST_AUTO_TEST_CASE_TEMPLATE(construction_test, unique_window_class_type, unique_window_class_types)
{
    unique_window_class_type::wndclass_type wc;
    memset(&wc, 0, sizeof(wc));
    set_size<unique_window_class_type::wndclass_type>::set(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = nullptr;   // TODO: might have to supply a real wndproc
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = STRING_LITERAL(unique_window_class_type::char_type, "test window class");

    unique_window_class_type uwc(wc);
    BOOST_CHECK_EQUAL(uwc.classname(), STRING_LITERAL(unique_window_class_type::char_type, "test window class"));
    BOOST_CHECK_EQUAL(uwc.hinstance(), GetModuleHandle(nullptr));
}

BOOST_AUTO_TEST_SUITE_END()

}
