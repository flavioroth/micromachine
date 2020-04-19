/*  Copyright (C) 2014  Adam Green (https://github.com/adamgreen)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "CpuTestHarness.hpp"

// Immediate values used for shift amount in tests.
#define IMM_1  1
#define IMM_32 0


/* LSR - Immediate (Logical Shift Right)
   Encoding: 000 01 imm:5 Rm:3 Rd:3 */
TEST_F(CpuTestHarness, lsrImmediate_registers::R2by1toR0)
{
	code_gen().emit_ins16("00001iiiiimmmddd", IMM_1, registers::R2, registers::R0);
	setExpectedXPSRflags("nzc");
	setExpectedRegisterValue(registers::R0, 0x22222222U >> 1);
	step();
}

TEST_F(CpuTestHarness, lsrImmediate_registers::R7by32toR0_ZeroResult)
{
	code_gen().emit_ins16("00001iiiiimmmddd", IMM_32, registers::R7, registers::R0);
	setExpectedXPSRflags("nZc");
	setExpectedRegisterValue(registers::R0, 0x0);
	step();
}

TEST_F(CpuTestHarness, lsrImmediate_registers::R1by1toR7_CarryOut)
{
	code_gen().emit_ins16("00001iiiiimmmddd", IMM_1, registers::R1, registers::R7);
	setExpectedXPSRflags("nzC");
	setExpectedRegisterValue(registers::R7, 0x11111111U >> 1);
	step();
}

TEST_F(CpuTestHarness, lsrImmediate_registers::R0by32_CarryOutAndIsZero)
{
	code_gen().emit_ins16("00001iiiiimmmddd", IMM_32, registers::R0, registers::R0);
	setExpectedXPSRflags("nZC");
	setRegisterValue(registers::R0, 0x80000000U);
	setExpectedRegisterValue(registers::R0, 0U);
	step();
}

// Can't generate a negative result as smallest shift is 1, meaning at least one 0 is shifted in from left.
