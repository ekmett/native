# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_wasm_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
find_program(NATIVE_WASM_NODE NAMES node REQUIRED)
add_executable(native_test_wasm_simd main.cc)
target_link_libraries(native_test_wasm_simd PRIVATE native::native)
set_target_properties(native_test_wasm_simd PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.simd COMMAND "${NATIVE_WASM_NODE}" "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_simd>)
set_tests_properties(native.wasm.simd PROPERTIES SKIP_RETURN_CODE 77 LABELS "wasm;modules;simd128")

add_library(native_test_wasm_simd_codegen OBJECT codegen.cc)
target_link_libraries(native_test_wasm_simd_codegen PRIVATE native::native)
target_compile_options(native_test_wasm_simd_codegen PRIVATE -O2)
set_target_properties(native_test_wasm_simd_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(wasm_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(wasm_objdump NAMES llvm-objdump HINTS "${wasm_compiler_dir}" REQUIRED)
add_test(NAME native.wasm.simd.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${wasm_objdump}"
  --source "${CMAKE_CURRENT_SOURCE_DIR}/codegen.cc"
  --output "${CMAKE_CURRENT_BINARY_DIR}/codegen-$<CONFIG>.json"
  $<TARGET_OBJECTS:native_test_wasm_simd_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.wasm.simd.codegen PROPERTIES LABELS "wasm;simd128;codegen")

add_executable(native_test_wasm_simd_clients clients.cc)
target_link_libraries(native_test_wasm_simd_clients PRIVATE native::native)
set_target_properties(native_test_wasm_simd_clients PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.simd.clients COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_simd_clients>)
set_tests_properties(native.wasm.simd.clients PROPERTIES
  SKIP_RETURN_CODE 77 LABELS "wasm;modules;simd128;wide")

add_executable(native_test_wasm_simd_api api.cc)
target_link_libraries(native_test_wasm_simd_api PRIVATE native::native)
set_target_properties(native_test_wasm_simd_api PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.simd.api COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_simd_api>)
set_tests_properties(native.wasm.simd.api PROPERTIES
  SKIP_RETURN_CODE 77 LABELS "wasm;modules;simd128;api")
