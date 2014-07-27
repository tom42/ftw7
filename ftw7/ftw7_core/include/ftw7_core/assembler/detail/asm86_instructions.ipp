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

asm86& call(const gpreg32& reg32)
{
    obj.emit8(0xff, 0xd0 + reg32.number());
    return *this;
}

asm86& cmp(const gpreg32& dst, const imm32& src)
{
    obj.emit8(0x81);
    obj.emit8(mod_rm(3, 7, dst.number()));
    obj.emit32(src.to_abs32(obj));
    return *this;
}

asm86& int3()
{
    obj.emit8(0xcc);
    return *this;
}

asm86& jz(const imm32& target)
{
    jcc_rel8(0x74, target);
    return *this;
}

asm86& je(const imm32& target)
{
    jz(target);
    return *this;
}

asm86& jnz(const imm32& target)
{
    jcc_rel8(0x75, target);
    return *this;
}

asm86& jne(const imm32& target)
{
    jnz(target);
    return *this;
}

asm86& jmp(const imm32& target)
{
    const auto next_instruction_address = obj.counter() + 5;
    obj.emit8(0xe9);
    obj.emit32(target.to_rel32(obj, next_instruction_address));
    return *this;
}

asm86& mov(const gpreg32& dst, const gpreg32& src)
{
    obj.emit8(0x8b, mod_rm(3, dst.number(), src.number()));
    return *this;
}

asm86& mov(const gpreg32& dst, const imm32& src)
{
    obj.emit8(0xb8 + dst.number());
    obj.emit32(src.to_abs32(obj));
    return *this;
}

asm86& nop()
{
    obj.emit8(0x90);
    return *this;
}

asm86& or(const gpreg32& dst, const gpreg32& src)
{
    obj.emit8(0x0b, mod_rm(3, dst.number(), src.number()));
    return *this;
}

asm86& pop(const gpreg32& reg32)
{
    obj.emit8(0x58 + reg32.number());
    return *this;
}

asm86& popa()
{
    obj.emit8(0x61);
    return *this;
}

asm86& popf()
{
    obj.emit8(0x9d);
    return *this;
}

asm86& push(const gpreg32& reg32)
{
    obj.emit8(0x50 + reg32.number());
    return *this;
}

asm86& push(const imm32& imm32)
{
    obj.emit8(0x68);
    obj.emit32(imm32.to_abs32(obj));
    return *this;
}

asm86& pusha()
{
    obj.emit8(0x60);
    return *this;
}

asm86& pushf()
{
    obj.emit8(0x9c);
    return *this;
}

asm86& ret()
{
    obj.emit8(0xc3);
    return *this;
}

asm86& xor(const gpreg32& dst, const gpreg32& src)
{
    obj.emit8(0x33, mod_rm(3, dst.number(), src.number()));
    return *this;
}
