/*  Copyright (C) 2014  Adam Green (https://github.com/adamgreen): Original implementation
    Copyright (C) 2019  Flavio Roth (https://github.com/flavioroth): Adaptation

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


/* MVN - Register (MOve Negative)
   Encoding: 010000 1111 Rm:3 Rd:3 */
/* NOTE: APSR_C state is maintained by this instruction. */
TEST_F(CpuTestHarness, mvnRegister_UseLowestRegisterForAllArgs)
{
	// Use a couple of tests to explicitly set/clear carry to verify both states are maintained.
	code_gen().emit16bit("0100001111mmmddd", R0, R0);
	setExpectedXPSRflags("NzC");
	setCarry();
	setExpectedRegisterValue(R0, ~0U);
	step();
}

TEST_F(CpuTestHarness, mvnRegister_UseHigestRegisterForAllArgs)
{
	code_gen().emit16bit("0100001111mmmddd", R7, R7);
	setExpectedXPSRflags("Nzc");
	clearCarry();
	setExpectedRegisterValue(R7, ~0x77777777U);
	step();
}

TEST_F(CpuTestHarness, mvnRegister_UseDifferentRegistersForEachArg)
{
	code_gen().emit16bit("0100001111mmmddd", R2, R1);
	setExpectedXPSRflags("Nz");
	setExpectedRegisterValue(R1, ~0x22222222U);
	step();
}

TEST_F(CpuTestHarness, mvnRegister_MoveANegationOfNegativeOne_ClearsNegativeFlagAndSetsZeroFlag)
{
	code_gen().emit16bit("0100001111mmmddd", R2, R1);
	setRegisterValue(R2, -1);
	setExpectedXPSRflags("nZ");
	setExpectedRegisterValue(R1, 0U);
	step();
}
