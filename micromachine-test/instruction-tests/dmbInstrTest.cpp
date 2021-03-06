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

/* DMB
   Encoding: 11110 0 111 01 1 (1)(1)(1)(1)
			 10 (0) 0 (1)(1)(1)(1) 0101 option:4 */
MICROMACHINE_TEST_F(dmb, OptionSetTo15, CpuTestFixture) {

	emitInstruction32("1111001110111111", "100011110101oooo", 15);
	StepAndExpectThat(instruction32Executed());
}

MICROMACHINE_TEST_F(dmb, OptionSetTo0, CpuTestFixture) {

	emitInstruction32("1111001110111111", "100011110101oooo", 0);
	StepAndExpectThat(instruction32Executed());
}
