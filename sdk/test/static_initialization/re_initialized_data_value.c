
#include <system.h>
#include <stdio.h>

uint32_t test_variable_1 = 0x99c0feee;

extern void _init();

// should be equivalent as above
static uint32_t test_variable_2 = 0xbeefc0de;

void main() {
	static uint32_t test_variable_3 = 0x42424242;

	// check that variables have their initial values
	printf("%08x %i\n", test_variable_1, _section_data_contains(&test_variable_1));
	printf("%08x %i\n", test_variable_2, _section_data_contains(&test_variable_2));
	printf("%08x %i\n", test_variable_3, _section_data_contains(&test_variable_3));
	printf("----\n");

	// store new values in the variables
	test_variable_1 = 0xC0CAF00D;
	test_variable_2 = 0xACECACA0;
	test_variable_3 = 0xFEEDC0DE;
	printf("%08x %i\n", test_variable_1, _section_data_contains(&test_variable_1));
	printf("%08x %i\n", test_variable_2, _section_data_contains(&test_variable_2));
	printf("%08x %i\n", test_variable_3, _section_data_contains(&test_variable_3));
	printf("----\n");

	// call the reinitialization code again
	_init();


	// check that the variables are reset to their initial values
	printf("%08x %i\n", test_variable_1, _section_data_contains(&test_variable_1));
	printf("%08x %i\n", test_variable_2, _section_data_contains(&test_variable_2));
	printf("%08x %i\n", test_variable_3, _section_data_contains(&test_variable_3));
	printf("----\n");
}
