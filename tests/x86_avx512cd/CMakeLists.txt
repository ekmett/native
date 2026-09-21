# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_avx512cd_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

set(interface_number 1)
foreach(interface IN ITEMS module hub)
  set(target native_test_x86_avx512cd_${interface})
  add_executable(${target} main.cc)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  # The complete lane/mask banks exceed Clang's default constexpr step budget.
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /clang:-fconstexpr-steps=8000000)
  else()
    target_compile_options(${target} PRIVATE -fconstexpr-steps=8000000)
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.avx512cd.${interface} COMMAND ${target})
  set_tests_properties(native.x86.avx512cd.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;avx512cd")
  math(EXPR interface_number "${interface_number}+1")
endforeach()

# Raw-register helpers are private implementation inputs; installed consumers
# exercise only the public module and hub above.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

add_library(native_test_x86_avx512cd_codegen OBJECT codegen.cc)
target_link_libraries(native_test_x86_avx512cd_codegen PRIVATE native::headers)
target_compile_features(native_test_x86_avx512cd_codegen PRIVATE cxx_std_26)
set_target_properties(native_test_x86_avx512cd_codegen PROPERTIES CXX_SCAN_FOR_MODULES OFF)

add_library(native_test_x86_avx512cd_codegen_public OBJECT codegen_public.cc)
target_link_libraries(native_test_x86_avx512cd_codegen_public PRIVATE native::native)
set_target_properties(native_test_x86_avx512cd_codegen_public PROPERTIES CXX_SCAN_FOR_MODULES ON)

foreach(target IN ITEMS native_test_x86_avx512cd_codegen native_test_x86_avx512cd_codegen_public)
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2)
  else()
    target_compile_options(${target} PRIVATE -O2)
  endif()
endforeach()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(native_avx512cd_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_AVX512CD_OBJDUMP NAMES llvm-objdump
  HINTS "${native_avx512cd_compiler_dir}" REQUIRED)
add_test(NAME native.x86.avx512cd.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_AVX512CD_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_avx512cd_codegen>
    --public $<TARGET_OBJECTS:native_test_x86_avx512cd_codegen_public>
    --expected 37
    --output "${CMAKE_CURRENT_BINARY_DIR}/avx512cd-paired-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.avx512cd.codegen_pairs PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;avx512cd")
