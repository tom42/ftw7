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
#ifndef FTW7_CORE_UNIQUE_HANDLE_HPP_INCLUDED
#define FTW7_CORE_UNIQUE_HANDLE_HPP_INCLUDED

namespace ftw7_core
{

// Unique handle class, based on http://msdn.microsoft.com/en-us/magazine/hh288076.aspx.
template <typename THandle, typename THandleTraits>
class unique_handle
{
public:
    explicit unique_handle(THandle handle = THandleTraits::invalid()) throw()
        : m_handle(handle) {}

    ~unique_handle() throw()
    {
        close();
    }

    THandle get() const throw()
    {
        return m_handle;
    }

    explicit operator bool() const
    {
        return is_valid();
    }

private:
    unique_handle(const unique_handle&) = delete;
    unique_handle& operator = (const unique_handle&) = delete;

    void close() throw()
    {
        if (is_valid())
        {
            THandleTraits::close(m_handle);
        }
    }

    bool is_valid() const throw()
    {
        return m_handle != THandleTraits::invalid();
    }

    THandle m_handle;
};

}

#endif
