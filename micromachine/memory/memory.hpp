#pragma once

#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "exception/exception_controller.hpp"
#include "mapping.hpp"

#include "register_map.hpp"

namespace micromachine::system {

class memory {
public:
	using region_vec = std::vector<memory_mapping>;

private:
	region_vec _regions;
	exception_controller& _exception_controller;
	const register_map _memory_mapped_registers;

public:
	memory(exception_controller& exception_controller, register_map::map memory_mapped_registers_map)
		: _exception_controller(exception_controller)
		, _memory_mapped_registers(std::move(memory_mapped_registers_map)) {}

	bool write32(uint32_t address, uint32_t val) {
		return this->write<uint32_t>(address, val);
	}

	bool write16(uint32_t address, uint16_t val) {
		return this->write<uint16_t>(address, val);
	}

	bool write8(uint32_t address, uint8_t val) {
		return this->write<uint8_t>(address, val);
	}

	uint32_t read32(uint32_t address) const {
		return read<uint32_t>(address);
	}

	uint32_t read32(uint32_t address, bool& ok) const {
		return read<uint32_t>(address, ok);
	}

	uint16_t read16(uint32_t address) const {
		return read<uint16_t>(address);
	}

	uint16_t read16(uint32_t address, bool& ok) const {
		return read<uint16_t>(address, ok);
	}

	uint16_t read16_unchecked(uint32_t address) const {
		return read_unchecked<uint16_t>(address);
	}

	uint32_t read32_unchecked(uint32_t address) const {
		return read_unchecked<uint32_t>(address);
	}

	uint8_t read8(uint32_t address) const {
		return read<uint8_t>(address);
	}

	uint8_t read8(uint32_t address, bool& ok) const {
		return read<uint8_t>(address, ok);
	}

	uint8_t read8_unchecked(uint32_t address) const {
		return read_unchecked<uint8_t>(address);
	}

	void map(uint8_t* host, uint32_t start_addr, uint32_t size, const std::string& name) {
		_regions.emplace_back(host, start_addr, size, name);
	}

	void map(uint8_t* host, uint32_t start_addr, uint32_t size) {
		_regions.emplace_back(host, start_addr, size);
	}

	template <typename access_t>
	static bool constexpr is_aligned(uint32_t address) {
		return 0 == (address & (sizeof(access_t) - 1));
	}

	const region_vec& regions() const {
		return _regions;
	}

	const memory_mapping* find_const_region(uint32_t address) const {
		const auto it =
			std::find_if(std::begin(_regions), std::end(_regions), [=](const memory_mapping& mm) {
				return in_range(address, mm);
			});

		if(std::end(_regions) != it) {
			return it.base();
		}
		return nullptr;
	}

private:
	template <typename access_t>
	static access_t& access_host(void* host_addr) {
		return *(static_cast<access_t*>(host_addr));
	}

	template <typename access_t>
	static void write(memory_mapping* region, uint32_t addr, access_t val) {
		access_host<access_t>(region->translate(addr)) = val;
	}

	template <typename access_t>
	static access_t read(const memory_mapping* region, uint32_t addr) {
		return access_host<access_t>(region->translate(addr));
	}

	template <typename access_t>
	bool write(uint32_t address, access_t value) {

		// If the memory access is 32-bit aligned, it might be a memory
		// mapped register.
		if(std::is_same<uint32_t, access_t>::value) {
			if(auto memory_mapped_register = _memory_mapped_registers.get_register_at(address)) {
				memory_mapped_register->get() = value;
				return true;
			}
		}

		if(!is_aligned<access_t>(address)) {
			_exception_controller.raise_memory_hardfault();
			return false;
		}

		memory_mapping* region = find_region(address);

		if(!region) {
			_exception_controller.raise_memory_hardfault();
			return false;
		}

		write<access_t>(region, address, value);

		return true;
	}

	template <typename access_t>
	access_t read(uint32_t address, bool& ok) const {

		// If the memory access is 32-bit aligned, it might be a memory
		// mapped register.
		if(std::is_same<uint32_t, access_t>::value) {
			if(auto memory_mapped_register = _memory_mapped_registers.get_register_at(address)) {
				ok = true;
				return memory_mapped_register->get();
			}
		}

		if(!is_aligned<access_t>(address)) {
			_exception_controller.raise_memory_hardfault();
			ok = false;
			return 0;
		}

		const memory_mapping* region = find_const_region(address);

		if(!region) {
			_exception_controller.raise_memory_hardfault();
			ok = false;
			return 0;
		}
		ok = true;
		return read<access_t>(region, address);
	}

	template <typename access_t>
	access_t read_unchecked(uint32_t address) const {
		const memory_mapping* region = find_const_region(address);
		if(!region) {
			return 0;
		} else {
			return read<access_t>(region, address);
		}
	}

	template <typename access_t>
	access_t read(uint32_t address) const {
		bool dontcare;
		return read<access_t>(address, dontcare);
	}

	static bool in_range(uint32_t address, const memory_mapping& mm) {
		return address >= mm.start() && address < mm.end();
	}

	memory_mapping* find_region(uint32_t address) {
		return const_cast<memory_mapping*>(find_const_region(address));
	}
};
} // namespace micromachine::system
