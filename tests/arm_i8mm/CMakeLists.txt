# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_arm_i8mm_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  return()
endif()

set(suites arm_hub module main)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Installed consumers use public modules, not private implementation headers.
  set(suites module main)
endif()
foreach(suite IN LISTS suites)
  set(target native_test_arm_i8mm_${suite})
  add_executable(${target} ${suite}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.arm.i8mm.${suite} COMMAND ${target})
  set_tests_properties(native.arm.i8mm.${suite} PROPERTIES LABELS "host;modules;baseline;i8mm" SKIP_RETURN_CODE 77)
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(i8mm_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(i8mm_objdump NAMES llvm-objdump HINTS "${i8mm_compiler_dir}" REQUIRED)
foreach(kind IN ITEMS codegen minimum no_feature no_target bad_lane)
  if(kind MATCHES "^(codegen|minimum)$")
    add_library(native_test_arm_i8mm_${kind} OBJECT ${kind}.cc)
  else()
    add_library(native_test_arm_i8mm_${kind} OBJECT EXCLUDE_FROM_ALL negative.cc)
    string(TOUPPER "${kind}" definition)
    target_compile_definitions(native_test_arm_i8mm_${kind} PRIVATE TEST_${definition})
    if(kind STREQUAL "no_target")
      set(diagnostic "requires target feature|target specific option mismatch")
    else()
      set(diagnostic "deleted function|constraints not satisfied|no matching function")
    endif()
    add_test(NAME native.arm.i8mm.reject_${kind} COMMAND "${Python3_EXECUTABLE}"
      "${PROJECT_SOURCE_DIR}/tests/source_targets/check_negative.py" --cmake "${CMAKE_COMMAND}"
      --build "${CMAKE_BINARY_DIR}" --target native_test_arm_i8mm_${kind} --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log" --diagnostic "${diagnostic}")
    set_tests_properties(native.arm.i8mm.reject_${kind} PROPERTIES RUN_SERIAL TRUE LABELS "host;i8mm;negative")
  endif()
  target_link_libraries(native_test_arm_i8mm_${kind} PRIVATE native::native)
  target_compile_features(native_test_arm_i8mm_${kind} PRIVATE cxx_std_26)
  set_target_properties(native_test_arm_i8mm_${kind} PROPERTIES CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(kind STREQUAL "minimum")
    set(arch_flag -march=armv8-a+i8mm)
  else()
    set(arch_flag -march=armv8-a)
  endif()
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(native_test_arm_i8mm_${kind} PRIVATE /clang:${arch_flag} /O2)
  else()
    target_compile_options(native_test_arm_i8mm_${kind} PRIVATE ${arch_flag} -O2)
  endif()
endforeach()
add_test(NAME native.arm.i8mm.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${i8mm_objdump}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/i8mm-$<CONFIG>.disassembly"
  $<TARGET_OBJECTS:native_test_arm_i8mm_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.i8mm.codegen PROPERTIES LABELS "host;i8mm;codegen")

add_test(NAME native.arm.i8mm.zero_overhead
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../arm_dotprod/check_equivalence.py"
    --objdump "${i8mm_objdump}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/public-raw-$<CONFIG>.json"
    $<TARGET_OBJECTS:native_test_arm_i8mm_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.i8mm.zero_overhead PROPERTIES
  LABELS "host;arm;i8mm;codegen")
