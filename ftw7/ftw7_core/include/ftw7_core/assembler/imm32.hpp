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
#ifndef FTW7_CORE_ASSEMBLER_IMM32_HPP_INCLUDED
#define FTW7_CORE_ASSEMBLER_IMM32_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include "ftw7_core/assembler/basic_datatypes.hpp"
#include "ftw7_core/assembler/detail/object.hpp"
#include "ftw7_core/assembler/detail/reference.hpp"

namespace ftw7_core
{
namespace assembler
{

class imm32
{
public:
    imm32(dword_t value) : m_is_reference(false), m_value(value) {}

    imm32(const char* label) : m_is_reference(true), m_value(0),
        m_label(label) {}

    imm32(const std::string& label) : m_is_reference(true), m_value(0),
        m_label(label) {}

    template <typename address_type>
    dword_t to_abs32(detail::object<address_type>& obj) const
    {
        return to_absolute_value<dword_t, address_type>(obj);
    }

    template <typename address_type>
    byte_t to_rel8(detail::object<address_type>& obj, address_type base_address) const
    {
        return to_relative_value<byte_t, address_type>(obj, base_address);
    }

    template <typename address_type>
    dword_t to_rel32(detail::object<address_type>& obj, address_type base_address) const
    {
        return to_relative_value<dword_t, address_type>(obj, base_address);
    }

private:
    imm32(const imm32&) = delete;
    imm32& operator = (const imm32&) = delete;

    template <typename target_type, typename address_type>
    target_type to_absolute_value(detail::object<address_type>& obj) const
    {
        if (m_is_reference)
        {
            obj.add_reference(detail::reference<address_type>(m_label, obj.counter()));
            return 0;
        }
        else
        {
            return convert_to_type<target_type>(m_value);
        }
    }

    template <typename target_type, typename address_type>
    target_type to_relative_value(detail::object<address_type>& obj,
        address_type base_address) const
    {
        if (m_is_reference)
        {
            obj.add_reference(
                detail::reference<address_type>::make_rel<target_type>(
                    m_label, obj.counter(), base_address));
            return 0;
        }
        else
        {
            return convert_to_type<target_type>(m_value);
        }
    }

    template <typename target_type>
    static target_type convert_to_type(dword_t value)
    {
        if ( (value < std::numeric_limits<target_type>::min()) ||
            (value > std::numeric_limits<target_type>::max()) )
        {
            throw std::out_of_range("value out of range");
        }
        return target_type(value);
    }

    const bool m_is_reference;
    const dword_t m_value;
    const std::string m_label;
};

}
}

#endif
