#pragma once

#include "exception.hpp"
#include "exception_state.hpp"
#include "nvic.hpp"
#include "registers/system_control/icsr_reg.hpp"
#include "registers/system_control/shpr2_reg.hpp"
#include "registers/system_control/shpr3_reg.hpp"

#include <algorithm>
#include <list>
#include <memory>

// Defining this enables exception processing optimizations
#define MICROMACHINE_USE_EXCEPTION_LOOP_UNROLLING

namespace micromachine::system {

class internal_exception_state : public exception_state {
private:
	bool _pending;

public:
	internal_exception_state(exception kind)
		: exception_state(kind)
		, _pending(false) {}

	bool is_pending() const override {
		return _pending;
	}

	void set_pending(bool pending) override {
		_pending = pending;
	}

	bool is_enabled() const override {
		// internal exceptions are always enabled
		return true;
	}

	void set_enable(bool enable) override {
		// Can't disable an internal exception
	}
};

template <int8_t Priority>
class fixed_priority_exception_state : public internal_exception_state {
public:
	using internal_exception_state::internal_exception_state;
	exception::priority_t priority() const override {
		return Priority;
	}

	void set_priority(exception::priority_t priority) override {
		// Can't set priority of a fixed priority exception
	}
};

class nmi_exception_state : public exception_state {
private:
	interrupt_control_state_reg& _pending_state_reg;

public:
	nmi_exception_state(interrupt_control_state_reg& pending_state_reg)
		: exception_state(exception::NMI)
		, _pending_state_reg(pending_state_reg) {}

	exception::priority_t priority() const override {
		return exception::NMI_PRIORITY;
	}

	void set_priority(exception::priority_t priority) override {
		// Can't set priority of a fixed priority exception
	}

	bool is_pending() const override {
		return _pending_state_reg.nmi_pending();
	}

	void set_pending(bool pending) override {
		_pending_state_reg.nmi_pending() = pending;
	}

	bool is_enabled() const override {
		// internal exceptions are always enabled
		return true;
	}

	void set_enable(bool enable) override {
		// Can't disable an internal exception
	}
};

class shpr2_exception_state : public internal_exception_state {
protected:
	shpr2_reg& _priority_reg;

public:
	shpr2_exception_state(exception kind, shpr2_reg& priority_reg)
		: internal_exception_state(kind)
		, _priority_reg(priority_reg) {}
};

class shpr3_exception_state : public exception_state {
protected:
	shpr3_reg& _priority_reg;

public:
	shpr3_exception_state(exception kind, shpr3_reg& priority_reg)
		: exception_state(kind)
		, _priority_reg(priority_reg) {}
};

class pri11_exception_state : public shpr2_exception_state {
public:
	using shpr2_exception_state::shpr2_exception_state;

	exception::priority_t priority() const override {
		return _priority_reg.pri11();
	}

	void set_priority(exception::priority_t priority) override {
		micromachine_check(priority > -1 && priority < 4, "priority not withing range");
		_priority_reg.pri11() = (uint8_t)priority;
	}
};

class icsr_controllable_exception : public shpr3_exception_state {
protected:
	interrupt_control_state_reg& _pending_state_reg;

public:
	icsr_controllable_exception(exception exception, shpr3_reg& priority_reg,
								interrupt_control_state_reg& pending_state_reg)
		: shpr3_exception_state(exception, priority_reg)
		, _pending_state_reg(pending_state_reg) {}

	bool is_enabled() const override {
		// internal exceptions are always enabled
		return true;
	}

	void set_enable(bool enable) override {
		// Can't disable an internal exception
	}
};

class pendsv_exception_state : public icsr_controllable_exception {
public:
	using icsr_controllable_exception::icsr_controllable_exception;

	bool is_pending() const override {
		return _pending_state_reg.pendsv_pending();
	}

	void set_pending(bool pending) override {
		_pending_state_reg.pendsv_pending() = pending;
	}

	exception::priority_t priority() const override {
		return _priority_reg.pri14();
	}

	void set_priority(exception::priority_t priority) override {
		_priority_reg.pri14() = (uint8_t)priority;
	}

};

class systick_exception_state : public icsr_controllable_exception {
public:
	using icsr_controllable_exception::icsr_controllable_exception;

	bool is_pending() const override {
		return _pending_state_reg.systick_pending();
	}

	void set_pending(bool pending) override {
		_pending_state_reg.systick_pending() = pending;
	}

	exception::priority_t priority() const override {
		return _priority_reg.pri15();
	}

