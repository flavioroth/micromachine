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

// Immediate values used for shift amount in tests.
#define IMM_0  0
#define IMM_1  1
#define IMM_3  3
#define IMM_4  4
#define IMM_31 31


/* LSL - Immediate (Logical Shift Left)
   Encoding: 000 00 imm:5 Rm:3 Rd:3 */
TEST_F(pinkySimBase, lslImmediate_MovR7toR0_CarryUnmodified)
{
    emitInstruction16("00000iiiiimmmddd", IMM_0, R7, R0);
    setExpectedXPSRflags("nzc");
    clearCarry();
    setExpectedRegisterValue(R0, 0x77777777U);
    pinkySimStep(&m_context);
}

TEST_F(pinkySimBase, lslImmediate_MovR0toR7_ZeroResultAndCarryUnmodified)
{
    emitInstruction16("00000iiiiimmmddd", IMM_0, R0, R7);
    setExpectedXPSRflags("nZC");
    setCarry();
    setExpectedRegisterValue(R7, 0x0);
    pinkySimStep(&m_context);
}

TEST_F(pinkySimBase, lslImmediate_ShiftR1by3_ResultInNegativeValue)
{
    emitInstruction16("00000iiiiimmmddd", IMM_3, R1, R0);
    setExpectedXPSRflags("Nzc");
    setExpectedRegisterValue(R0, 0x11111111U << 3);
    pinkySimStep(&m_context);
}

TEST_F(pinkySimBase, lslImmediate_ShiftR1by4_HasCarryOut)
{
    emitInstruction16("00000iiiiimmmddd", IMM_4, R1, R0);
    setExpectedXPSRflags("nzC");
    setExpectedRegisterValue(R0, 0x11111111U << 4);
    pinkySimStep(&m_context);
}

TEST_F(pinkySimBase, lslImmediate_ShiftR0by31_PushesLowestbitIntoSignBit)
{
    emitInstruction16("00000iiiiimmmddd", IMM_31, R0, R0);
    setExpectedXPSRflags("Nzc");
    setRegisterValue(R0, 1U);
    setExpectedRegisterValue(R0, 1U << 31);
    pinkySimStep(&m_context);
}

TEST_F(pinkySimBase, lslImmediate_CarryOutFromHighestBit)
{
    emitInstruction16("00000iiiiimmmddd", IMM_1, R0, R0);
    setExpectedXPSRflags("nzC");
    setRegisterValue(R0, 0xA0000000U);
    setExpectedRegisterValue(R0, 0xA0000000U << 1);
    pinkySimStep(&m_context);
}

TEST_F(pinkySimBase, lslImmediate_CarryOutFromLowestBit)
{
    emitInstruction16("00000iiiiimmmddd", IMM_31, R0, R0);
    setExpectedXPSRflags("nZC");
    setRegisterValue(R0, 0x2U);
    setExpectedRegisterValue(R0, 0x2U << 31);
    pinkySimStep(&m_context);
}
