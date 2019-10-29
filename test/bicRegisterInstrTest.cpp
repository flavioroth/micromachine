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

#include "framework/pinkySimBaseTest.hpp"


/* BIC - Register
   Encoding: 010000 1110 Rm:3 Rdn:3 */
/* NOTE: APSR_C state is maintained by this instruction. */
TEST_F(CpuTestHelper, bicRegister_UseLowestRegisterForBothArgs)
{
	emitInstruction16("0100001110mmmddd", R0, R0);
	// Use a couple of tests to explicitly set/clear carry to verify both states are maintained.
	setExpectedXPSRflags("nZc");
	clearCarry();
	setExpectedRegisterValue(R0, 0);
	step();
}

TEST_F(CpuTestHelper, bicRegister_UseHighestRegisterForBothArgs)
{
	emitInstruction16("0100001110mmmddd", R7, R7);
	setExpectedXPSRflags("nZC");
	setCarry();
	setExpectedRegisterValue(R7, 0);
	step();
}

TEST_F(CpuTestHelper, bicRegister_UseR3andR7)
{
	emitInstruction16("0100001110mmmddd", R3, R7);
	setExpectedXPSRflags("nz");
	setExpectedRegisterValue(R7, 0x77777777 & ~0x33333333);
	step();
}

TEST_F(CpuTestHelper, bicRegister_UseBicToClearLSbit)
{
	emitInstruction16("0100001110mmmddd", R6, R1);
	setRegisterValue(R1, -1);
	setRegisterValue(R6, 1);
	setExpectedXPSRflags("Nz");
	setExpectedRegisterValue(R1, -1U & ~1);
	step();
}
