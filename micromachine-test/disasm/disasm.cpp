
#include "helpers/disassembler.hpp"
#include "nvic.hpp"
#include <fstream>
#include <gtest/gtest.h>

TEST(Disasm, DisamRegressionOf16bitInstructions)
{
	std::ifstream truth(TEST_SRC_DIR "/disasm/disasm-16.txt");
	std::string line;
	char buf[128] = {0};
	int i = 0;
	for(i = 0; i < 0x10000; i++) {
		instruction_pair instruction((uint16_t)i, 0);
		std::string str = disasm::disassemble_instruction(instruction, 64);
		sprintf(buf, "%04x %s", (uint16_t)instruction.first(), str.c_str());
		//truth << buf << std::endl;
		EXPECT_TRUE(std::getline(truth, line));
		EXPECT_STREQ(buf, line.c_str());
	}
	EXPECT_EQ(0x10000, i);
	truth.close();
}
