# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_arm_dotprod_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  return()
endif()

set(interface_number 0)
set(interfaces arm_hub module hub)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Installed consumers see public modules, not private implementation headers.
  set(interface_number 1)
  set(interfaces module hub)
endif()
foreach(interface IN LISTS interfaces)
  set(target native_test_arm_dotprod_${interface})
  add_executable(${target} main.cc)
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.arm.dotprod.${interface} COMMAND ${target})
  set_tests_properties(native.arm.dotprod.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;arm;dotprod")
  math(EXPR interface_number "${interface_number}+1")
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

function(native_arm_dotprod_baseline target)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  # Each fixture function enables only its own feature. Explicit CPU selection
  # avoids inheriting the platform's default architecture bundle.
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2
      /clang:-mcpu=generic /clang:-march=armv8-a+nodotprod+nordm)
  else()
    target_compile_options(${target} PRIVATE -O2 -mcpu=generic
      -march=armv8-a+nodotprod+nordm)
  endif()
endfunction()

add_library(native_test_arm_dotprod_codegen OBJECT codegen.cc)
native_arm_dotprod_baseline(native_test_arm_dotprod_codegen)
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/dotprod-codegen-object.txt"
  CONTENT "$<TARGET_OBJECTS:native_test_arm_dotprod_codegen>\n")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_ARM_DOTPROD_OBJDUMP NAMES llvm-objdump
  HINTS "${compiler_directory}" REQUIRED)
add_test(NAME native.arm.dotprod.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
    --objdump "${NATIVE_ARM_DOTPROD_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/dotprod-codegen.txt"
    $<TARGET_OBJECTS:native_test_arm_dotprod_codegen>)
set_tests_properties(native.arm.dotprod.codegen PROPERTIES
  LABELS "host;codegen;baseline;arm;dotprod")
# Let CMake preserve the platform compiler command, quoting and driver flags.
add_library(native_test_arm_dotprod_negative_target OBJECT EXCLUDE_FROM_ALL negative.cc)
native_arm_dotprod_baseline(native_test_arm_dotprod_negative_target)
add_test(NAME native.arm.dotprod.negative_target
  COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/tests/source_targets/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
    --target native_test_arm_dotprod_negative_target --config "$<CONFIG>"
    --diagnostic "requires target feature.*dotprod|target specific option mismatch"
    --log "${CMAKE_CURRENT_BINARY_DIR}/negative-target.txt")
set_tests_properties(native.arm.dotprod.negative_target PROPERTIES
  RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;compile;baseline;arm;dotprod")

# The public abstraction must add no instructions relative to the native helper.
add_test(NAME native.arm.dotprod.zero_overhead
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../arm_dotprod/check_equivalence.py"
    --objdump "${NATIVE_ARM_DOTPROD_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/public-raw-$<CONFIG>.json"
    $<TARGET_OBJECTS:native_test_arm_dotprod_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.dotprod.zero_overhead PROPERTIES
  LABELS "host;arm;dotprod;codegen")
