
#include <cxxopts.hpp>

#include "io/stream_server.hpp"
#include "loader.hpp"
#include "mcu.hpp"
#include "peripherals/iodev.hpp"

#include <chrono>
#include <unistd.h>

static const char* OPT_EXECUTABLE = "executable";
static const char* OPT_TESTING = "testing";
static const char* OPT_USART_INPUT_STR = "usart-input-string";
static const char* OPT_USART_INPUT_FILE = "usart-input-file";

int main(int argc, char** argv) {

	bool testing_enabled = false;
	std::string executable_path;
	std::string usart_input_string;
	std::string usart_input_file_path;

	cxxopts::Options options("micromachine", "ARMv6-M emulator");
	options.add_options()
		(OPT_EXECUTABLE, "Executable path", cxxopts::value<std::string>())
		(OPT_TESTING, "Enable testing", cxxopts::value<bool>()->default_value("false"))
		(OPT_USART_INPUT_STR, "usart input data to be passed to the application", cxxopts::value<std::string>()->default_value(""))
		(OPT_USART_INPUT_FILE, "file containing usart input data to be passed to the application", cxxopts::value<std::string>()->default_value(""))
		("h,help", "Print usage")
	;

	try {
		options.parse_positional({OPT_EXECUTABLE});
		auto result = options.parse(argc, argv);

		if (result.count("help")) {
			std::cout << options.help() << std::endl;
			return EXIT_SUCCESS;
		}

		if(result.count(OPT_EXECUTABLE) != 1) {
			std::cout << options.help() << std::endl;
			return EXIT_FAILURE;
		}

		if(result.count(OPT_USART_INPUT_STR) && result.count(OPT_USART_INPUT_FILE)) {
			throw cxxopts::OptionParseException("Can't specify both " +
												std::string(OPT_USART_INPUT_STR) +
												" and " +
												OPT_USART_INPUT_FILE);
		}

		usart_input_string = result[OPT_USART_INPUT_STR].as<std::string>();
		usart_input_file_path = result[OPT_USART_INPUT_FILE].as<std::string>();

		testing_enabled = result[OPT_TESTING].as<bool>();
		executable_path = result[OPT_EXECUTABLE].as<std::string>();
	} catch (const cxxopts::OptionParseException& e) {
		fprintf(stderr, "Error: %s\n", e.what());
		return EXIT_FAILURE;
	}

	micromachine::system::mcu mcu;
	micromachine::system::loader::program::ptr program = micromachine::system::loader::load_elf(executable_path.c_str(), mcu.get_memory(), testing_enabled);

	if(program->is_null()) {
		fprintf(stderr, "Error: Failed to load the given ELF file %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	if(usart_input_string.size()) {
		mcu.get_usart_controller().write_string(usart_input_string);
	}

	if(usart_input_file_path.size()) {

		std::ifstream usart_input_file(usart_input_file_path, std::ios::binary);
		if(!usart_input_file) {
			fprintf(stderr, "Error: Failed to open usart-input-file=%s\n", usart_input_file_path.c_str());
			return EXIT_FAILURE;
		}

		char byte;
		while(usart_input_file.read(&byte, 1)) {
			mcu.get_usart_controller().send(byte);
		}
	}

	stream_server usart_streamer(mcu.get_usart_controller(), "usart0", "/tmp/micromachine");

	// This client print (stdout) everything is written in USART0->TX register.
	stream_connection usart_printer(
		usart_streamer.pathname(),
		nullptr,
		[](const uint8_t* buffer, size_t size, void*) -> void {
			if(0 == write(STDOUT_FILENO, buffer, size)) {
				fprintf(stderr, "failed to write to stdout\n");
			}
		},
		nullptr
	);

	mcu.set_io_callback([](uint8_t data) {
		if(0 == write(STDOUT_FILENO, &data, 1)) {
			fprintf(stderr, "failed to write to stdout\n");
		}
	});

	while(usart_streamer.client_count() == 0) {
		std::this_thread::sleep_for(1ms);
		fprintf(stderr, "waiting for the usart printer...\n");
	}

	mcu.reset(program->entry_point());

	auto start = std::chrono::steady_clock::now();
	decltype(start) end;
	for(;;) {
		micromachine::system::mcu::step_result state = mcu.step();
		if(state == micromachine::system::mcu::step_result::HALT) {
			end = std::chrono::steady_clock::now();
			break;
		}
	}
	uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	double elapsed_secs = elapsed_ms / 1000.0;
	fprintf(stderr, "elapsed: %f seconds\n", elapsed_secs);

	uint64_t instructions_executed = mcu.get_cpu().debug_instruction_counter();
	double perf = instructions_executed / elapsed_secs;
	fprintf(stderr, "ran %lu instruction(s), %f i/s\n", instructions_executed, perf);

	usart_streamer.close();
	usart_printer.close();

	fflush(stdout);

	return EXIT_SUCCESS;
}
