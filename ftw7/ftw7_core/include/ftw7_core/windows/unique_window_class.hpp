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
#ifndef FTW7_CORE_WINDOWS_UNIQUE_WINDOW_CLASS_HPP_INCLUDED
#define FTW7_CORE_WINDOWS_UNIQUE_WINDOW_CLASS_HPP_INCLUDED

#include <Windows.h>
#include <string>

namespace ftw7_core
{
namespace windows
{

template <typename TWndClass, typename TChar>
class basic_unique_window_class
{
public:
    typedef std::basic_string<TChar> string_type;
    typedef TWndClass wndclass_type;

    basic_unique_window_class(const wndclass_type& wc)
        : m_classname(wc.lpszClassName),
        m_hinstance(nullptr) // TODO: initialize correctly
    {
        // TODO: no use for a default ctor here!
        // TODO: tuck away the class name
        // TODO: tuck away the hinstance
        // TODO: check wc.lpszClassName and wc.hInstance first? Then again, why bother. This is C++, after all.
    }

    const string_type& classname() const
    {
        return m_classname;
    }

private:
    basic_unique_window_class(const basic_unique_window_class&) = delete;
    basic_unique_window_class& operator = (const basic_unique_window_class&) = delete;

    string_type m_classname;
    HINSTANCE m_hinstance;
};

typedef basic_unique_window_class<WNDCLASSA, char> unique_window_classa;
typedef basic_unique_window_class<WNDCLASSEXA, char> unique_window_classexa;
typedef basic_unique_window_class<WNDCLASSW, wchar_t> unique_window_classw;
typedef basic_unique_window_class<WNDCLASSEXW, wchar_t> unique_window_classexw;

}
}

#endif
