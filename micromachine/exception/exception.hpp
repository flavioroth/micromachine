/* Copyright (C) Flavio Roth - All Rights Reserved

* Unauthorized copying of this file, via any medium is strictly prohibited.

* This project is proprietary and confidential. It can not be copied
and/or distributed without the express permission of Flavio Roth.

*/

#pragma once

#include <cstdint>
#include <string>

namespace micromachine::system {

class exception {
public:
	using priority_t = int8_t;
	static constexpr priority_t MAX_PRIORITY = std::numeric_limits<int8_t>::max();
	static constexpr priority_t RESET_PRIORITY = -3;
	static constexpr priority_t NMI_PRIORITY = -2;
	static constexpr priority_t HARDFAULT_PRIORITY = -1;
	static constexpr priority_t THREAD_MODE_PRIORITY = 4; // this is the default priority
	enum Type : uint32_t {
		INVALID = 0,
		RESET = 1,
		NMI = 2,
		HARDFAULT = 3,
		_RESERVED_0 = 4,
		_RESERVED_1 = 5,
		_RESERVED_2 = 6,
		_RESERVED_3 = 7,
		_RESERVED_4 = 8,
		_RESERVED_5 = 9,
		_RESERVED_6 = 10,
		SVCALL = 11,
		_RESERVED_7 = 12,
		_RESERVED_8 = 13,
		PENDSV = 14,
		SYSTICK = 15,
		EXTI_00 = 16,
		EXTI_01 = 17,
		EXTI_02 = 18,
		EXTI_03 = 19,
		EXTI_04 = 20,
		EXTI_05 = 21,
		EXTI_06 = 22,
		EXTI_07 = 23,
		EXTI_08 = 24,
		EXTI_09 = 25,
		EXTI_10 = 26,
		EXTI_11 = 27,
		EXTI_12 = 28,
		EXTI_13 = 29,
		EXTI_14 = 30,
		EXTI_15 = 31,
	};

	static Type from_number(uint8_t number) {
		return static_cast<Type>(number);
	}

	static std::string name_of(exception::Type e) {
		static const char* names[] = {
			"INVALID",      "RESET",       "NMI",          "HARDFAULT",    "_RESERVED_0",
			"_RESERVED_1",  "_RESERVED_2", "_RESERVED_3",  "_RESERVED_4",  "_RESERVED_5",
			"_RESERVED_6", "SVCALL",     "_RESERVED_7", "_RESERVED_8", "PENDSV",
			"SYSTICK",     "EXTI_00",    "EXTI_01",     "EXTI_02",     "EXTI_03",
			"EXTI_04",     "EXTI_05",    "EXTI_06",     "EXTI_07",     "EXTI_08",
			"EXTI_09",     "EXTI_10",    "EXTI_11",     "EXTI_12",     "EXTI_13",
			"EXTI_14",     "EXTI_15",
		};
		return names[e];
	}

};

} // namespace micromachine::system
