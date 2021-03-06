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

#include "CpuTestFixture.hpp"
using namespace micromachine::testing;

/* SXTH (Sign ExTend Halfword)
   Encoding: 1011 0010 00 Rm:3 Rd:3 */
MICROMACHINE_TEST_F(sxth, SignExtendLowestRegisterIntoHighestRegister_PositiveValue, CpuTestFixture) {
	emitInstruction16("1011001000mmmddd", registers::R7, registers::R0);
	setReg(registers::R7, 0x7FFF);
	StepAndExpectThatInstruction16IsExecutedAndThat(reg(registers::R0).equals(0x7FFF));
}

MICROMACHINE_TEST_F(sxth, SignExtendHighestRegisterIntoLowestRegister_NegativeValue, CpuTestFixture) {
	emitInstruction16("1011001000mmmddd", registers::R0, registers::R7),
	setReg(registers::R0, 0x8000);
	StepAndExpectThatInstruction16IsExecutedAndThat(reg(registers::R7).equals(0xFFFF8000));
}

MICROMACHINE_TEST_F(sxth, OverwriteUpperBits_PositiveValue, CpuTestFixture) {
	emitInstruction16("1011001000mmmddd", registers::R6, registers::R1),
	setReg(registers::R6, 0xF00D7FFF);
	StepAndExpectThatInstruction16IsExecutedAndThat(reg(registers::R1).equals(0x00007FFF));
}

MICROMACHINE_TEST_F(sxth, OverwriteUpperBits_NegativeValue, CpuTestFixture) {
	emitInstruction16("1011001000mmmddd", registers::R2, registers::R5), setReg(registers::R2, 0xF00D8000);
	StepAndExpectThatInstruction16IsExecutedAndThat(reg(registers::R5).equals(0xFFFF8000));
}
