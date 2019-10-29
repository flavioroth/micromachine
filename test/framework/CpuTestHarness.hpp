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

#include <cstdint>
#include <cassert>

#include "cpu.hpp"


// Include C++ headers for test harness.
//#include "CppUTest/TestHarness.h"
#include <gtest/gtest.h>
#include <stdarg.h>

// Initial values for special registers.
#define INITIAL_SP 0x00008000
#define INITIAL_LR 0xFFFFFFFF
#define INITIAL_PC 0x00004000


/* Bits in PinkySimContext::xPSR */
#define APSR_N      (1U << 31U) /* Negative flag */
#define APSR_Z      (1 << 30)   /* Zero flag */
#define APSR_C      (1 << 29)   /* Carry flag */
#define APSR_V      (1 << 28)   /* Overflow flag */
#define IPSR_MASK   0x3F        /* Mask for exception number */
#define EPSR_T      (1 << 24)   /* Thumb mode flag */

/* Useful xPSR flag combinations for masking off at runtime. */
#define APSR_NZCV   (APSR_N | APSR_Z | APSR_C | APSR_V)
#define APSR_NZC    (APSR_N | APSR_Z | APSR_C)
#define APSR_NZ     (APSR_N | APSR_Z)
#define APSR_NC     (APSR_N | APSR_C)
#define APSR_ZC     (APSR_Z | APSR_C)

/* Bits in PinkySimContext::PRIMASK */
#define PRIMASK_PM (1 << 0)

/* Condition codes */
#define COND_EQ 0x0
#define COND_NE (COND_EQ | 1)
#define COND_CS 0x2
#define COND_CC (COND_CS | 1)
#define COND_MI 0x4
#define COND_PL (COND_MI | 1)
#define COND_VS 0x6
#define COND_VC (COND_VS | 1)
#define COND_HI 0x8
#define COND_LS (COND_HI | 1)
#define COND_GE 0xA
#define COND_LT (COND_GE | 1)
#define COND_GT 0xC
#define COND_LE (COND_GT | 1)
#define COND_AL 0xE

/* SYSm field values for MSR and MRS instructions. */
#define SYS_APSR    0
#define SYS_IAPSR   1
#define SYS_EAPSR   2
#define SYS_XPSR    3
#define SYS_IPSR    5
#define SYS_EPSR    6
#define SYS_IEPSR   7
#define SYS_MSP     8
#define SYS_PSP     9
#define SYS_PRIMASK 16
#define SYS_CONTROL 20

/* Register names / indices into PinkySimContext::R array for first 13 registers. */
#define R0  0
#define R1  1
#define R2  2
#define R3  3
#define R4  4
#define R5  5
#define R6  6
#define R7  7
#define R8  8
#define R9  9
#define R10 10
#define R11 11
#define R12 12
#define SP  13
#define LR  14
#define PC  15


/* Values that can be returned from the step() or pinkySimRun() function. */
#define PINKYSIM_STEP_OK            0   /* Executed instruction successfully. */
#define PINKYSIM_STEP_UNDEFINED     1   /* Encountered undefined instruction. */
#define PINKYSIM_STEP_UNPREDICTABLE 2   /* Encountered instruction with unpredictable behaviour. */
#define PINKYSIM_STEP_HARDFAULT     3   /* Encountered instruction which generates hard fault. */
#define PINKYSIM_STEP_BKPT          4   /* Encountered BKPT instruction or other debug event. */
#define PINKYSIM_STEP_UNSUPPORTED   5   /* Encountered instruction not supported by simulator. */
#define PINKYSIM_STEP_SVC           11   /* Encountered SVC instruction. */
#define PINKYSIM_RUN_INTERRUPT      7   /* pinkySimRun() callback signalled interrupt. */
#define PINKYSIM_RUN_WATCHPOINT     8   /* pinkySimRun() callback signalled watchpoint event. */
#define PINKYSIM_RUN_SINGLESTEP     9   /* pinkySimRun() callback signalled single step. */



#define READ_WRITE 0
#define READ_ONLY  1

struct PinkySimContext
{
	void *pMemory;
};

class CpuTestHarness : public ::testing::Test
{
protected:
	const size_t MEMORY_SIZE = 0x8000;
	cpu _cpu;
	std::vector<uint8_t> _memory;

	int m_expectedStepReturn;
	uint32_t m_expectedXPSRflags;
	std::array<uint32_t, 13> m_expectedRegisterValues;
	uint32_t m_expectedSPmain;
	uint32_t m_expectedLR;
	uint32_t m_expectedPC;
	uint32_t m_expectedIPSR;
	uint32_t m_emitAddress;
	uint32_t PRIMASK;
	PinkySimContext m_context;


	using IMemory = void;

	void memory_write_32(uint32_t address, uint32_t value);

	uint32_t memory_read_32(uint32_t address);

	void step();

	void setup();

	void teardown();

	virtual void SetUp();

	void initContext();

	virtual void TearDown();

	void setExpectedStackGrowthSinceBeginning(int growth);

	void setExpectedExceptionTaken(int exceptionNumber);

	void setExpectedStepReturn(int expectedStepReturn);

	void setExpectedSPMain(uint32_t sp);

	void setExpectedXPSRflags(const char *pExpectedFlags);

	void setExpectedIPSR(uint32_t expectedValue);

	void setExpectedRegisterValue(int index, uint32_t expectedValue);

	void setRegisterValue(int index, uint32_t value);

	void emitInstruction16(const char *pEncoding, ...);

	void emitInstruction32(const char *pEncoding1, const char *pEncoding2, ...);

	void emitInstruction16Varg(const char *pEncoding, va_list valist);

	void pinkySimStep();

	void validateSignaledException();

	void validateXPSR();

	void validateRegisters();

	void setCarry();

	void clearCarry();

	void setZero();

	void clearZero();

	void setNegative();

	void clearNegative();

	void setOverflow();

	void clearOverflow();

	void setIPSR(uint32_t ipsr);
};
