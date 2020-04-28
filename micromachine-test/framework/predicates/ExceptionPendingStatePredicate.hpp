//
// Created by fla on 26.04.20.
// Copyright (c) 2020 The Micromachine Project. All rights reserved.
//

#pragma once

#include "ExceptionStatePredicate.hpp"

namespace micromachine::testing {

class ExceptionPendingStatePredicate : public ExceptionStatePredicate {
private:
	const bool _expectedPendingState;

public:
	ExceptionPendingStatePredicate(exception::Type exceptionType, bool expectedPendingState)
		: ExceptionStatePredicate(exceptionType)
		, _expectedPendingState(expectedPendingState) {}

	ExceptionPendingStatePredicate(exception::Type exceptionType, const mcu& expected)
		: ExceptionPendingStatePredicate(exceptionType,
										 expected.get_exception_vector().interrupt_state(exceptionType).is_pending()) {}

	void apply(mcu& expected) {
		stateOf(expected).set_pending(_expectedPendingState);
	}

	void check(const mcu& actual) const {
		EXPECT_PRED_FORMAT2(assertEquality, _expectedPendingState, stateOf(actual).is_pending());
	}

private:
	::testing::AssertionResult
	assertEquality(const char*, const char*, bool expectedPendingState, bool actualPendingState) const {

		if(expectedPendingState == actualPendingState) {
			return ::testing::AssertionSuccess();
		}

		return ::testing::AssertionFailure()
			   << "Equality check fail for pending status of Exception " << exceptionName(_exceptionType) << std::endl
			   << " * Expected: " << exception::name_of(_exceptionType) << " is "
			   << (expectedPendingState ? "pending" : "NOT pending") << std::endl
			   << " * Actual  : " << exception::name_of(_exceptionType) << " is "
			   << (actualPendingState ? "pending" : "NOT pending") << std::endl;
	}

	static std::string exceptionName(exception::Type e) {
		return exception::name_of(e) + " (number=" + std::to_string(e) + ")";
	}
};

} // namespace micromachine::testing