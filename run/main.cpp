

#include <chrono>
#include <unistd.h>
#include "cpu.hpp"
#include "programmer.hpp"

#include "gdb-server.hpp"

int main(int argc, const char** argv) {



	if(argc < 2) {
		fprintf(stderr, "usage: %s <elf-executable>\n", argv[0]);
		return EXIT_FAILURE;
	}

	cpu c;

	programmer::program::ptr program = programmer::load_elf(argv[1], c.mem());

	if(program->is_null()) {
		fprintf(stderr, "Error: Failed to load the given ELF file %s\n", argv[0]);
		return EXIT_FAILURE;
	}

	c.set_io_callback([](uint8_t op, uint8_t data){
		if(0 == write(STDOUT_FILENO, &data, 1)) {
			fprintf(stderr, "failed to write to stdout\n");
		}
	});

	c.reset(program->entry_point());
	gdb_server g(c);
	auto start = std::chrono::steady_clock::now();
	decltype(start) end;
	for(;;) {
		cpu::State state = c.step();
		if(state == cpu::State::BREAK || state == cpu::State::FAULT) {
			end = std::chrono::steady_clock::now();
			break;
		}
	}
	uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	double elapsed_secs = elapsed_ms/1000.0;
	fprintf(stderr, "elapsed: %f seconds\n", elapsed_secs);

	double perf = c.debug_instruction_counter() / elapsed_secs;
	fprintf(stderr, "run %ld instruction(s), %f i/s\n", c.debug_instruction_counter(), perf);
	//c.regs().print();
	return EXIT_SUCCESS;
}
