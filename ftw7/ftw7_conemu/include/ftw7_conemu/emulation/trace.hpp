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
#ifndef FTW7_CONEMU_EMULATION_TRACE_HPP_INCLUDED
#define FTW7_CONEMU_EMULATION_TRACE_HPP_INCLUDED

#include <sstream>
#include "ftw7_core/log/log.hpp"

namespace ftw7_conemu
{
namespace emulation
{
namespace trace
{
namespace detail
{

inline void print_args(std::wostream&)
{
}

template <typename T, typename... Args>
void print_args(std::wostream& os, T value, Args... args)
{
    // TODO: this is already pretty good. But now we should have a bunch of overloads to print selected arguments, I guess.
    //       Like wchar_t* and stuff. Note it is not good to use the << operator for such special things. Better use something else, really.
    os << L' ' << value;
    print_args(os, args...);
}

}
}

template <typename... Args>
void trace_api_call(const wchar_t* function, Args... args)
{
    // TODO: should check whether logging is enabled at all.
    // TODO: need to get at is_enabled in logging. This sits inside
    //       some detail namespace, but I see no real problem with
    //       moving it into a non-detail namespace, really.
    std::wostringstream os;
    os << function;
    trace::detail::print_args(os, args...);
    FTW7_LOG_TRACE << os.str();
}

}
}

#endif
