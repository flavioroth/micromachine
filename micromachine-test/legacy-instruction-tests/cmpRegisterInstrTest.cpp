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


/* CMP - Register - Encoding T1
   Encoding: 010000 1010 Rm:3 Rn:3 */
TEST_F(CpuTestHarness, cmpRegister_T1UseLowestRegisterForAllArgs)
{
	code_gen().emit16bit("0100001010mmmnnn", R0, R0);
	setExpectedXPSRflags("nZCv");
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T1UseHigestRegisterForAllArgs)
{
	code_gen().emit16bit("0100001010mmmnnn", R7, R7);
	setExpectedXPSRflags("nZCv");
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T1RnLargerThanRm)
{
	code_gen().emit16bit("0100001010mmmnnn", R1, R2);
	setExpectedXPSRflags("nzCv");
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T1RnSmallerThanRm)
{
	code_gen().emit16bit("0100001010mmmnnn", R1, R0);
	setExpectedXPSRflags("Nzcv");
	setRegisterValue(R1, 1);
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T1ForceNegativeOverflow)
{
	code_gen().emit16bit("0100001010mmmnnn", R1, R2);
	setExpectedXPSRflags("nzCV");
	setRegisterValue(R2, 0x80000000U);
	setRegisterValue(R1, 1U);
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T1ForcePositiveOverflow)
{
	code_gen().emit16bit("0100001010mmmnnn", R1, R2);
	setExpectedXPSRflags("NzcV");
	setRegisterValue(R2, 0x7FFFFFFFU);
	setRegisterValue(R1, -1U);
	step();
}



/* CMP - Register - Encoding T2
   Encoding: 010001 01 N:1 Rm:4 Rn:3
   NOTE: At least one register must be high register, R8 - R14. */
TEST_F(CpuTestHarness, cmpRegister_T2CompareLowestRegisterToHighestRegister)
{
	code_gen().emit16bit("01000101nmmmmnnn", R0, registers::LR);
	setRegisterValue(registers::LR, 0xEEEEEEEE);
	setExpectedXPSRflags("nzcv");
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T2CompareHighestRegisterToLowestRegister)
{
	code_gen().emit16bit("01000101nmmmmnnn", registers::LR, R0);
	setRegisterValue(registers::LR, 0xEEEEEEEE);
	setExpectedXPSRflags("NzCv");
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T2CompareR8ToItself)
{
	code_gen().emit16bit("01000101nmmmmnnn", R8, R8);
	setExpectedXPSRflags("nZCv");
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T2ForceNegativeOverflow)
{
	code_gen().emit16bit("01000101nmmmmnnn", R11, R12);
	setExpectedXPSRflags("nzCV");
	setRegisterValue(R11, 0x80000000U);
	setRegisterValue(R12, 1U);
	step();
}

TEST_F(CpuTestHarness, cmpRegister_T2ForcePositiveOverflow)
{
	code_gen().emit16bit("01000101nmmmmnnn", R11, R12);
	setExpectedXPSRflags("NzcV");
	setRegisterValue(R11, 0x7FFFFFFFU);
	setRegisterValue(R12, -1U);
	step();
}
/*
TEST_SIM_ONLY(cmpRegister, T2UnpredictableForBothArgsToBeLowRegisters)
{
    code_gen().emit_ins16("01000101nmmmmnnn", R6, R7);
    setExpectedStepReturn(CPU_STEP_UNPREDICTABLE);
    setExpectedRegisterValue(registers::PC, INITIAL_PC);
    pinkySimStep(&m_context);
}

TEST_SIM_ONLY(cmpRegister, T2UnpredictableForRnToBeR15)
{
    code_gen().emit_ins16("01000101nmmmmnnn", registers::PC, R8);
    setExpectedStepReturn(CPU_STEP_UNPREDICTABLE);
    setExpectedRegisterValue(registers::PC, INITIAL_PC);
    pinkySimStep(&m_context);
}

TEST_SIM_ONLY(cmpRegister, T2UnpredictableForRmToBeR15)
{
    code_gen().emit_ins16("01000101nmmmmnnn", R8, registers::PC);
    setExpectedStepReturn(CPU_STEP_UNPREDICTABLE);
    setExpectedRegisterValue(registers::PC, INITIAL_PC);
    step(&m_context);
}
*/
