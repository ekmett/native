# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_wasm_math_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
find_program(NATIVE_WASM_NODE NAMES node REQUIRED)
find_program(NATIVE_WASM_WASMTIME NAMES wasmtime REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(wasm_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_WASM_OBJDUMP NAMES llvm-objdump HINTS "${wasm_compiler_directory}" REQUIRED)
add_executable(native_test_wasm_math main.cc)
target_link_libraries(native_test_wasm_math PRIVATE native::native)
set_target_properties(native_test_wasm_math PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.math COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../wasm_simd/run.mjs" $<TARGET_FILE:native_test_wasm_math>)
set_tests_properties(native.wasm.math PROPERTIES LABELS "wasm;simd128;math;modules" SKIP_RETURN_CODE 77)
add_test(NAME native.wasm.math.base_engine COMMAND "${NATIVE_WASM_WASMTIME}"
  run -Ccache=n -Wsimd=y -Wrelaxed-simd=n $<TARGET_FILE:native_test_wasm_math>)
set_tests_properties(native.wasm.math.base_engine PROPERTIES LABELS "wasm;simd128;math;modules")
add_library(native_test_wasm_math_codegen OBJECT codegen.cc)
target_link_libraries(native_test_wasm_math_codegen PRIVATE native::native)
target_compile_options(native_test_wasm_math_codegen PRIVATE -O2)
set_target_properties(native_test_wasm_math_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
add_test(NAME native.wasm.math.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${NATIVE_WASM_OBJDUMP}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/codegen-$<CONFIG>.json"
  $<TARGET_OBJECTS:native_test_wasm_math_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.wasm.math.codegen PROPERTIES LABELS "wasm;simd128;math;codegen")

# The same promoted-kernel regressions run through the baseline Wasm engine.
foreach(operation IN ITEMS tanh atan2)
  add_executable(native_test_wasm_${operation} ../core_regression/promoted_${operation}.cc)
  target_link_libraries(native_test_wasm_${operation} PRIVATE native::native)
  target_compile_definitions(native_test_wasm_${operation} PRIVATE NATIVE_TEST_IMPORT=1)
  target_compile_options(native_test_wasm_${operation} PRIVATE -msimd128 -mno-relaxed-simd)
  set_target_properties(native_test_wasm_${operation} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  add_test(NAME native.wasm.${operation} COMMAND "${NATIVE_WASM_NODE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wasm_simd/run.mjs" $<TARGET_FILE:native_test_wasm_${operation}>)
  set_tests_properties(native.wasm.${operation} PROPERTIES LABELS "wasm;simd128;math;modules" SKIP_RETURN_CODE 77)
  add_test(NAME native.wasm.${operation}.base_engine COMMAND "${NATIVE_WASM_WASMTIME}"
    run -Ccache=n -Wsimd=y -Wrelaxed-simd=n $<TARGET_FILE:native_test_wasm_${operation}>)
  set_tests_properties(native.wasm.${operation}.base_engine PROPERTIES LABELS "wasm;simd128;math;modules")
endforeach()
