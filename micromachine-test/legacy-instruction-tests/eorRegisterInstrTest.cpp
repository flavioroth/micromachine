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


/* EOR - Register
   Encoding: 010000 0001 Rm:3 Rdn:3 */
/* NOTE: APSR_C state is maintained by this instruction. */
TEST_F(CpuTestHarness, eorRegister_UseLowestRegisterForBothArgs)
{
	code_gen().emit16bit("0100000001mmmddd", R0, R0);
	// Use a couple of tests to explicitly set/clear carry to verify both states are maintained.
	setExpectedXPSRflags("nZc");
	clearCarry();
	setExpectedRegisterValue(R0, 0);
	step();
}

TEST_F(CpuTestHarness, eorRegister_UseHighestRegisterForBothArgs)
{
	code_gen().emit16bit("0100000001mmmddd", R7, R7);
	setExpectedXPSRflags("nZC");
	setCarry();
	setExpectedRegisterValue(R7, 0);
	step();
}

TEST_F(CpuTestHarness, eorRegister_XorR3andR7)
{
	code_gen().emit16bit("0100000001mmmddd", R3, R7);
	setExpectedXPSRflags("nzc");
	setExpectedRegisterValue(R7, 0x33333333 ^ 0x77777777);
	clearCarry();
	step();
}

TEST_F(CpuTestHarness, eorRegister_UseXorToJustFlipNegativeSignBitOn)
{
	code_gen().emit16bit("0100000001mmmddd", R6, R3);
	setRegisterValue(R6, 0x80000000);
	setExpectedXPSRflags("NzC");
	setExpectedRegisterValue(R3, 0x33333333 ^ 0x80000000);
	setCarry();
	step();
}
