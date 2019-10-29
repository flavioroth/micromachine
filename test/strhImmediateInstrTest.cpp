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


/* STRH - Immediate
   Encoding: 1000 0 Imm:5 Rn:3 Rt:3 */
TEST_F(CpuTestHelper, strhImmediate_UseAMixOfRegistersWordAlignedWithSmallestOffset)
{
	emitInstruction16("10000iiiiinnnttt", 0, R7, R0);
	setRegisterValue(R7, INITIAL_PC + 4);
	memory_write_32(INITIAL_PC + 4, 0xBAADFEED);
	step();
	EXPECT_EQ(0xBAAD0000, memory_read_32(INITIAL_PC + 4));
}

TEST_F(CpuTestHelper, strhImmediate_AnotherMixOfRegistersNotWordAligned)
{
	emitInstruction16("10000iiiiinnnttt", 1, R0, R7);
	setRegisterValue(R0, INITIAL_PC + 4);
	memory_write_32(INITIAL_PC + 4, 0xBAADFEED);
	step();
	EXPECT_EQ(0x7777FEED, memory_read_32(INITIAL_PC + 4));
}

TEST_F(CpuTestHelper, strhImmediate_LargestOffset)
{
	emitInstruction16("10000iiiiinnnttt", 31, R1, R6);
	setRegisterValue(R1, INITIAL_PC);
	memory_write_32(INITIAL_PC + 60, 0xBAADFEED);
	step();
	EXPECT_EQ(0x6666FEED, memory_read_32(INITIAL_PC + 60));
}

TEST_F(CpuTestHelper, strhImmediate_AttemptStoreToInvalidAddress)
{
	emitInstruction16("10000iiiiinnnttt", 0, R3, R1);
	setRegisterValue(R3, 0xFFFFFFFC);
	setExpectedExceptionTaken(PINKYSIM_STEP_HARDFAULT);
	step();
}