	void set_priority(exception::priority_t priority) override {
		_priority_reg.pri15() = (uint8_t)priority;
	}
};

template <size_t ExternalInterruptNumber>
class nvic_based_exception_state : public exception_state {
private:
	nvic& _nvic;

public:
	nvic_based_exception_state(exception kind, nvic& nvic)
		: exception_state(kind)
		, _nvic(nvic) {}

	exception::priority_t priority() const override {
		return _nvic.priority_bits_for<ExternalInterruptNumber>();
	}

	void set_priority(exception::priority_t priority) override {
		_nvic.priority_bits_for<ExternalInterruptNumber>() = priority;
	}

	bool is_pending() const override {
		return _nvic.pending_bit_for<ExternalInterruptNumber>();
	}

	void set_pending(bool pending) override {
		_nvic.pending_bit_for<ExternalInterruptNumber>() = pending;
	}

	bool is_enabled() const override {
		return _nvic.enable_bit_for<ExternalInterruptNumber>();
	}

	void set_enable(bool enable) override {
		_nvic.enable_bit_for<ExternalInterruptNumber>() = enable;
	}
};

class non_implemented_exception_state : public fixed_priority_exception_state<4> {
public:
	non_implemented_exception_state()
		: fixed_priority_exception_state(exception::INVALID) {}

	exception::priority_t priority() const override {
		// not supported
		return exception::MAX_PRIORITY;
	}

	void set_priority(exception::priority_t priority) override {
		// not supported
	}
};

class exception_vector {
public:
	// copy constructor
	exception_vector(nvic& nvic,
					 shpr2_reg& shpr2,
					 shpr3_reg& shpr3,
					 interrupt_control_state_reg& icsr,
					 const exception_vector& existing_state)
		: exception_vector(nvic, shpr2, shpr3, icsr) {
		// Initializes everything as usual.
		// Then copy the exception states from the existing state
		for(exception type : _implemented_exception_types) {
			auto& current = _indexed[type].get();
			auto& existing = existing_state._indexed[type].get();
			current.copy_state_from(existing);
		}
	}

	exception_vector(nvic& nvic, shpr2_reg& shpr2, shpr3_reg& shpr3, interrupt_control_state_reg& icsr)
		: _reset(exception::RESET)
		, _nmi(icsr)
		, _hard_fault(exception::HARDFAULT)
		, _svc(exception::SVCALL, shpr2)
		, _pend_sv(exception::PENDSV, shpr3, icsr)
		, _sys_tick(exception::SYSTICK, shpr3, icsr)
		, _ext_interrupt_0(exception::EXTI_00, nvic)
		, _ext_interrupt_1(exception::EXTI_01, nvic)
		, _ext_interrupt_2(exception::EXTI_02, nvic)
		, _ext_interrupt_3(exception::EXTI_03, nvic)
		, _ext_interrupt_4(exception::EXTI_04, nvic)
		, _ext_interrupt_5(exception::EXTI_05, nvic)
		, _ext_interrupt_6(exception::EXTI_06, nvic)
		, _ext_interrupt_7(exception::EXTI_07, nvic)
		, _ext_interrupt_8(exception::EXTI_08, nvic)
		, _ext_interrupt_9(exception::EXTI_09, nvic)
		, _ext_interrupt_10(exception::EXTI_10, nvic)
		, _ext_interrupt_11(exception::EXTI_11, nvic)
		, _ext_interrupt_12(exception::EXTI_12, nvic)
		, _ext_interrupt_13(exception::EXTI_13, nvic)
		, _ext_interrupt_14(exception::EXTI_14, nvic)
		, _ext_interrupt_15(exception::EXTI_15, nvic)
		, _implemented_exception_types{{
			exception::RESET,
			exception::NMI,
			exception::HARDFAULT,
			exception::SVCALL,
			exception::PENDSV,
			exception::SYSTICK,
			exception::EXTI_00,
			exception::EXTI_01,
			exception::EXTI_02,
			exception::EXTI_03,
			exception::EXTI_04,
			exception::EXTI_05,
			exception::EXTI_06,
			exception::EXTI_07,
			exception::EXTI_08,
			exception::EXTI_09,
			exception::EXTI_10,
			exception::EXTI_11,
			exception::EXTI_12,
			exception::EXTI_13,
			exception::EXTI_14,
			exception::EXTI_15,
		}}
		, _indexed{{_used_for_sp,
					_reset,
					_nmi,
					_hard_fault,
					_reserved_0,
					_reserved_1,
					_reserved_2,
					_reserved_3,
					_reserved_4,
					_reserved_5,
					_reserved_6,
					_svc,
					_reserved_7,
					_reserved_8,
					_pend_sv,
					_sys_tick,
					_ext_interrupt_0,
					_ext_interrupt_1,
					_ext_interrupt_2,
					_ext_interrupt_3,
					_ext_interrupt_4,
					_ext_interrupt_5,
					_ext_interrupt_6,
					_ext_interrupt_7,
					_ext_interrupt_8,
					_ext_interrupt_9,
					_ext_interrupt_10,
					_ext_interrupt_11,
					_ext_interrupt_12,
					_ext_interrupt_13,
					_ext_interrupt_14,
					_ext_interrupt_15}} {}

