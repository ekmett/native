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
