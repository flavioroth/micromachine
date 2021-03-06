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


/* MOV - Register Encoding 1
   Encoding: 010001 10 D:1 Rm:4 Rd:3
   NOTE: This encoding doesn't update the APSR flags. */
TEST_F(CpuTestHarness, movRegister_UseLowestRegisterForAllArgs)
{
	code_gen().emit16bit("01000110dmmmmddd", R0, R0);
	step();
}

TEST_F(CpuTestHarness, movRegister_UseHighRegisterForAllArgs)
{
	code_gen().emit16bit("01000110dmmmmddd", registers::LR, registers::LR);
	step();
}

TEST_F(CpuTestHarness, movRegister_MoveHighRegisterToLowRegister)
{
	code_gen().emit16bit("01000110dmmmmddd", R7, R12);
	setExpectedRegisterValue(R7, 0xCCCCCCCC);
	step();
}

TEST_F(CpuTestHarness, movRegister_MoveLowRegisterToLHighRegister)
{
	code_gen().emit16bit("01000110dmmmmddd", R12, R7);
	setExpectedRegisterValue(R12, 0x77777777);
	step();
}

TEST_F(CpuTestHarness, movRegister_MoveOddAddressIntoPCAndMakeSureLSbitIsCleared)
{
	code_gen().emit16bit("01000110dmmmmddd", registers::PC, R1);
	setRegisterValue(R1, INITIAL_PC + 1025);
	setExpectedRegisterValue(registers::PC, INITIAL_PC + 1024);
	step();
}

TEST_F(CpuTestHarness, movRegister_MoveEvenAddressIntoPC)
{
	code_gen().emit16bit("01000110dmmmmddd", registers::PC, R2);
	setRegisterValue(R2, INITIAL_PC + 1024);
	setExpectedRegisterValue(registers::PC, INITIAL_PC + 1024);
	step();
}

TEST_F(CpuTestHarness, movRegister_MovePCintoOtherRegister)
{
	code_gen().emit16bit("01000110dmmmmddd", R3, registers::PC);
	setExpectedRegisterValue(R3, INITIAL_PC + 4);
	step();
}
