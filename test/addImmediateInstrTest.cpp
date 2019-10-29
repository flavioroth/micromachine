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



/* ADD - Immediate - Encoding T1
   Encoding: 000 11 1 0 Imm:3 Rn:3 Rd:3 */
TEST_F(CpuTestHelper, addImmediate_T1UseLowestRegisterOnlyAddLargestImmediate)
{
	emitInstruction16("0001110iiinnnddd", 7, R0, R0);
	setExpectedXPSRflags("nzcv");
	setExpectedRegisterValue(R0, 0U + 7U);
	step();
}

TEST_F(CpuTestHelper, addImmediate_T1UseHigestRegisterOnlyAddSmallestImmediate)
{
	emitInstruction16("0001110iiinnnddd", 0, R7, R7);
	setExpectedXPSRflags("nzcv");
	setExpectedRegisterValue(R7, 0x77777777U + 0U);
	step();
}

TEST_F(CpuTestHelper, addImmediate_T1UseDifferentRegistersForEachArg)
{
	emitInstruction16("0001110iiinnnddd", 3, R7, R0);
	setExpectedXPSRflags("nzcv");
	setExpectedRegisterValue(R0, 0x77777777U + 3U);
	step();
}

TEST_F(CpuTestHelper, addImmediate_T1ForceCarryByAdding1ToLargestInteger)
{
	emitInstruction16("0001110iiinnnddd", 1, R6, R1);
	setExpectedXPSRflags("nZCv");
	setRegisterValue(R6, 0xFFFFFFFFU);
	setExpectedRegisterValue(R1, 0);
	step();
}

TEST_F(CpuTestHelper, addImmediate_T1ForceOverflowPastLargestPositiveInteger)
{
	emitInstruction16("0001110iiinnnddd", 1, R2, R5);
	setExpectedXPSRflags("NzcV");
	setRegisterValue(R2, 0x7FFFFFFFU);
	setExpectedRegisterValue(R5, 0x7FFFFFFFU + 1);
	step();
}



/* ADD - Immediate - Encoding T2
   Encoding: 001 10 Rdn:3 Imm:8 */
TEST_F(CpuTestHelper, addImmediate_T2UseLowestRegisterAndAddLargestImmediate)
{
	emitInstruction16("00110dddiiiiiiii", R0, 255);
	setExpectedXPSRflags("nzcv");
	setExpectedRegisterValue(R0, 0U + 255U);
	step();
}

TEST_F(CpuTestHelper, addImmediate_T2UseHigestRegisterAndAddSmallestImmediate)
{
	emitInstruction16("00110dddiiiiiiii", R7, 0);
	setExpectedXPSRflags("nzcv");
	setExpectedRegisterValue(R7, 0x77777777U + 0U);
	step();
}

TEST_F(CpuTestHelper, addImmediate_T2ForceCarryByAdding1ToLargestInteger)
{
	emitInstruction16("00110dddiiiiiiii", R3, 1);
	setExpectedXPSRflags("nZCv");
	setRegisterValue(R3, 0xFFFFFFFFU);
	setExpectedRegisterValue(R3, 0);
	step();
}

TEST_F(CpuTestHelper, addImmediate_T2ForceOverflowPastLargestPositiveInteger)
{
	emitInstruction16("00110dddiiiiiiii", R3, 1);
	setExpectedXPSRflags("NzcV");
	setRegisterValue(R3, 0x7FFFFFFFU);
	setExpectedRegisterValue(R3, 0x7FFFFFFFU + 1);
	step();
}
