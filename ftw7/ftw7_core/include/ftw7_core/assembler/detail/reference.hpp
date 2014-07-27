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
#ifndef FTW7_CORE_ASSEMBLER_DETAIL_REFERENCE_HPP_INCLUDED
#define FTW7_CORE_ASSEMBLER_DETAIL_REFERENCE_HPP_INCLUDED

#include <limits>
#include <string>
#include "ftw7_core/assembler/basic_datatypes.hpp"

namespace ftw7_core
{
namespace assembler
{
namespace detail
{

template <typename address_type>
class reference
{
public:
    // Creates an absolute  reference of size address_type, at fixup_location.
    reference(const std::string& label, address_type fixup_location)
        : m_label(label),
        m_fixup_location(fixup_location),
        m_fixup_size_bytes(n_bits_per_address / n_bits_per_byte),
        m_base_address(0),
        m_is_relative(false)
    {}

    template <typename reference_field_type>
    static reference<address_type> make_rel(const std::string& label,
        address_type fixup_location, address_type base_address)
    {
        return reference(label, fixup_location, sizeof(reference_field_type),
            base_address, true);
    }

    // The referenced label.
    const std::string& label() const
    {
        return m_label;
    }

    // The location of the fixup inside the object.
    address_type fixup_location() const
    {
        return m_fixup_location;
    }

    // The size of the fixup in bytes.
    address_type fixup_size_bytes() const
    {
        return m_fixup_size_bytes;
    }

    // Relative references only:
    // The reference's base address relative to the object's origin.
    // This is not necessarily the same address as the fixup location.
    address_type base_address() const
    {
        return m_base_address;
    }

    bool is_relative() const
    {
        return m_is_relative;
    }

    // Returns the mask for the fixup's most significant bit,
    // e.g. 0x80 for a byte sized fixup or 0x8000 for a word sized one.
    address_type fixup_msb_mask() const
    {
        return 1u << (m_fixup_size_bytes * n_bits_per_byte - 1u);
    }

private:
    reference<address_type>& operator = (const reference<address_type>&);

    reference(const std::string& label, address_type fixup_location,
        address_type fixup_size_bytes, address_type base_address,
        bool is_relative)
        : m_label(label),
        m_fixup_location(fixup_location),
        m_fixup_size_bytes(fixup_size_bytes),
        m_base_address(base_address),
        m_is_relative(is_relative)
    {}

    static const unsigned int n_bits_per_address = std::numeric_limits<address_type>::digits;
    static const unsigned int n_bits_per_byte = std::numeric_limits<byte_t>::digits;

    const std::string m_label;
    const address_type m_fixup_location;
    const address_type m_fixup_size_bytes;
    const address_type m_base_address;
    const bool m_is_relative;
};

}
}
}

#endif
