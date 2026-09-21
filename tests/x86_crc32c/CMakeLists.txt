# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_crc32c_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_CRC32C_OBJDUMP NAMES llvm-objdump
  HINTS "${compiler_directory}" REQUIRED)
set(disabled_features -mno-crc32 -mno-sse4.2 -mno-sse4.1 -mno-ssse3 -mno-sse3
  -mno-popcnt -mno-lzcnt -mno-bmi -mno-bmi2 -mno-pclmul -mno-aes -mno-gfni
  -mno-avx -mno-avx2 -mno-avx512f -mno-fma)
set(interface_number 0)
set(interfaces header module hub x86)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Implementation headers remain private; installed consumers import modules.
  set(interface_number 1)
  set(interfaces module hub x86)
endif()
foreach(interface IN LISTS interfaces)
  if(interface STREQUAL "hub" OR interface STREQUAL "x86")
    set(provider native::native)
  else()
    set(provider native::minimal)
  endif()
  set(target native_test_x86_crc32c_${interface})
  add_executable(${target} main.cc)
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  target_link_libraries(${target} PRIVATE ${provider})
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.crc32c.${interface} COMMAND ${target})
  set_tests_properties(native.x86.crc32c.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;crc32c")

  set(codegen native_test_x86_crc32c_codegen_${interface})
  add_library(${codegen} OBJECT codegen.cc)
  target_compile_definitions(${codegen} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  set_target_properties(${codegen} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(interface STREQUAL "header")
    target_link_libraries(${codegen} PRIVATE native::headers)
    target_compile_features(${codegen} PRIVATE cxx_std_26)
    set_target_properties(${codegen} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(${codegen} PRIVATE /O2)
    else()
      target_compile_options(${codegen} PRIVATE -O2)
    endif()
    # The textual fixture overrides optional features even with a stronger
    # configured minimum. Module importers retain the provider's BMI flags.
    foreach(option IN LISTS disabled_features)
      if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        target_compile_options(${codegen} PRIVATE "/clang:${option}")
      else()
        target_compile_options(${codegen} PRIVATE "${option}")
      endif()
    endforeach()
  else()
    target_link_libraries(${codegen} PRIVATE ${provider})
  endif()
  add_test(NAME native.x86.crc32c.codegen_${interface}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
      --objdump "${NATIVE_CRC32C_OBJDUMP}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/crc32c-${interface}-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${codegen}> COMMAND_EXPAND_LISTS)
  set_tests_properties(native.x86.crc32c.codegen_${interface} PROPERTIES
    LABELS "host;codegen;baseline;x86;crc32c")

  foreach(kind IN ITEMS feature target)
    if(kind STREQUAL "feature")
      set(diagnostic "constraints not satisfied|no matching function")
    else()
      set(diagnostic "requires target feature 'crc32'|target specific option mismatch")
    endif()
    foreach(width IN ITEMS 8 16 32 64)
      set(negative native_crc32c_reject_${kind}_u${width}_${interface})
      add_library(${negative} OBJECT EXCLUDE_FROM_ALL negative.cc)
      target_link_libraries(${negative} PRIVATE ${provider})
      target_compile_definitions(${negative} PRIVATE
        NATIVE_TEST_INTERFACE=${interface_number} NATIVE_TEST_WIDTH=${width}
        NATIVE_TEST_NEGATIVE_FEATURE=$<STREQUAL:${kind},feature>)
      set_target_properties(${negative} PROPERTIES
        CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
      add_test(NAME native.x86.crc32c.reject_${kind}_u${width}_${interface}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_negative.py"
          --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
          --target "${negative}" --config "$<CONFIG>"
          --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-u${width}-${interface}-$<CONFIG>.log"
          --diagnostic "${diagnostic}")
      set_tests_properties(native.x86.crc32c.reject_${kind}_u${width}_${interface} PROPERTIES
        SKIP_RETURN_CODE 77 RUN_SERIAL TRUE TIMEOUT 180
        LABELS "host;negative;baseline;x86;crc32c")
    endforeach()
  endforeach()
  math(EXPR interface_number "${interface_number}+1")
endforeach()

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  foreach(mode RANGE 0 2)
    set(target native_test_x86_crc32c_minimum_${mode})
    add_library(${target} OBJECT minimum.cc)
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_MINIMUM=${mode})
    set_target_properties(${target} PROPERTIES
      CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
    set(options ${disabled_features})
    if(mode EQUAL 1)
      list(APPEND options -mcrc32)
    elseif(mode EQUAL 2)
      list(APPEND options -msse4.2)
    endif()
    foreach(option IN LISTS options)
      if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE "/clang:${option}")
      else()
        target_compile_options(${target} PRIVATE "${option}")
      endif()
    endforeach()
  endforeach()
endif()
