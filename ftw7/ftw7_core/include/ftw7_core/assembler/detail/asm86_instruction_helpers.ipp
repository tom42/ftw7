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

template <typename T>
static bool in_range(T x, T lower, T upper)
{
    return (x >= lower) && (x <= upper);
}

template <typename T>
static void check_range(T x, T lower, T upper, const char* who, const char* what)
{
    if (!in_range(x, lower, upper))
    {
        std::string msg;
        msg += who;
        msg += ": ";
        msg += what;
        msg += " is out of range";
        throw std::out_of_range(msg);
    }
}

static byte_t mod_rm(byte_t mod, byte_t reg2, byte_t reg1)
{
    check_range<decltype(mod)>(mod, 0, 3, "mod_rm", "mod");
    check_range<decltype(reg2)>(reg2, 0, 7, "mod_rm", "reg2");
    check_range<decltype(reg1)>(reg1, 0, 7, "mod_rm", "reg1");
    return (mod << 6) | (reg2 << 3) | reg1;
}

void jcc_rel8(byte_t opcode, const imm32& target)
{
    const auto next_instruction_address = obj.counter() + 2;
    obj.emit8(opcode);
    obj.emit8(target.to_rel8(obj, next_instruction_address));
}
