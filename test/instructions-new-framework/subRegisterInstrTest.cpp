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


/* SUB - Register
   Encoding: 000 11 0 1 Rm:3 Rn:3 Rd:3 */
TEST_F(CpuTestHarness, subRegister_UseLowestRegisterForAllArgs)
{
	code_gen().emit_ins16("0001101mmmnnnddd", registers::R0, registers::R0, registers::R0);
	setExpectedXPSRflags("nZCv");
	setExpectedRegisterValue(registers::R0, 0);
	step();
}

TEST_F(CpuTestHarness, subRegister_UseHigestRegisterForAllArgs)
{
	code_gen().emit_ins16("0001101mmmnnnddd", registers::R7, registers::R7, registers::R7);
	setExpectedXPSRflags("nZCv");
	setExpectedRegisterValue(registers::R7, 0);
	step();
}

TEST_F(CpuTestHarness, subRegister_UseDifferentRegistersForEachArg)
{
	code_gen().emit_ins16("0001101mmmnnnddd", registers::R1, registers::R2, registers::R0);
	setExpectedXPSRflags("nzCv");
	setExpectedRegisterValue(registers::R0, 0x22222222U - 0x11111111U);
	step();
}

// Force APSR flags to be set which haven't already been covered above.
TEST_F(CpuTestHarness, subRegister_ForceCarryClearToIndicateBorrowAndResultWillBeNegative)
{
	code_gen().emit_ins16("0001101mmmnnnddd", registers::R1, registers::R0, registers::R2);
	setExpectedXPSRflags("Nzcv");
	setRegisterValue(registers::R1, 1);
	setExpectedRegisterValue(registers::R2, 0U - 1U);
	step();
}

TEST_F(CpuTestHarness, subRegister_ForceNegativeOverflow)
{
	code_gen().emit_ins16("0001101mmmnnnddd", registers::R1, registers::R2, registers::R0);
	setExpectedXPSRflags("nzCV");
	setRegisterValue(registers::R2, 0x80000000U);
	setRegisterValue(registers::R1, 1U);
	setExpectedRegisterValue(registers::R0, 0x7FFFFFFF);
	step();
}

TEST_F(CpuTestHarness, subRegister_ForcePositiveOverflow)
{
	code_gen().emit_ins16("0001101mmmnnnddd", registers::R1, registers::R2, registers::R0);
	setExpectedXPSRflags("NzcV");
	setRegisterValue(registers::R2, 0x7FFFFFFFU);
	setRegisterValue(registers::R1, -1U);
	setExpectedRegisterValue(registers::R0, 0x7FFFFFFFU + 1U);
	step();
}