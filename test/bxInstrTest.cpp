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

#include "framework/CpuTestHarness.hpp"


/* BX (Branch and Exchange)
   Encoding: 010001 11 0 Rm:4 (0)(0)(0) */
TEST_F(CpuTestHarness,
	   bx_UseLowestRegisterToBranchToEvenAddressWhichClearsThumbModeToCauseHardFaultOnNextInstruction)
{
	emitInstruction16("010001110mmmm000", R0);
	setExpectedXPSRflags("t");
	setRegisterValue(R0, INITIAL_PC + 16);
	setExpectedRegisterValue(PC, INITIAL_PC + 16);
	step();

	const uint16_t NOP = 0xBF00;
	memory_write_32(INITIAL_PC + 16, NOP);
	setExpectedExceptionTaken(PINKYSIM_STEP_HARDFAULT);
	step();
}

TEST_F(CpuTestHarness, bx_UseHighestRegisterToBranchToOddAddressWhichIsRequiredForThumb)
{
	emitInstruction16("010001110mmmm000", LR);
	setRegisterValue(LR, (INITIAL_PC + 16) | 1);
	setExpectedRegisterValue(PC, INITIAL_PC + 16);
	step();
}
/*
TEST_SIM_ONLY(bx, UnpredictableToUseR15)
{
    emitInstruction16("010001110mmmm000", PC);
    setExpectedStepReturn(PINKYSIM_STEP_UNPREDICTABLE);
    setExpectedRegisterValue(PC, INITIAL_PC);
    pinkySimStep(&m_context);
}

TEST_SIM_ONLY(bx, UnpredictableForBit0ToBeHigh)
{
    emitInstruction16("010001110mmmm001", R0);
    setExpectedStepReturn(PINKYSIM_STEP_UNPREDICTABLE);
    setExpectedRegisterValue(PC, INITIAL_PC);
    pinkySimStep(&m_context);
}

TEST_SIM_ONLY(bx, UnpredictableForBit1ToBeHigh)
{
    emitInstruction16("010001110mmmm010", R0);
    setExpectedStepReturn(PINKYSIM_STEP_UNPREDICTABLE);
    setExpectedRegisterValue(PC, INITIAL_PC);
    pinkySimStep(&m_context);
}

TEST_SIM_ONLY(bx, UnpredictableForBit2ToBeHigh)
{
    emitInstruction16("010001110mmmm100", R0);
    setExpectedStepReturn(PINKYSIM_STEP_UNPREDICTABLE);
    setExpectedRegisterValue(PC, INITIAL_PC);
    step(&m_context);
}
*/