/* Copyright (C) Flavio Roth - All Rights Reserved

* Unauthorized copying of this file, via any medium is strictly prohibited.

* This project is proprietary and confidential. It can not be copied
and/or distributed without the express permission of Flavio Roth.

*/

#ifndef MICROMACHINE_TESTSYSTEM_HPP
#define MICROMACHINE_TESTSYSTEM_HPP

#include "cpu.hpp"

#include <array>

/**
 * A system is a cpu + memory
 */
class TestSystem {
public:
	static constexpr size_t MEMORY_SIZE = 0x00008000;
	static constexpr uint32_t INITIAL_SP = 0x00002000;
	static constexpr uint32_t INITIAL_PC = 0x00001000;
	static constexpr uint32_t INITIAL_LR = 0xFFFFFFFF;

private:
	std::vector<uint8_t> _memoryStorage;
	cpu _cpu;

public:
	TestSystem()
		: _memoryStorage(MEMORY_SIZE) {
		// Maps host to virtual memory
		// There's only one segment in the test system
		_cpu.mem().map(_memoryStorage.data(), 0, _memoryStorage.size());

		// zero memory
		std::fill(_memoryStorage.begin(), _memoryStorage.end(), 0);

		// write SP to memory address 0x0
		((uint32_t*)_memoryStorage.data())[0] = INITIAL_SP;

		// initialize the cpu
		_cpu.reset(INITIAL_PC);

	};

	TestSystem(const TestSystem& other)
		: _memoryStorage(other._memoryStorage)
		, _cpu(other._cpu) {
		// Map the existing memory
		_cpu.mem().map(_memoryStorage.data(), 0, _memoryStorage.size());
	}

	const cpu& getCpu() const {
		return _cpu;
	}

	cpu& getCpu() {
		return _cpu;
	}
};




#endif //MICROMACHINE_TESTSYSTEM_HPP