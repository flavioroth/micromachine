
//
// Created by fla on 04.05.20.
// Copyright (c) 2020 The Micromachine Project. All rights reserved.
//

#pragma once

#include "registers/memory_mapped_reg.hpp"
#include "types/types.hpp"

namespace micromachine::system {

class config_and_control_reg : public memory_mapped_reg {
public:
	static constexpr uint32_t CCR = 0xE000ED14;
	static constexpr uint32_t RESET_VALUE = 0b1000001000;

	using stack_align_bit = bits<9>;
	using unalign_trap_bit = bits<3>;

	config_and_control_reg()
		: memory_mapped_reg(RESET_VALUE)
	{}

	bool stack_align() const {
		return self<stack_align_bit>();
	}

	bool unaligned_trap() const {
		return self<unalign_trap_bit>();
	}

	void reset() override {
		_word = RESET_VALUE;
	}

private:
	void set(uint32_t word) override {
		// writes to this register are ignored
	}

	uint32_t get() const override {
		return _word;
	}
};

} // namespace micromachine::system
