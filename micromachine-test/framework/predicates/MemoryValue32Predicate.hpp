//
// Created by fla on 25.04.20.
//

#pragma once

#include <string>
#include <types/types.hpp>

#include "MemoryValuePredicate.hpp"
#include "ValueFormaters.hpp"

namespace micromachine::testing {

class MemoryValue32Predicate : public MemoryValuePredicate {
protected:
	const uint32_t _expectedValue;

public:
	MemoryValue32Predicate(uint32_t address, uint32_t value)
		: MemoryValuePredicate(address)
		, _expectedValue(value)
	{}

	void apply(mcu& expected) {
		if(!expected.get_memory().write32(_address, _expectedValue)) {
			micromachine_fail("MemoryValue32Predicate applied at an unmapped address. address=%08x value=%08x", _address,
							  _expectedValue);
		}
	}

	void check(const mcu& actual) const {
		bool memory_access_ok = false;
		uint32_t actualValue = actual.get_memory().read32(_address, memory_access_ok);
		EXPECT_TRUE(memory_access_ok);
		EXPECT_PRED_FORMAT3(assertMemoryEquality, actualValue, _expectedValue, _address);
	}

private:
	static ::testing::AssertionResult assertMemoryEquality(const char*,
														   const char*,
														   const char*,
														   uint32_t actualValue,
														   uint32_t expectedValue,
														   uint32_t address) {

		if(expectedValue == actualValue) {
			return ::testing::AssertionSuccess();
		}

		return ::testing::AssertionFailure()
			<< "Equality check fail for memory at address " << uint32ToStr(address) << std::endl
			<< " * Expected: " << uint32ToStr(expectedValue) << std::endl
			<< " * Actual  : " << uint32ToStr(actualValue) << std::endl;


	}

};

} // namespace micromachine::testing
