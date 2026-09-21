# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_arm_neon_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
add_executable(native_test_arm_neon main.cc)
target_link_libraries(native_test_arm_neon PRIVATE native::native)
set_target_properties(native_test_arm_neon PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.arm.neon COMMAND native_test_arm_neon)
set_tests_properties(native.arm.neon PROPERTIES SKIP_RETURN_CODE 77 LABELS "arm;neon;modules;constexpr")
add_executable(native_test_arm_neon_participation participation.cc)
target_link_libraries(native_test_arm_neon_participation PRIVATE native::native)
set_target_properties(native_test_arm_neon_participation PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.arm.neon.participation COMMAND native_test_arm_neon_participation)
set_tests_properties(native.arm.neon.participation PROPERTIES SKIP_RETURN_CODE 77 LABELS "arm;neon;modules")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()
add_library(native_test_arm_neon_codegen OBJECT codegen.cc)
target_link_libraries(native_test_arm_neon_codegen PRIVATE native::native)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_arm_neon_codegen PRIVATE /O2 /clang:-mcpu=generic /clang:-march=armv8-a)
else()
  target_compile_options(native_test_arm_neon_codegen PRIVATE -O2 -mcpu=generic -march=armv8-a)
endif()
set_target_properties(native_test_arm_neon_codegen PROPERTIES CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(neon_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(neon_objdump NAMES llvm-objdump HINTS "${neon_compiler_dir}" REQUIRED)
add_test(NAME native.arm.neon.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../arm_dotprod/check_equivalence.py"
  --objdump "${neon_objdump}" --output "${CMAKE_CURRENT_BINARY_DIR}/paired-$<CONFIG>.json"
  $<TARGET_OBJECTS:native_test_arm_neon_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.neon.codegen PROPERTIES LABELS "arm;neon;codegen")

foreach(kind IN ITEMS feature target shape)
  set(target native_test_arm_neon_reject_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL negative_${kind}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  if(kind STREQUAL "feature")
    set(diagnostic "call to consteval function.*not a constant expression")
  elseif(kind STREQUAL "target")
    set(diagnostic "requires target feature.*neon|target specific option mismatch")
  else()
    set(diagnostic "no matching function")
  endif()
  add_test(NAME native.arm.neon.reject_${kind} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fp16fml/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
    --config "$<CONFIG>" --diagnostic "${diagnostic}"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log")
  set_tests_properties(native.arm.neon.reject_${kind} PROPERTIES
    LABELS "arm;neon;negative" RUN_SERIAL TRUE)
endforeach()

add_test(NAME native.arm.neon.big_endian_codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_big_endian.py"
  --compiler "${CMAKE_CXX_COMPILER}"
  --header "${PROJECT_SOURCE_DIR}/src/native/arm/neon.h"
  --output-dir "${CMAKE_CURRENT_BINARY_DIR}/big-endian")
set_tests_properties(native.arm.neon.big_endian_codegen PROPERTIES LABELS "arm;neon;codegen;big-endian")

add_library(native_test_arm_neon_endian STATIC)
target_sources(native_test_arm_neon_endian PUBLIC FILE_SET CXX_MODULES FILES endian.ccm)
target_link_libraries(native_test_arm_neon_endian PUBLIC native::native)
add_executable(native_test_arm_neon_endian_positive endian.cc)
target_link_libraries(native_test_arm_neon_endian_positive PRIVATE native_test_arm_neon_endian)
set_target_properties(native_test_arm_neon_endian_positive PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.arm.neon.endian_constraints COMMAND native_test_arm_neon_endian_positive)
set_tests_properties(native.arm.neon.endian_constraints PROPERTIES LABELS "arm;neon;constexpr;big-endian")
foreach(op RANGE 0 11)
  set(target native_test_arm_neon_endian_reject_${op})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL endian.cc)
  target_link_libraries(${target} PRIVATE native_test_arm_neon_endian)
  target_compile_definitions(${target} PRIVATE NATIVE_NEON_ENDIAN_REJECT=${op})
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  add_test(NAME native.arm.neon.endian_reject_${op} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fp16fml/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
    --config "$<CONFIG>" --diagnostic "call to consteval function.*not a constant expression"
    --log "${CMAKE_CURRENT_BINARY_DIR}/endian-reject-${op}-$<CONFIG>.log")
  set_tests_properties(native.arm.neon.endian_reject_${op} PROPERTIES
    LABELS "arm;neon;negative;big-endian" RUN_SERIAL TRUE)
endforeach()
