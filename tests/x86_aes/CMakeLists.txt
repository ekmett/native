# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_aes_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

foreach(interface IN ITEMS module main)
  set(target native_test_x86_aes_${interface})
  add_executable(${target} ${interface}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.aes.${interface} COMMAND ${target})
  set_tests_properties(native.x86.aes.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;aes")
endforeach()

add_library(native_test_x86_aes_baseline OBJECT baseline.cc)
target_link_libraries(native_test_x86_aes_baseline PRIVATE native::native)
set_target_properties(native_test_x86_aes_baseline PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
