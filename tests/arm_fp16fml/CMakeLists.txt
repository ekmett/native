# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_arm_fp16fml_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  return()
endif()
set(suites arm_hub module main)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Installed consumers use the public modules.
  set(suites module main)
endif()
foreach(suite IN LISTS suites)
  set(target native_test_arm_fp16fml_${suite})
  add_executable(${target} ${suite}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.arm.fp16fml.${suite} COMMAND ${target})
  set_tests_properties(native.arm.fp16fml.${suite} PROPERTIES
    LABELS "host;modules;arm;fp16fml" SKIP_RETURN_CODE 77)
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

add_library(native_test_arm_fp16fml_codegen OBJECT codegen.cc)
target_link_libraries(native_test_arm_fp16fml_codegen PRIVATE native::native)
target_compile_features(native_test_arm_fp16fml_codegen PRIVATE cxx_std_26)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_arm_fp16fml_codegen PRIVATE /O2 /clang:-march=armv8-a)
else()
  target_compile_options(native_test_arm_fp16fml_codegen PRIVATE -O2 -march=armv8-a)
endif()
set_target_properties(native_test_arm_fp16fml_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(arm_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(arm_objdump NAMES llvm-objdump HINTS "${arm_compiler_dir}" REQUIRED)
add_test(NAME native.arm.fp16fml.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${arm_objdump}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/fp16fml-$<CONFIG>.disassembly"
  $<TARGET_OBJECTS:native_test_arm_fp16fml_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.fp16fml.codegen PROPERTIES LABELS "host;arm;fp16fml;codegen")

foreach(kind IN ITEMS no_target)
  set(target native_test_arm_fp16fml_reject_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL ${kind}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_features(${target} PRIVATE cxx_std_26)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2 /clang:-march=armv8-a)
  else()
    target_compile_options(${target} PRIVATE -O2 -march=armv8-a)
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(kind MATCHES "target")
    set(diagnostic "requires target feature|target specific option mismatch")
  else()
    set(diagnostic "call to deleted function")
  endif()
  add_test(NAME native.arm.fp16fml.reject_${kind} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fp16fml/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
    --config "$<CONFIG>" --diagnostic "${diagnostic}"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log")
  set_tests_properties(native.arm.fp16fml.reject_${kind} PROPERTIES
    LABELS "host;arm;fp16fml;negative" RUN_SERIAL TRUE)
endforeach()

add_test(NAME native.arm.fp16fml.endian COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fcma/check_endian.py"
  --compiler "${CMAKE_CXX_COMPILER}" --source "${CMAKE_CURRENT_SOURCE_DIR}/endian.cc"
  --include "${PROJECT_SOURCE_DIR}/src"
  --support "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fcma/endian_support"
  --output "${CMAKE_CURRENT_BINARY_DIR}/endian-$<CONFIG>.s")
set_tests_properties(native.arm.fp16fml.endian PROPERTIES LABELS "host;arm;fp16fml;codegen")

# The public abstraction must add no instructions relative to the native helper.
add_test(NAME native.arm.fp16fml.zero_overhead
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../arm_dotprod/check_equivalence.py"
    --objdump "${arm_objdump}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/public-raw-$<CONFIG>.json"
    $<TARGET_OBJECTS:native_test_arm_fp16fml_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.fp16fml.zero_overhead PROPERTIES
  LABELS "host;arm;fp16fml;codegen")
