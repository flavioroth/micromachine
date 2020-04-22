#ifndef MICROMACHINE_SP_REG_HPP
#define MICROMACHINE_SP_REG_HPP

#include "control_reg.hpp"
#include "execution_mode.hpp"
#include "standard_reg.hpp"

class sp_reg : public ireg {
private:
	// Banked stack pointers
	// 0: SP Main
	// 1: SP Process
	std::array<uint32_t, 2> _banked_stack_pointers;
	const execution_mode& _execution_mode;
	const control_reg& _control_reg;

public:
	sp_reg(const execution_mode& execution_mode, const control_reg& control_reg)
		: _banked_stack_pointers{{0, 0}}
		, _execution_mode(execution_mode)
		, _control_reg(control_reg) {}

	sp_reg(const execution_mode& execution_mode,
		   const control_reg& control_reg,
		   const sp_reg& existing_state)
		: _banked_stack_pointers(existing_state._banked_stack_pointers)
		, _execution_mode(execution_mode)
		, _control_reg(control_reg) {}

	using ireg::operator=;

	enum class StackType : size_t { Main = 0, Process = 1 };

	void set_specific_banked_sp(StackType type, uint32_t word) {
		_banked_stack_pointers[(size_t)type] = word;
	}

	uint32_t get_specific_banked_sp(StackType type) {
		return _banked_stack_pointers[(size_t)type];
	}

	void reset() override {
		set(0);
	}

private:
	static constexpr uint32_t VALUE_MASK = binops::make_mask<uint32_t>(2, 30);

	void set(uint32_t word) override {
		// these two bits should always be zero, or UNPREDICTABLE
		if (bits<0, 2>::of(word)) {
			// unpredictable
		}
		current_banked_value() = word;
	}

	uint32_t get() const override {
		return VALUE_MASK & current_banked_value();
	}

	uint32_t& current_banked_value() {
		if (execution_mode::thread == _execution_mode) {
			return _banked_stack_pointers[_control_reg.sp_sel()];
		} else {
			// handler mode
			return _banked_stack_pointers[0];
		}
	}

	const uint32_t& current_banked_value() const {
		if (execution_mode::thread == _execution_mode) {
			return _banked_stack_pointers[_control_reg.sp_sel()];
		} else {
			// handler mode
			return _banked_stack_pointers[0];
		}
	}
};

#endif // MICROMACHINE_SP_REG_HPP
