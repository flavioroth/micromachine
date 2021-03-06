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


/* MOV - Immediate
   Encoding: 001 00 Rd:3 Imm:8 */
/* NOTE: APSR_C state is maintained by this instruction. */
TEST_F(CpuTestHarness, movImmediate_MovToR0)
{
	code_gen().emit16bit("00100dddiiiiiiii", R0, 127);
	// Use a couple of tests to explicitly set/clear carry to verify both states are maintained.
	setExpectedXPSRflags("nzc");
	clearCarry();
	setExpectedRegisterValue(R0, 127);
	step();
}

TEST_F(CpuTestHarness, movImmediate_MovToR7)
{
	code_gen().emit16bit("00100dddiiiiiiii", R7, 127);
	setExpectedXPSRflags("nzC");
	setCarry();
	setExpectedRegisterValue(R7, 127);
	step();
}

TEST_F(CpuTestHarness, movImmediate_MovSmallestImmediateValueToR3)
{
	code_gen().emit16bit("00100dddiiiiiiii", R3, 0);
	setExpectedXPSRflags("nZ");
	setExpectedRegisterValue(R3, 0);
	step();
}

TEST_F(CpuTestHarness, movImmediate_MovLargestImmediateValueToR3)
{
	code_gen().emit16bit("00100dddiiiiiiii", R3, 255);
	setExpectedXPSRflags("nz");
	setExpectedRegisterValue(R3, 255);
	step();
}