	size_t highest_accepted_exception_number() const {
		return _indexed.size() - 1;
	}

	size_t supported_exception_count() const {
		return _indexed.size();
	}

private:
	non_implemented_exception_state _used_for_sp;
	fixed_priority_exception_state<exception::RESET_PRIORITY> _reset;
	nmi_exception_state _nmi;
	fixed_priority_exception_state<exception::HARDFAULT_PRIORITY> _hard_fault;
	non_implemented_exception_state _reserved_0;
	non_implemented_exception_state _reserved_1;
	non_implemented_exception_state _reserved_2;
	non_implemented_exception_state _reserved_3;
	non_implemented_exception_state _reserved_4;
	non_implemented_exception_state _reserved_5;
	non_implemented_exception_state _reserved_6;
	pri11_exception_state _svc;
	non_implemented_exception_state _reserved_7;
	non_implemented_exception_state _reserved_8;
	pendsv_exception_state _pend_sv;
	systick_exception_state _sys_tick;

	// TODO: Map the 16 remaining NVIC interrupts (NVIC 16-31) for a total of 48 exception states
	nvic_based_exception_state<0> _ext_interrupt_0;
	nvic_based_exception_state<1> _ext_interrupt_1;
	nvic_based_exception_state<2> _ext_interrupt_2;
	nvic_based_exception_state<3> _ext_interrupt_3;
	nvic_based_exception_state<4> _ext_interrupt_4;
	nvic_based_exception_state<5> _ext_interrupt_5;
	nvic_based_exception_state<6> _ext_interrupt_6;
	nvic_based_exception_state<7> _ext_interrupt_7;
	nvic_based_exception_state<8> _ext_interrupt_8;
	nvic_based_exception_state<9> _ext_interrupt_9;
	nvic_based_exception_state<10> _ext_interrupt_10;
	nvic_based_exception_state<11> _ext_interrupt_11;
	nvic_based_exception_state<12> _ext_interrupt_12;
	nvic_based_exception_state<13> _ext_interrupt_13;
	nvic_based_exception_state<14> _ext_interrupt_14;
	nvic_based_exception_state<15> _ext_interrupt_15;

	const std::array<exception, 22> _implemented_exception_types;
	std::array<std::reference_wrapper<exception_state>, 32> _indexed;

public:
	exception_state& interrupt_state(exception t) {
		return _indexed.at(t);
	}

	const exception_state& interrupt_state(exception t) const {
		return _indexed.at(t);
	}

	size_t active_count() const {
		return std::count_if(std::begin(_indexed),
							 std::end(_indexed),
							 [](const exception_state& e) { return e.is_active(); });
	}

	bool any_active() const {
		return std::any_of(std::begin(_indexed), std::end(_indexed), [](const exception_state& e) {
			return e.is_active();
		});
	}

	bool any_pending() const {
		return std::any_of(std::begin(_indexed), std::end(_indexed), [](const exception_state& e) {
			return e.is_pending();
		});
	}

