# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_wasm_boundaries_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^wasm(32|64)$")
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(NATIVE_WASMTIME NAMES wasmtime REQUIRED)
get_filename_component(wasm_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_WASM_OBJDUMP NAMES llvm-objdump
  HINTS "${wasm_compiler_directory}" REQUIRED)

foreach(level IN ITEMS baseline simd relaxed)
  add_executable(native_test_wasm_boundary_${level} ${level}.cc)
  target_link_libraries(native_test_wasm_boundary_${level} PRIVATE native::native)
  set_target_properties(native_test_wasm_boundary_${level} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
endforeach()
add_test(NAME native.wasm.boundaries
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check.py"
    --wasmtime "${NATIVE_WASMTIME}" --objdump "${NATIVE_WASM_OBJDUMP}"
    --baseline $<TARGET_FILE:native_test_wasm_boundary_baseline>
    --simd $<TARGET_FILE:native_test_wasm_boundary_simd>
    --relaxed $<TARGET_FILE:native_test_wasm_boundary_relaxed>
    --output "${CMAKE_CURRENT_BINARY_DIR}/boundaries")
set_tests_properties(native.wasm.boundaries PROPERTIES
  LABELS "wasm;modules;baseline;simd128;relaxed_simd" TIMEOUT 120)
