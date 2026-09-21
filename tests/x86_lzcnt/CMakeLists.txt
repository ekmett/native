# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_lzcnt_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

set(interface_number 0)
set(interfaces header module hub)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Installed consumers see public modules, not private implementation headers.
  set(interface_number 1)
  set(interfaces module hub)
endif()
foreach(interface IN LISTS interfaces)
  set(target native_test_x86_lzcnt_${interface})
  add_executable(${target} main.cc)
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  if(interface STREQUAL "hub")
    target_link_libraries(${target} PRIVATE native::native)
  else()
    target_link_libraries(${target} PRIVATE native::minimal)
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.lzcnt.${interface} COMMAND ${target})
  set_tests_properties(native.x86.lzcnt.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;lzcnt")
  math(EXPR interface_number "${interface_number}+1")
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

add_library(native_test_x86_lzcnt_codegen OBJECT codegen.cc)
target_link_libraries(native_test_x86_lzcnt_codegen PRIVATE native::headers)
target_compile_features(native_test_x86_lzcnt_codegen PRIVATE cxx_std_26)
set_target_properties(native_test_x86_lzcnt_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
set(disabled_features -mno-popcnt -mno-lzcnt -mno-bmi -mno-bmi2
  -mno-avx -mno-avx2 -mno-avx512f -mno-fma)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_lzcnt_codegen PRIVATE /O2)
  foreach(option IN LISTS disabled_features)
    target_compile_options(native_test_x86_lzcnt_codegen PRIVATE "/clang:${option}")
  endforeach()
else()
  target_compile_options(native_test_x86_lzcnt_codegen PRIVATE -O2 ${disabled_features})
endif()
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/lzcnt-codegen-object.txt"
  CONTENT "$<TARGET_OBJECTS:native_test_x86_lzcnt_codegen>\n")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_LZCNT_OBJDUMP NAMES llvm-objdump
  HINTS "${compiler_directory}" REQUIRED)
add_test(NAME native.x86.lzcnt.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
    --objdump "${NATIVE_LZCNT_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/lzcnt-codegen.txt"
    $<TARGET_OBJECTS:native_test_x86_lzcnt_codegen>)
set_tests_properties(native.x86.lzcnt.codegen PROPERTIES
  LABELS "host;codegen;baseline;x86;lzcnt")
