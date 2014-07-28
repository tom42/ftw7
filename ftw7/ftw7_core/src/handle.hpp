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
#ifndef FTW7_CORE_HANDLE_HPP_INCLUDED
#define FTW7_CORE_HANDLE_HPP_INCLUDED

#include <Windows.h>
#include <string>

namespace ftw7_core
{

// This is a utility for the process class to manage its process and thread
// handle. This is *not* a generic Windows handle wrapper. For a generic
// solution, see http://msdn.microsoft.com/en-us/magazine/hh288076.aspx.
class handle
{
public:
    handle(const std::string& handle_description)
        : handle_description(handle_description), h(0) {}

    ~handle()
    {
        close();
    }

    void close()
    {
        if (h)
        {
            CloseHandle(h);
            h = 0;
        }
    }

    HANDLE get()
    {
        if (!h)
        {
            throw std::logic_error("handle::get: no " + handle_description + " handle");
        }
        return h;
    }

    void set(HANDLE new_handle)
    {
        close();
        h = new_handle;
    }

    bool is_available() const
    {
        return !!h;
    }
private:
    handle(const handle&) = delete;
    handle& operator = (const handle&) = delete;

    const std::string handle_description;
    HANDLE h;
};

}

#endif
