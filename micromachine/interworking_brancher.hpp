/* Copyright (C) Flavio Roth - All Rights Reserved

* Unauthorized copying of this file, via any medium is strictly prohibited.

* This project is proprietary and confidential. It can not be copied
and/or distributed without the express permission of Flavio Roth.

*/

#pragma once

#include "exception/exception_return_handler.hpp"
#include "execution/execution_mode.hpp"
#include "registers/epsr_reg.hpp"
#include "registers/pc_reg.hpp"

#include "binops.hpp"
#include "bits.hpp"
#include "types.hpp"

namespace micromachine::system {

class interworking_brancher {
private:
	const execution_mode& _execution_mode;
	epsr_reg& _epsr_reg;
	exception_return_handler& _exception_return_handler;
	pc_reg& _pc_reg;

public:
	interworking_brancher(const execution_mode& execution_mode,
						  epsr_reg& epsr_reg,
						  exception_return_handler& exception_return_handler,
						  pc_reg& pc_reg)
		: _execution_mode(execution_mode)
		, _epsr_reg(epsr_reg)
		, _exception_return_handler(exception_return_handler)
		, _pc_reg(pc_reg) {}

	void branch_interworking(uint32_t address) {
		if(_execution_mode == execution_mode::handler && 0b1111 == bits<28, 4>::of(address)) {
			uint32_t return_address = bits<0, 28>::of(address);
			_exception_return_handler.exception_return(return_address);
		} else {

			// trigger a hard fault on next instruction if Thumb bit is not set
			_epsr_reg.set_thumb_bit(bits<0>::of(address));

			// Inter-working branch, thumb bit is always cleared
			_pc_reg.branch(address);
		}
	}

	void branch_link_interworking(uint32_t address) {
		// if the thumb bit is not set, a hard fault must be taken on the next instruction
		// copy the last bit of jump_addr to allow this to happen
		_epsr_reg.set_thumb_bit(bits<0>::of(address));

		// Inter-working branch, thumb bit is always cleared
		bits<0>::of(address) = false;
		_pc_reg.branch(address);
	}
};

} // namespace micromachine::system
