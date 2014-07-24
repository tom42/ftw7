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
#ifndef FTW7_CORE_WEXCEPT_HPP_INCLUDED
#define FTW7_CORE_WEXCEPT_HPP_INCLUDED

#include <stdexcept>
#include <string>

namespace ftw7_core
{

class wruntime_error : public std::runtime_error
{
public:
    wruntime_error(const std::wstring& message)
        : std::runtime_error("wruntime_error"), m_message(message) {}
    wruntime_error(const wchar_t* message)
        : std::runtime_error("wruntime_error"), m_message(message) {}

    virtual ~wruntime_error() {}

    virtual const wchar_t* wwhat() const
    {
        return m_message.c_str();
    }

private:
    std::wstring m_message;
};

}

#endif
