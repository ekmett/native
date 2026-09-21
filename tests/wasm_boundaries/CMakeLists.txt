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
find_program(NATIVE_WASM_WASMTIME NAMES wasmtime REQUIRED)
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
    --wasmtime "${NATIVE_WASM_WASMTIME}" --objdump "${NATIVE_WASM_OBJDUMP}"
    --baseline $<TARGET_FILE:native_test_wasm_boundary_baseline>
    --simd $<TARGET_FILE:native_test_wasm_boundary_simd>
    --relaxed $<TARGET_FILE:native_test_wasm_boundary_relaxed>
    --output "${CMAKE_CURRENT_BINARY_DIR}/boundaries")
set_tests_properties(native.wasm.boundaries PROPERTIES
  LABELS "wasm;modules;baseline;simd128;relaxed_simd" TIMEOUT 120)

# Each rejected call has an otherwise identical successfully compiled control.
set(boundary_index 0)
foreach(boundary IN ITEMS caller_target missing_storage short_shape lane_index shuffle_index default_storage)
  math(EXPR boundary_index "${boundary_index}+1")
  foreach(control IN ITEMS positive negative)
    set(target native_test_wasm_boundary_${control}_${boundary})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL reject.cc)
    target_link_libraries(${target} PRIVATE native::native)
    target_compile_definitions(${target} PRIVATE NATIVE_WASM_CASE=${boundary_index}
      NATIVE_WASM_POSITIVE=$<STREQUAL:${control},positive>)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  endforeach()
  if(boundary STREQUAL "caller_target")
    set(diagnostic "requires target feature 'simd128'")
  elseif(boundary MATCHES "storage|shape")
    set(diagnostic "undefined template|incomplete type")
  else()
    set(diagnostic "no matching member function|no matching function|constraints not satisfied")
  endif()
  add_test(NAME native.wasm.boundaries.reject.${boundary}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --positive native_test_wasm_boundary_positive_${boundary}
      --negative native_test_wasm_boundary_negative_${boundary}
      --config "$<CONFIG>" --diagnostic "${diagnostic}"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${boundary}.log")
  set_tests_properties(native.wasm.boundaries.reject.${boundary} PROPERTIES
    RUN_SERIAL TRUE TIMEOUT 180 LABELS "wasm;modules;negative;simd128")
endforeach()
