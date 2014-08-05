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
#include "ftw7_core/windows/windows_error.hpp"

namespace ftw7_core
{
namespace windows
{

template <typename TWndClass, typename TChar>
class basic_unique_window_class
{
public:
    typedef TChar char_type;
    typedef std::basic_string<char_type> string_type;
    typedef TWndClass wndclass_type;

    explicit basic_unique_window_class(const wndclass_type& wc)
        : m_classname(wc.lpszClassName),
        m_hinstance(wc.hInstance)
    {
        // register_class will throw if RegisterClass fails, leaving us without
        // a basic_unique_window_class, which is exactly what we want.
        register_class(wc);
    }

    ~basic_unique_window_class()
    {
        unregister_class();
    }

    const string_type& classname() const
    {
        return m_classname;
    }

    HINSTANCE hinstance() const
    {
        return m_hinstance;
    }

private:
    basic_unique_window_class(const basic_unique_window_class&) = delete;
    basic_unique_window_class& operator = (const basic_unique_window_class&) = delete;

    void unregister_class()
    {
        do_unregister_class(m_classname, m_hinstance);
    }

    static void register_class(const wndclass_type& wc)
    {
        const auto atom = do_register_class(&wc);
        if (!atom)
        {
            const auto error = GetLastError();
            throw ftw7_core::windows::windows_error(L"could not register window class", error);
        }
    }

    static ATOM do_register_class(const WNDCLASSEXA* wc)
    {
        return RegisterClassExA(wc);
    }

    static ATOM do_register_class(const WNDCLASSEXW* wc)
    {
        return RegisterClassExW(wc);
    }

    static BOOL do_unregister_class(const std::string& classname, HINSTANCE hinstance)
    {
        return UnregisterClassA(classname.c_str(), hinstance);
    }

    static BOOL do_unregister_class(const std::wstring& classname, HINSTANCE hinstance)
    {
        return UnregisterClassW(classname.c_str(), hinstance);
    }

    const string_type m_classname;
    const HINSTANCE m_hinstance;
};

typedef basic_unique_window_class<WNDCLASSEXA, char> unique_window_classexa;
typedef basic_unique_window_class<WNDCLASSEXW, wchar_t> unique_window_classexw;

}
}

#endif
