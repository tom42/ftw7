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

asm86& align(address_type alignment_in_bytes)
{
    obj.align(alignment_in_bytes);
    return *this;
}

asm86& label(const std::string& name)
{
    obj.add_symbol(name);
    return *this;
}

asm86& data(const void* data, size_t size)
{
    const unsigned char* p = static_cast<const unsigned char*>(data);
    for (size_t i = 0; i < size; ++i)
    {
        obj.emit8(*p);
        ++p;
    }
    return *this;
}

asm86& string_z(const char* s)
{
    for (; *s; ++s)
    {
        obj.emit8(*s);
    }
    obj.emit8(0);
    return *this;
}

asm86& string_z(const std::string& s)
{
    string_z(s.c_str());
    return *this;
}

asm86& wstring_z(const wchar_t* s)
{
    for (; *s; ++s)
    {
        obj.emit16(*s);
    }
    obj.emit16(0);
    return *this;
}

asm86& wstring_z(const std::wstring& s)
{
    wstring_z(s.c_str());
    return *this;
}
