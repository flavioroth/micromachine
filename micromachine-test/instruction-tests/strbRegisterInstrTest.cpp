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

/* STRB - Register
   Encoding: 0101 010 Rm:3 Rn:3 Rt:3 */
MICROMACHINE_TEST_F(strbRegister, UseAMixOfRegistersWordAligned, CpuTestFixture) {

	emitInstruction16("0101010mmmnnnttt", registers::R7, registers::R3, registers::R0);
	setReg(registers::R3, TestMachine::INITIAL_PC);
	setReg(registers::R7, 4);
	memWrite32(TestMachine::INITIAL_PC + 4, 0xBAADFEED);

	StepAndExpectThatInstruction16IsExecutedAndThat(memoryAt(TestMachine::INITIAL_PC + 4).equals32(0xBAADFE00));
}

MICROMACHINE_TEST_F(strbRegister, UseAnotherMixOfRegistersSecondByteInWord, CpuTestFixture) {

	emitInstruction16("0101010mmmnnnttt", registers::R1, registers::R0, registers::R7);

	setReg(registers::R0, TestMachine::INITIAL_PC);
	setReg(registers::R1, 5);
	memWrite32(TestMachine::INITIAL_PC + 4, 0xBAADFEED);

	StepAndExpectThatInstruction16IsExecutedAndThat(memoryAt(TestMachine::INITIAL_PC + 4).equals32(0xBAAD77ED));
}

MICROMACHINE_TEST_F(strbRegister, YetAnotherMixOfRegistersThirdByteInWord, CpuTestFixture) {

	emitInstruction16("0101010mmmnnnttt", registers::R0, registers::R7, registers::R4);

	setReg(registers::R7, TestMachine::INITIAL_PC);
	setReg(registers::R0, 6);
	memWrite32(TestMachine::INITIAL_PC + 4, 0xBAADFEED);

	StepAndExpectThatInstruction16IsExecutedAndThat(memoryAt(TestMachine::INITIAL_PC + 4).equals32(0xBA44FEED));
}

MICROMACHINE_TEST_F(strbRegister, YetAnotherMixOfRegistersFourthByteInWord, CpuTestFixture) {

	emitInstruction16("0101010mmmnnnttt", registers::R0, registers::R7, registers::R5);

	setReg(registers::R7, TestMachine::INITIAL_PC);
	setReg(registers::R0, 7);
	memWrite32(TestMachine::INITIAL_PC + 4, 0xBAADFEED);

	StepAndExpectThatInstruction16IsExecutedAndThat(memoryAt(TestMachine::INITIAL_PC + 4).equals32(0x55ADFEED));
}

MICROMACHINE_TEST_F(strbRegister, AttemptStoreToInvalidAddress, CpuTestFixture) {

	emitInstruction16("0101010mmmnnnttt", registers::R7, registers::R3, registers::R0);
	setReg(registers::R3, 0xFFFFFFFC);
	setReg(registers::R7, 0);
	StepAndExpectThat(hardfaultHandlerReached());
}
