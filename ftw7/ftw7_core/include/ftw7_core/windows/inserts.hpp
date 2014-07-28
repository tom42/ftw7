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
#ifndef FTW7_CORE_WINDOWS_INSERTS_HPP_INCLUDED
#define FTW7_CORE_WINDOWS_INSERTS_HPP_INCLUDED

#include <iterator>
#include <string>
#include <vector>

namespace ftw7_core
{
namespace windows
{

namespace detail
{
namespace inserts
{

template <typename T>
class formatter
{
public:
    typedef std::basic_string<T> string;
    typedef std::vector<const T*> argument_vector;

    string format(const string& format_string,
        const argument_vector& arguments)
    {
        string result;
        auto i = format_string.begin();
        auto o = std::back_inserter(result);
        while (i != format_string.end())
        {
            if ( !( (*i == '%') &&
                handle_insert(i, format_string.end(), o, arguments) ) )
            {
                // Not an insert, copy character to output.
                *o++ = *i++;
            }
        }
        return result;
    }

private:
    template <typename input_iterator, typename output_iterator>
    bool handle_insert(input_iterator& input, input_iterator end,
        output_iterator output, const argument_vector& arguments)
    {
        auto old_input = input;

        // Consume initial % sign and attempt to handle the escape sequence.
        ++input;
        bool result = handle_insert2(input, end, output, arguments);
        if (!result)
        {
            // Unrecognized escape sequence, restore input pointer and process
            // as normal input.
            input = old_input;
        }

        return result;
    }

    template <typename input_iterator, typename output_iterator>
    bool handle_insert2(input_iterator& input, input_iterator end,
        output_iterator output, const argument_vector& arguments)
    {
        if (input == end)
        {
            // Lone % sign at end of input.
            return false;
        }

        switch (*input)
        {
        case '%':
            // %% => %
            ++input;
            *output++ = '%';
            return true;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            {
                const argument_vector::size_type index = *input - '0' - 1;
                if ( (index < 0) || (index >= arguments.size()) )
                {
                    // No argument available for this insert.
                    return false;
                }
                for (const T* p = arguments.at(index); *p; ++p)
                {
                    *output++ = *p;
                }
                ++input;
            }
            return true;

        default:
            // Unrecognized escape sequence.
            return false;
        }
    }
};

}
}

template <typename T>
std::basic_string<T> replace_inserts(const std::basic_string<T>& format_string,
    const std::basic_string<T>& a1)
{
    std::vector<const T*> arguments;
    arguments.reserve(1);
    arguments.push_back(a1.c_str());
    return detail::inserts::formatter<T>().format(format_string, arguments);
}

template <typename T>
std::basic_string<T> replace_inserts(const std::basic_string<T>& format_string,
    const std::basic_string<T>& a1, const std::basic_string<T>& a2)
{
    std::vector<const T*> arguments;
    arguments.reserve(2);
    arguments.push_back(a1.c_str());
    arguments.push_back(a2.c_str());
    return detail::inserts::formatter<T>().format(format_string, arguments);
}

}
}

#endif
