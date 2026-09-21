# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_arm_rdm_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
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
  set(target native_test_arm_rdm_${interface})
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
  add_test(NAME native.arm.rdm.${interface} COMMAND ${target})
  set_tests_properties(native.arm.rdm.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;arm;rdm")
  math(EXPR interface_number "${interface_number}+1")
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

function(native_arm_rdm_baseline target)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
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

add_library(native_test_arm_rdm_codegen OBJECT codegen.cc)
native_arm_rdm_baseline(native_test_arm_rdm_codegen)
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/rdm-codegen-object.txt"
  CONTENT "$<TARGET_OBJECTS:native_test_arm_rdm_codegen>\n")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_ARM_RDM_OBJDUMP NAMES llvm-objdump
  HINTS "${compiler_directory}" REQUIRED)
add_test(NAME native.arm.rdm.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
    --objdump "${NATIVE_ARM_RDM_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/rdm-codegen.txt"
    $<TARGET_OBJECTS:native_test_arm_rdm_codegen>)
set_tests_properties(native.arm.rdm.codegen PROPERTIES
  LABELS "host;codegen;baseline;arm;rdm")
# Let CMake preserve the platform compiler command, quoting and driver flags.
foreach(kind IN ITEMS target lane)
  if(kind STREQUAL "lane")
    set(source negative_lane.cc)
    set(diagnostic "deleted function|no matching function")
  else()
    set(source negative.cc)
    set(diagnostic "requires target feature.*rdm|target specific option mismatch")
  endif()
  set(target native_test_arm_rdm_negative_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL ${source})
  native_arm_rdm_baseline(${target})
  add_test(NAME native.arm.rdm.negative_${kind}
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/tests/source_targets/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
      --config "$<CONFIG>" --diagnostic "${diagnostic}"
      --log "${CMAKE_CURRENT_BINARY_DIR}/negative-${kind}.txt")
  set_tests_properties(native.arm.rdm.negative_${kind} PROPERTIES
    RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;compile;baseline;arm;rdm")
endforeach()

add_test(NAME native.arm.rdm.big_endian_codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_big_endian.py"
    --compiler "${CMAKE_CXX_COMPILER}"
    --header "${PROJECT_SOURCE_DIR}/src/native/arm/rdm.h"
    --source "${CMAKE_CURRENT_LIST_DIR}/codegen.cc"
    --output-dir "${CMAKE_CURRENT_BINARY_DIR}/big-endian")
set_tests_properties(native.arm.rdm.big_endian_codegen PROPERTIES
  LABELS "host;codegen;cross-compile;arm;rdm")
