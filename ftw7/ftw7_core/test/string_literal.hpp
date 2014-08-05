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
#ifndef FTW7_CORE_TEST_STRING_LITERAL_HPP_INCLUDED
#define FTW7_CORE_TEST_STRING_LITERAL_HPP_INCLUDED

// Depending on T, create a narrow or wide string literal from x.
// T should be one of char or wchar_t.
#define FTW7_STRING_LITERAL(T, x) ftw7_string_literal<T>::make_literal(x, L##x)

template <typename T>
struct ftw7_string_literal
{
    static const char* make_literal(const char* narrow, const wchar_t* /*wide*/) { return narrow; }
};

template <>
struct ftw7_string_literal<wchar_t>
{
    static const wchar_t* make_literal(const char* /*narrow*/, const wchar_t* wide) { return wide; }
};


#endif