	exception_state* top_pending() const {

#ifndef MICROMACHINE_USE_EXCEPTION_LOOP_UNROLLING
		exception_state* top = nullptr;
		// Find the pending exception with the lowest priority
		for(exception_state& e : _indexed) {
			// ignore exception that are not pending
			if(!e.is_pending())
				continue;

			// ignore exception that are not enabled
			if(!e.is_enabled())
				continue;

			// select any exception with lower priority value
			if(top == nullptr || e.priority() < top->priority()) {
				top = &e;
			} else if(e.priority() == top->priority()) {
				// When two pending exceptions have the same group priority, the lower pending
				// exception number has priority over the higher pending number as part of the
				// priority precedence rule
				if(e.kind() < top->kind()) {
					top = &e;
				}
			}
		}

		return top;
#else
		size_t top_pending_number = exception::INVALID;
		exception::priority_t top_pending_priority = exception::MAX_PRIORITY;

		#define sort_exception_priority(e) \
		if(e.is_pending() && e.is_enabled()) { \
			if(top_pending_number == exception::INVALID || e.priority() < top_pending_priority) { \
				top_pending_number = e.kind().number(); \
				top_pending_priority = e.priority(); \
			} else if(e.priority() == top_pending_priority) { \
				if(e.kind().number() < top_pending_number) { \
					top_pending_number = e.kind().number(); \
					top_pending_priority = e.priority(); \
				} \
			} \
		}

		sort_exception_priority(_reset);
		sort_exception_priority(_nmi);
		sort_exception_priority(_hard_fault);
		sort_exception_priority(_svc);
		sort_exception_priority(_pend_sv);
		sort_exception_priority(_sys_tick);
		sort_exception_priority(_ext_interrupt_0);
		sort_exception_priority(_ext_interrupt_1);
		sort_exception_priority(_ext_interrupt_2);
		sort_exception_priority(_ext_interrupt_3);
		sort_exception_priority(_ext_interrupt_4);
		sort_exception_priority(_ext_interrupt_5);
		sort_exception_priority(_ext_interrupt_6);
		sort_exception_priority(_ext_interrupt_7);
		sort_exception_priority(_ext_interrupt_8);
		sort_exception_priority(_ext_interrupt_9);
		sort_exception_priority(_ext_interrupt_10);
		sort_exception_priority(_ext_interrupt_11);
		sort_exception_priority(_ext_interrupt_12);
		sort_exception_priority(_ext_interrupt_13);
		sort_exception_priority(_ext_interrupt_14);
		sort_exception_priority(_ext_interrupt_15);

		#undef sort_exception_priority

		if(top_pending_number == exception::INVALID) {
			return nullptr;
		} else {
			return &_indexed[top_pending_number].get();
		}
#endif
	}

	exception::priority_t current_execution_priority(bool primask_pm) const {
		exception::priority_t prio = exception::THREAD_MODE_PRIORITY;

#ifndef MICROMACHINE_USE_EXCEPTION_LOOP_UNROLLING
		// if primask is set, ignore all maskable exceptions by
		// pretending the executing priority is now 0
		if(primask_pm) {
			prio = 0;
		} else {
			for(size_t i = 2; i < 32; i++) {
				const exception_state& e = _indexed.at(i);
				if(!e.is_active())
					continue;
				if(e.priority() < prio) {
					prio = e.priority();
				}
			}
		}
#else
		#define sort_max_priority(e) \
		if (e.is_active() && (e.priority() < prio)) { \
			prio = e.priority(); \
		}

		if(primask_pm) {
			prio = 0;
		} else {

			// TODO: Could early exit the unrolled loop if prio >= THREAD_MODE_PRIORITY because
			// subsequent iterations wont make a difference in the end. ?
			sort_max_priority(_hard_fault);
			sort_max_priority(_svc);
			sort_max_priority(_pend_sv);
			sort_max_priority(_sys_tick);
			sort_max_priority(_ext_interrupt_0);
			sort_max_priority(_ext_interrupt_1);
			sort_max_priority(_ext_interrupt_2);
			sort_max_priority(_ext_interrupt_3);
			sort_max_priority(_ext_interrupt_4);
			sort_max_priority(_ext_interrupt_5);
			sort_max_priority(_ext_interrupt_6);
			sort_max_priority(_ext_interrupt_7);
			sort_max_priority(_ext_interrupt_8);
			sort_max_priority(_ext_interrupt_9);
			sort_max_priority(_ext_interrupt_10);
			sort_max_priority(_ext_interrupt_11);
			sort_max_priority(_ext_interrupt_12);
			sort_max_priority(_ext_interrupt_13);
			sort_max_priority(_ext_interrupt_14);
			sort_max_priority(_ext_interrupt_15);
		}

		#undef sort_max_priority
#endif
		return std::min(exception::THREAD_MODE_PRIORITY, prio);
	}

	void reset() {
		for(exception_state& e : _indexed) {
			e.reset();
		}

		_svc.set_priority(0);
		_pend_sv.set_priority(0);
		_sys_tick.set_priority(0);
	}

private:
	const exception_state& at(size_t index) const {
		return _indexed.at(index);
	}
};
} // namespace micromachine::system
