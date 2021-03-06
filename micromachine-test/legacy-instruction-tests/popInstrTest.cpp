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


/* POP
   Encoding: 1011 1 10 P:1 RegisterList:8 */
TEST_F(CpuTestHarness, pop_JustPopPC)
{
	code_gen().emit16bit("1011110Prrrrrrrr", 1, 0);
	setRegisterValue(registers::SP, INITIAL_SP - 4);
	setExpectedRegisterValue(registers::SP, INITIAL_SP);
	setExpectedRegisterValue(registers::PC, INITIAL_PC + 16);
	memory_write_32(INITIAL_SP - 4, (INITIAL_PC + 16) | 1);
	step();
}

TEST_F(CpuTestHarness, pop_JustPopR0)
{
	code_gen().emit16bit("1011110Prrrrrrrr", 0, 1);
	setRegisterValue(registers::SP, INITIAL_SP - 4);
	setExpectedRegisterValue(registers::SP, INITIAL_SP);
	setExpectedRegisterValue(R0, 0xFFFFFFFF);
	memory_write_32(INITIAL_SP - 4, 0xFFFFFFFF);
	step();
}

TEST_F(CpuTestHarness, pop_JustPopR7)
{
	code_gen().emit16bit("1011110Prrrrrrrr", 0, (1 << 7));
	setRegisterValue(registers::SP, INITIAL_SP - 4);
	setExpectedRegisterValue(registers::SP, INITIAL_SP);
	setExpectedRegisterValue(R7, 0xFFFFFFFF);
	memory_write_32(INITIAL_SP - 4, 0xFFFFFFFF);
	step();
}

TEST_F(CpuTestHarness, pop_PopAll)
{
	code_gen().emit16bit("1011110Prrrrrrrr", 1, 0xFF);
	setRegisterValue(registers::SP, INITIAL_SP - 4 * 9);
	setExpectedRegisterValue(registers::SP, INITIAL_SP);
	setExpectedRegisterValue(R0, 9);
	setExpectedRegisterValue(R1, 8);
	setExpectedRegisterValue(R2, 7);
	setExpectedRegisterValue(R3, 6);
	setExpectedRegisterValue(R4, 5);
	setExpectedRegisterValue(R5, 4);
	setExpectedRegisterValue(R6, 3);
	setExpectedRegisterValue(R7, 2);
	setExpectedRegisterValue(registers::PC, 1 & ~1);
	for (int i = 1; i <= 9; i++)
		memory_write_32(INITIAL_SP - 4 * i, i);
	step();
}

TEST_F(CpuTestHarness, pop_PopToSetPCToEvenAddressWhichGeneratesHardFault)
{
	code_gen().emit16bit("1011110Prrrrrrrr", 1, 0);
	setExpectedXPSRflags("t");
	setRegisterValue(registers::SP, INITIAL_SP - 4);
	setExpectedRegisterValue(registers::SP, INITIAL_SP);
	setExpectedRegisterValue(registers::PC, INITIAL_PC + 16);
	memory_write_32(INITIAL_SP - 4, INITIAL_PC + 16);
	step();

	const uint16_t NOP = 0xBF00;
	memory_write_32(INITIAL_PC + 16, NOP);
	setExpectedExceptionTaken(CPU_STEP_HARDFAULT);
	step();
}
/*
TEST_F(CpuTestHarness, pop_HardFaultFromInvalidMemoryRead)
{
    code_gen().emit_ins16("1011110Prrrrrrrr", 0, 1);
    setRegisterValue(registers::SP, 0xFFFFFFFC);
    setExpectedExceptionHandled(CPU_STEP_HARDFAULT);
    step(&m_context);
}*/
/*
TEST_SIM_ONLY(pop, UnpredictableToPopNoRegisters)
{
    code_gen().emit_ins16("1011110Prrrrrrrr", 0, 0);
    setExpectedStepReturn(CPU_STEP_UNPREDICTABLE);
    step(&m_context);
}
*/
