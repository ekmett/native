# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_arm_bf16_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  return()
endif()
set(interfaces module arm hub)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(interfaces module arm hub)
endif()
foreach(interface IN LISTS interfaces)
  set(target native_test_arm_bf16_${interface})
  add_executable(${target} ${interface}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.arm.bf16.${interface} COMMAND ${target})
  set_tests_properties(native.arm.bf16.${interface} PROPERTIES
    LABELS "host;arm;bf16;modules" SKIP_RETURN_CODE 77)
endforeach()
add_test(NAME native.arm.bf16.enhanced COMMAND native_test_arm_bf16_module enhanced)
set_tests_properties(native.arm.bf16.enhanced PROPERTIES
  LABELS "host;arm;bf16;enhanced" SKIP_RETURN_CODE 77)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

function(native_arm_bf16_baseline target)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2 /clang:-mcpu=generic /clang:-march=armv8-a+nobf16)
  else()
    target_compile_options(${target} PRIVATE -O2 -mcpu=generic -march=armv8-a+nobf16)
  endif()
endfunction()
add_library(native_test_arm_bf16_codegen OBJECT codegen.cc)
native_arm_bf16_baseline(native_test_arm_bf16_codegen)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(arm_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(arm_objdump NAMES llvm-objdump HINTS "${arm_compiler_dir}" REQUIRED)
add_test(NAME native.arm.bf16.codegen COMMAND "${Python3_EXECUTABLE}" -B
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${arm_objdump}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/bf16-$<CONFIG>.disassembly"
  $<TARGET_OBJECTS:native_test_arm_bf16_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.bf16.codegen PROPERTIES LABELS "host;arm;bf16;codegen")
foreach(kind RANGE 0 14)
  set(target native_test_arm_bf16_reject_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL reject.cc)
  native_arm_bf16_baseline(${target})
  target_compile_definitions(${target} PRIVATE NATIVE_BF16_REJECT_CASE=${kind})
  if(kind LESS 3)
    set(diagnostic "requires target feature|target specific option mismatch")
  else()
    set(diagnostic "call to deleted function")
  endif()
  add_test(NAME native.arm.bf16.reject_${kind} COMMAND "${Python3_EXECUTABLE}" -B
    "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fp16fml/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
    --config "$<CONFIG>" --diagnostic "${diagnostic}"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log")
  set_tests_properties(native.arm.bf16.reject_${kind} PROPERTIES
    LABELS "host;arm;bf16;negative" RUN_SERIAL TRUE)
endforeach()

add_test(NAME native.arm.bf16.endian COMMAND "${Python3_EXECUTABLE}" -B
  "${CMAKE_CURRENT_SOURCE_DIR}/check_endian.py"
  --compiler "${CMAKE_CXX_COMPILER}" --source "${CMAKE_CURRENT_SOURCE_DIR}/endian.cc"
  --include "${PROJECT_SOURCE_DIR}/src"
  --support "${CMAKE_CURRENT_SOURCE_DIR}/endian_support"
  --output "${CMAKE_CURRENT_BINARY_DIR}/endian-$<CONFIG>.s")
set_tests_properties(native.arm.bf16.endian PROPERTIES LABELS "host;arm;bf16;codegen")

add_test(NAME native.arm.bf16.reference COMMAND "${Python3_EXECUTABLE}" -B
  "${CMAKE_CURRENT_SOURCE_DIR}/check_reference.py")
set_tests_properties(native.arm.bf16.reference PROPERTIES LABELS "host;arm;bf16;reference")
