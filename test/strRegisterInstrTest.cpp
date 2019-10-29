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

TEST_GROUP_BASE(strRegister, pinkySimBase)
{
    void setup()
    {
        pinkySimBase::setup();
    }

    void teardown()
    {
        pinkySimBase::teardown();
    }
};


/* STR - Register
   Encoding: 0101 000 Rm:3 Rn:3 Rt:3 */
PINKY_TEST(strRegister, UseAMixOfRegisters)
{
    emitInstruction16("0101000mmmnnnttt", R7, R3, R0);
    setRegisterValue(R3, INITIAL_PC);
    setRegisterValue(R7, 4);
    SimpleMemory_SetMemory(m_context.pMemory, INITIAL_PC + 4, 0xBAADFEED, READ_WRITE);
    pinkySimStep(&m_context);
    CHECK_EQUAL(0x00000000, IMemory_Read32(m_context.pMemory, INITIAL_PC + 4));
}

PINKY_TEST(strRegister, UseAnotherMixOfRegisters)
{
    emitInstruction16("0101000mmmnnnttt", R1, R0, R7);
    setRegisterValue(R0, INITIAL_PC);
    setRegisterValue(R1, 4);
    SimpleMemory_SetMemory(m_context.pMemory, INITIAL_PC + 4, 0xBAADFEED, READ_WRITE);
    pinkySimStep(&m_context);
    CHECK_EQUAL(0x77777777, IMemory_Read32(m_context.pMemory, INITIAL_PC + 4));
}

PINKY_TEST(strRegister, YetAnotherMixOfRegisters)
{
    emitInstruction16("0101000mmmnnnttt", R0, R7, R4);
    setRegisterValue(R7, INITIAL_PC);
    setRegisterValue(R0, 4);
    SimpleMemory_SetMemory(m_context.pMemory, INITIAL_PC + 4, 0xBAADFEED, READ_WRITE);
    pinkySimStep(&m_context);
    CHECK_EQUAL(0x44444444, IMemory_Read32(m_context.pMemory, INITIAL_PC + 4));
}

PINKY_TEST(strRegister, AttemptUnalignedStore)
{
    emitInstruction16("0101000mmmnnnttt", R7, R3, R0);
    setRegisterValue(R3, INITIAL_PC);
    setRegisterValue(R7, 2);
	setExpectedExceptionTaken(PINKYSIM_STEP_HARDFAULT);
    pinkySimStep(&m_context);
}

PINKY_TEST(strRegister, AttemptStoreToInvalidAddress)
{
    emitInstruction16("0101000mmmnnnttt", R7, R3, R0);
    setRegisterValue(R3, 0xFFFFFFFC);
    setRegisterValue(R7, 0);
	setExpectedExceptionTaken(PINKYSIM_STEP_HARDFAULT);
    pinkySimStep(&m_context);
}
