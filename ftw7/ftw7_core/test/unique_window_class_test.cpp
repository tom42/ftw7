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
#include "string_literal.hpp"
#include "wstring_ostream.hpp"

namespace
{

using namespace ftw7_core::windows;
typedef boost::mpl::list<unique_window_classexa, unique_window_classexw> unique_window_class_types;

template <typename TWndClassType, typename TChar>
TWndClassType create_windowclass_structure(const TChar* classname)
{
    TWndClassType wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = nullptr;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = classname;
    return wc;
}

bool is_windowclass_registered(const char* classname)
{
    WNDCLASSEXA wc;
    return GetClassInfoExA(GetModuleHandle(nullptr), classname, &wc) ? true : false;
}

bool is_windowclass_registered(const wchar_t* classname)
{
    WNDCLASSEXW wc;
    return GetClassInfoExW(GetModuleHandle(nullptr), classname, &wc) ? true : false;
}

BOOST_AUTO_TEST_SUITE(unique_window_class_test)

BOOST_AUTO_TEST_CASE_TEMPLATE(construction_test, unique_window_class_type, unique_window_class_types)
{
    auto classname = FTW7_STRING_LITERAL(unique_window_class_type::char_type, "test window class");
    auto wc = create_windowclass_structure<unique_window_class_type::wndclass_type, unique_window_class_type::char_type>(classname);

    BOOST_CHECK(!is_windowclass_registered(classname));
    {
        unique_window_class_type uwc(wc);
        BOOST_CHECK_EQUAL(uwc.classname(), classname);
        BOOST_CHECK_EQUAL(uwc.hinstance(), GetModuleHandle(nullptr));
        BOOST_CHECK(is_windowclass_registered(classname));
    }
    BOOST_CHECK(!is_windowclass_registered(classname));
}

BOOST_AUTO_TEST_SUITE_END()

}
