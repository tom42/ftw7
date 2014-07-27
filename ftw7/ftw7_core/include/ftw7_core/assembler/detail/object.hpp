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
#ifndef FTW7_CORE_ASSEMBLER_DETAIL_OBJECT_HPP_INCLUDED
#define FTW7_CORE_ASSEMBLER_DETAIL_OBJECT_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <boost/noncopyable.hpp>
#include "ftw7_core/assembler/basic_datatypes.hpp"
#include "ftw7_core/assembler/detail/reference.hpp"
#include "ftw7_core/assembler/detail/symbol_table.hpp"

namespace ftw7_core
{
namespace assembler
{
namespace detail
{

template <typename address_type>
class object : boost::noncopyable
{
    static_assert(std::is_unsigned<address_type>::value,
        "template argument address_type must be an unsigned integer type");
    static_assert(std::numeric_limits<byte_t>::digits == 8,
        "byte_t has wrong number of bits");
    static_assert(std::numeric_limits<word_t>::digits == 16,
        "word_t has wrong number of bits");
    static_assert(std::numeric_limits<dword_t>::digits == 32,
        "dword_t has wrong bumber of bits");

public:
    typedef reference<address_type> reference_type;

    object(address_type alignment) : object_alignment(alignment)
    {
        check_alignment_is_not_zero(alignment);
    }

    void add_reference(const detail::reference<address_type>& reference)
    {
        references.push_back(reference);
    }

    void add_symbol(const std::string& symbol_name)
    {
        symbols.add(symbol_name, counter());
    }

    void link(address_type origin)
    {
        check_origin(origin);
        for (auto ref = references.begin(); ref != references.end(); ++ref)
        {
            fixup_address(*ref, origin);
        }
    }

    bytevector to_bytevector() const
    {
        return data;
    }

    address_type counter() const
    {
        return static_cast<address_type>(data.size());
    }

    void align(address_type alignment)
    {
        check_alignment(alignment);
        while (counter() % alignment != 0)
        {
            emit8(0);
        }
    }

    void emit8(byte_t b)
    {
        if (counter() >= std::numeric_limits<address_type>::max())
        {
            throw std::length_error("maximum object size reached");
        }
        data.push_back(b);
    }

    void emit8(byte_t b0, byte_t b1)
    {
        emit8(b0);
        emit8(b1);
    }

    void emit16(word_t w)
    {
        emit8(w & 0xff);
        emit8((w >> 8) & 0xff);
    }

    void emit32(dword_t d)
    {
        emit16(d & 0xffff);
        emit16((d >> 16) & 0xffff);
    }

private:
    static void check_alignment_is_not_zero(address_type alignment)
    {
        if (alignment == 0)
        {
            throw std::invalid_argument("alignment must not be 0");
        }
    }

    void check_alignment(address_type alignment)
    {
        check_alignment_is_not_zero(alignment);
        if (object_alignment % alignment != 0)
        {
            throw std::invalid_argument(
                "alignment does not match object alignment");
        }
    }

    void check_origin(address_type origin)
    {
        if (origin % object_alignment != 0)
        {
            throw std::invalid_argument(
                "origin does not match object alignment");
        }

        if (std::numeric_limits<address_type>::max() - counter() < origin)
        {
            // This origin is too large and would move the object out of its
            // address space.
            throw std::out_of_range("origin too large");
        }
    }

    address_type get_relocated_address(const std::string& symbol_name,
        address_type origin) const
    {
        const address_type address = symbols.get(symbol_name);
        if (std::numeric_limits<address_type>::max() - origin < address)
        {
            throw std::out_of_range("overflow when relocating symbol");
        }
        return address + origin;
    }

    address_type get_displacement(const reference<address_type>& ref,
        address_type origin) const
    {
        const address_type start = ref.base_address() + origin;
        const address_type end = get_relocated_address(ref.label(), origin);
        const address_type displacement = end - start;

        // Check we're in range. Doing everything unsigned here.
        if (end < start)
        {
            // Negative displacement, e.g. 0x80..0xff, or 0x8000..0xffff
            const address_type ndisp = start - end;
            if (!((ndisp >= 0) && (ndisp <= ref.fixup_msb_mask())))
            {
                throw std::out_of_range("displacement out of range");
            }
        }
        else
        {
            // Positive displacement, e.g. 0x00..0x7f, or 0x0000..0x7fff
            if (!((displacement >= 0) && (displacement <= (ref.fixup_msb_mask() - 1))))
            {
                throw std::out_of_range("displacement out of range");
            }
        }

        return displacement;
    }

    address_type get_fixup_value(const reference<address_type>& ref,
        address_type origin) const
    {
        if (ref.is_relative())
        {
            return get_displacement(ref, origin);
        }
        else
        {
            return get_relocated_address(ref.label(), origin);
        }
    }

    void fixup_address(const reference<address_type>& ref, address_type origin)
    {
        const unsigned int n_byte_bits = std::numeric_limits<byte_t>::digits;
        const unsigned int mask = (1 << n_byte_bits) - 1;

        // Get fixup value
        address_type fixup_value = get_fixup_value(ref, origin);

        // Calculate last address of fixup, catch overflows while doing so.
        if (std::numeric_limits<address_type>::max() - (ref.fixup_size_bytes() - 1) < ref.fixup_location())
        {
            throw std::out_of_range("fixup past end of maximum object size");
        }
        const address_type fixup_end = ref.fixup_location() + (ref.fixup_size_bytes() - 1);

        if ( (ref.fixup_location() >= counter()) || (fixup_end >= counter()) )
        {
            throw std::out_of_range("fixup past end of object");
        }

        auto fixup_location = ref.fixup_location();
        for (address_type i = 0; i < ref.fixup_size_bytes(); ++i)
        {
            data[fixup_location] = fixup_value & mask;
            ++fixup_location;
            fixup_value >>= n_byte_bits;
        }
    }

    const address_type object_alignment;
    detail::symbol_table<address_type> symbols;
    std::vector<reference_type> references;
    bytevector data;
};

}
}
}

#endif
