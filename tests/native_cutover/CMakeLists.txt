# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_cutover_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()

# Neither the detector nor the generic math entry points select a project ISA.
# The BMI2 calls in main.cc carry their own source-level target scope.
foreach(suite IN ITEMS main math platform)
  add_executable(native_test_cutover_${suite} ${suite}.cc)
  target_link_libraries(native_test_cutover_${suite} PRIVATE native::native)
  set_target_properties(native_test_cutover_${suite} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION FALSE CXX_SCAN_FOR_MODULES ON)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(native_test_cutover_${suite})
  endif()
  add_test(NAME native.cutover.${suite} COMMAND native_test_cutover_${suite})
  set_tests_properties(native.cutover.${suite} PROPERTIES LABELS "host;modules;baseline;cutover")
endforeach()

# Compile the README's complete example, including its runtime admission check.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64|aarch64|arm64|ARM64)$")
  set(readme "${CMAKE_CURRENT_SOURCE_DIR}/../../README.md")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${readme}")
  file(READ "${readme}" readme_text)
  string(REGEX MATCH "```cpp\n([^`]*)\n```" readme_example "${readme_text}")
  if(NOT readme_example)
    message(FATAL_ERROR "Cannot find the README C++ example")
  endif()
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/readme.cc" "${CMAKE_MATCH_1}\n")
  add_executable(native_test_cutover_readme "${CMAKE_CURRENT_BINARY_DIR}/readme.cc")
  target_link_libraries(native_test_cutover_readme PRIVATE native::native)
  set_target_properties(native_test_cutover_readme PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION FALSE CXX_SCAN_FOR_MODULES ON)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(native_test_cutover_readme)
  endif()
  add_test(NAME native.cutover.readme COMMAND native_test_cutover_readme)
  set_tests_properties(native.cutover.readme PROPERTIES LABELS "host;modules;baseline;cutover")
endif()
