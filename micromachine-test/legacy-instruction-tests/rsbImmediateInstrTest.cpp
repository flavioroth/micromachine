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


/* RSB - Immediate
   Encoding: 010000 1001 Rn:3 Rd:3 */
TEST_F(CpuTestHarness, rsbImmediate_UseLowestRegisterOnly)
{
	code_gen().emit16bit("0100001001nnnddd", R0, R0);
	setExpectedXPSRflags("nZCv");
	setExpectedRegisterValue(R0, 0U);
	step();
}

TEST_F(CpuTestHarness, rsbImmediate_UseHigestRegisterOnly)
{
	code_gen().emit16bit("0100001001nnnddd", R7, R7);
	setExpectedXPSRflags("Nzcv");
	setExpectedRegisterValue(R7, -0x77777777U);
	step();
}

TEST_F(CpuTestHarness, rsbImmediate_UseDifferentRegistersForEachArg)
{
	code_gen().emit16bit("0100001001nnnddd", R2, R0);
	setExpectedXPSRflags("Nzcv");
	setExpectedRegisterValue(R0, -0x22222222);
	step();
}

TEST_F(CpuTestHarness, rsbImmediate_ForceOverflowByNegatingLargestNegativeValue)
{
	code_gen().emit16bit("0100001001nnnddd", R0, R7);
	setExpectedXPSRflags("NzcV");
	setRegisterValue(R0, 0x80000000);
	setExpectedRegisterValue(R7, 0x80000000U);
	step();
}
