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


/* ADD SP Plus Immediate - Encoding T1
   Encoding: 1010 1 Rd:3 Imm:8 */
TEST_F(CpuTestHarness, addSP_T1UseHighestRegisterAddSmallestImmediate)
{
	code_gen().emit16bit("10101dddiiiiiiii", R7, 0);
	setExpectedRegisterValue(R7, INITIAL_SP + 0);
	step();
}

TEST_F(CpuTestHarness, addSP_T1UseLowestRegisterAddLargestImmediate)
{
	code_gen().emit16bit("10101dddiiiiiiii", R0, 255);
	setExpectedRegisterValue(R0, INITIAL_SP + 255 * 4);
	step();
}

TEST_F(CpuTestHarness, addSP_T1UseIntermediateValues)
{
	code_gen().emit16bit("10101dddiiiiiiii", R3, 128);
	setExpectedRegisterValue(R3, INITIAL_SP + 128 * 4);
	step();
}



/* ADD SP Plus Immediate - Encoding T2
   Encoding: 1011 0000 0 Imm:7 */
TEST_F(CpuTestHarness, addSP_T2SmallestImmediate)
{
	code_gen().emit16bit("101100000iiiiiii", 0);
	setRegisterValue(registers::SP, INITIAL_PC + 1024);
	setExpectedRegisterValue(registers::SP, INITIAL_PC + 1024 + 0);
	step();
}

TEST_F(CpuTestHarness, addSP_T2LargestImmediate)
{
	code_gen().emit16bit("101100000iiiiiii", 127);
	setRegisterValue(registers::SP, INITIAL_PC + 1024);
	setExpectedRegisterValue(registers::SP, INITIAL_PC + 1024 + 127 * 4);
	step();
}

TEST_F(CpuTestHarness, addSP_T2IntermediateValues)
{
	code_gen().emit16bit("101100000iiiiiii", 64);
	setRegisterValue(registers::SP, INITIAL_PC + 1024);
	setExpectedRegisterValue(registers::SP, INITIAL_PC + 1024 + 64 * 4);
	step();
}
