# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(native_wasm_minimum_consumer LANGUAGES CXX)
find_package(native CONFIG REQUIRED COMPONENTS native)
enable_testing()
set(NATIVE_EXPECTED_WASM_BASELINE 0 CACHE STRING "Expected provider minimum: 0, 1 or 2")
find_program(NATIVE_WASM_NODE NAMES node REQUIRED)
add_executable(native_test_wasm_minimum main.cc)
target_link_libraries(native_test_wasm_minimum PRIVATE native::native)
target_compile_definitions(native_test_wasm_minimum PRIVATE
  NATIVE_EXPECTED_WASM_BASELINE=${NATIVE_EXPECTED_WASM_BASELINE})
set_target_properties(native_test_wasm_minimum PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.minimum COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../wasm_simd/run.mjs" $<TARGET_FILE:native_test_wasm_minimum>)
