# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(simd_kernel_integer_refinement LANGUAGES CXX)
  set(SIMD_BUILD_TESTS OFF CACHE BOOL "Build the focused fixture only" FORCE)
  add_subdirectory(../.. simd)
  enable_testing()
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(integer_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(integer_objdump NAMES llvm-objdump HINTS "${integer_compiler_dir}" REQUIRED)
add_test(NAME simd.kernel_integer.codegen_controls COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --self-test)
foreach(kind IN ITEMS header import)
  add_library(simd_kernel_integer_${kind} OBJECT main.cc)
  target_link_libraries(simd_kernel_integer_${kind} PRIVATE simd::simd)
  if(kind STREQUAL "import")
    target_compile_definitions(simd_kernel_integer_${kind} PRIVATE SIMD_TEST_IMPORT=1)
  endif()
  set_target_properties(simd_kernel_integer_${kind} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_executable(simd_test_kernel_integer_${kind} $<TARGET_OBJECTS:simd_kernel_integer_${kind}>)
  target_link_libraries(simd_test_kernel_integer_${kind} PRIVATE simd::simd)
  set_target_properties(simd_test_kernel_integer_${kind} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND simd_link_clang_cl_asan)
    simd_link_clang_cl_asan(simd_test_kernel_integer_${kind})
  endif()
  add_test(NAME simd.kernel_integer.${kind} COMMAND simd_test_kernel_integer_${kind})
  set_tests_properties(simd.kernel_integer.${kind} PROPERTIES LABELS "host;integer;modules;exact" TIMEOUT 120)
  # Instrumentation inserts calls/branches; inspect the ordinary object only.
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$" AND NOT SIMD_ENABLE_ASAN)
    add_test(NAME simd.kernel_integer.${kind}_codegen COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${integer_objdump}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/${kind}-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:simd_kernel_integer_${kind}> COMMAND_EXPAND_LISTS)
    set_tests_properties(simd.kernel_integer.${kind}_codegen PROPERTIES LABELS "host;integer;codegen" TIMEOUT 120)
  endif()
endforeach()
