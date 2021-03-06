cmake_minimum_required(VERSION 3.0.2)

include(ExternalProject)

project(micromachine)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Winit-self -Wextra -Wno-ignored-qualifiers  -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-maybe-uninitialized")

find_package(Threads)

include(cotire)

add_subdirectory(micromachine)
add_subdirectory(vm)
add_subdirectory(termdbg)
add_subdirectory(micromachine-test)

ExternalProject_Add(micromachine_sdk_project
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/sdk
	BINARY_DIR ${CMAKE_BINARY_DIR}/sdk
	BUILD_ALWAYS TRUE
	CONFIGURE_COMMAND ${CMAKE_COMMAND}
		-DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/sdk/micromachine.toolchain
		-DMICROMACHINE_VM=$<TARGET_FILE:micromachine_vm>
		${CMAKE_SOURCE_DIR}/sdk
	INSTALL_COMMAND ""
	DEPENDS micromachine_vm
)

add_custom_target(sdk ALL DEPENDS micromachine_sdk_project)

add_custom_target(sdk_tests DEPENDS sdk)

add_custom_command(
	TARGET sdk_tests
	COMMENT "Run SDK tests"
	POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND} ARGS -V
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/sdk/test
)
