# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_wasm_features_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native headers)
  enable_testing()
else()
  set(native_WASM_FEATURES_JS "${PROJECT_SOURCE_DIR}/src/native/wasm/features.mjs")
endif()
set(interface_number 0)
foreach(interface IN ITEMS header module features hub)
  set(target native_test_wasm_features_${interface})
  add_executable(${target} main.cc)
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  if(interface STREQUAL "header")
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_20)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 20 CXX_SCAN_FOR_MODULES OFF)
  elseif(interface STREQUAL "hub")
    target_link_libraries(${target} PRIVATE native::native)
  else()
    target_link_libraries(${target} PRIVATE native::minimal)
  endif()
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.wasm.features.${interface} COMMAND ${target})
  set_tests_properties(native.wasm.features.${interface} PROPERTIES LABELS "host;wasm;baseline" TIMEOUT 30)
  math(EXPR interface_number "${interface_number}+1")
endforeach()
find_program(NATIVE_WASM_NODE NAMES node nodejs)
if(NATIVE_WASM_NODE)
  add_test(NAME native.wasm.features.javascript COMMAND "${NATIVE_WASM_NODE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/javascript.mjs" "${native_WASM_FEATURES_JS}"
    "$<TARGET_FILE:native_test_wasm_features_header>")
else()
  add_test(NAME native.wasm.features.javascript
    COMMAND "${CMAKE_COMMAND}" -E echo "SKIP: Node.js is unavailable")
  set_tests_properties(native.wasm.features.javascript PROPERTIES SKIP_REGULAR_EXPRESSION "SKIP:")
endif()
set_tests_properties(native.wasm.features.javascript PROPERTIES LABELS "host;wasm;javascript" TIMEOUT 30)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
foreach(family RANGE 0 1)
  set(target native_wasm_reject_family_${family})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL invalid_family.cc)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_20)
  target_compile_definitions(${target} PRIVATE NATIVE_INVALID_FAMILY=${family})
  add_test(NAME native.wasm.features.reject_family_${family} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../source_targets/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${target} --config "$<CONFIG>"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-family-${family}-$<CONFIG>.log"
    --diagnostic "invalid operands to binary expression")
  set_tests_properties(native.wasm.features.reject_family_${family} PROPERTIES
    LABELS "host;wasm;negative" RUN_SERIAL TRUE TIMEOUT 180)
endforeach()

# Resolve the real Wasm predefines without a C++ sysroot, then type-check the
# resulting ISA expressions against the public metadata on the native host.
set(wasm_baseline_source "${CMAKE_CURRENT_BINARY_DIR}/wasm-baseline.cc")
add_custom_command(OUTPUT "${wasm_baseline_source}"
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/baseline.py"
    --compiler "${CMAKE_CXX_COMPILER}" --include "${PROJECT_SOURCE_DIR}/src"
    --output "${wasm_baseline_source}"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/baseline.py" "${PROJECT_SOURCE_DIR}/src/native/targets.h"
  VERBATIM)
add_library(native_test_wasm_baseline OBJECT "${wasm_baseline_source}")
target_link_libraries(native_test_wasm_baseline PRIVATE native::headers)
target_compile_features(native_test_wasm_baseline PRIVATE cxx_std_20)
set_target_properties(native_test_wasm_baseline PROPERTIES CXX_SCAN_FOR_MODULES OFF)
