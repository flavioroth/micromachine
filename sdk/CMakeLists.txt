set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/micromachine.toolchain")
project(micromachine-sdk)

set(MICROMACHINE_VM CACHE STRING "MICROMACHINE_VM-NOTFOUND")
cmake_minimum_required(VERSION 3.2)

if(MICROMACHINE_VM)
	message(INFO "MICROMACHINE_VM=${MICROMACHINE_VM}")
else()
	message(FATAL_ERROR "${MICROMACHINE_VM} MICROMACHINE_VM not found. You must pass the path to the micromachine vm
	executable. It is used for running tests")
endif()

function(add_micromachine_executable)
	set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/link.ld)
	set_target_properties(${ARGV0} PROPERTIES LINK_FLAGS "-T ${LINKER_SCRIPT}")
	set_target_properties(${ARGV0} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

	# Link with system library that provides implementation for the libc primitives and
	# cmsis-headers which provides the include path for the CMSIS headers.
	target_link_libraries(${ARGV0} PRIVATE
		-Wl,--whole-archive
		system
		-Wl,--no-whole-archive
		cmsis-headers
	)

endfunction()

function(generate_dissasembly)
	set(CROSS_OBJCOPY "${TARGET_PREFIX}objcopy")
	set(CROSS_OBJDUMP "${TARGET_PREFIX}objdump")
	set(INPUT_TARGET_NAME ${ARGV0})
	set(OUTPUT_TARGET_NAME ${INPUT_TARGET_NAME}_disassembly)
	set(OUTPUT_FILE "${OUTPUT_TARGET_NAME}.txt")
	add_custom_target(${OUTPUT_TARGET_NAME} ALL
		COMMAND
			${CROSS_OBJDUMP} -Mforce-thumb -marm  -D -S --prefix-addresses $<TARGET_FILE:${INPUT_TARGET_NAME}> > ${OUTPUT_FILE}
		DEPENDS
			${INPUT_TARGET_NAME}
		VERBATIM
	)
	set_target_properties(${OUTPUT_TARGET_NAME} PROPERTIES OUTFILE "${OUTPUT_FILE}")
endfunction()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/system/include)
include_directories(CMSIS/Core/Include)
add_subdirectory(cmsis)
add_subdirectory(system)
add_subdirectory(freertos)
add_subdirectory(examples)
add_subdirectory(test)
