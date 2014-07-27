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
#ifndef FTW7_CORE_ASSEMBLER_DETAIL_SYMBOL_TABLE_HPP_INCLUDED
#define FTW7_CORE_ASSEMBLER_DETAIL_SYMBOL_TABLE_HPP_INCLUDED

#include <map>
#include <sstream>
#include <string>

namespace ftw7_core
{
namespace assembler
{
namespace detail
{

template <typename address_type>
class symbol_table
{
public:
    void add(const std::string& label, address_type address)
    {
        const auto sym = table.find(label);
        if (sym == table.end())
        {
            table.insert(std::make_pair(label, address));
        }
        else
        {
            // Label is already defined.
            // This is OK only if it is redefined with the same address.
            if (sym->second != address)
            {
                std::ostringstream message;
                message << "label '" << label << "' is already defined";
                throw std::logic_error(message.str());
            }
        }
    }

    address_type get(const std::string& label) const
    {
        auto sym = table.find(label);
        if (sym == table.end())
        {
            std::ostringstream message;
            message << "label '" << label << "' is undefined";
            throw std::logic_error(message.str());
        }
        return sym->second;
    }

private:
    std::map<std::string, address_type> table;
};

}
}
}

#endif
