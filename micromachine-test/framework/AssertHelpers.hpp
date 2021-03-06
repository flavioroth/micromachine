#pragma once

#include <stdexcept>
#include <string>

namespace testing {

template <bool Cond>
struct exceptionalize_static_assert;

template <>
struct exceptionalize_static_assert<true> {
	explicit exceptionalize_static_assert(const char*) {}
};

template <bool Exp>
void static_assert_exception(const char* message) {
	testing::exceptionalize_static_assert<Exp> ex(message);
}

template <typename FromType, typename ToType>
void assert_convertible(const char* message) {
	static_assert_exception<std::is_convertible<FromType, ToType>::value>(message);
}

template <typename FromType, typename ToType>
void assert_assignable(const char* message) {
	static_assert_exception<std::is_assignable<FromType, ToType>::value>(message);
}


} // namespace testing
