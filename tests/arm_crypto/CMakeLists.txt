# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_arm_crypto_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  return()
endif()
set(suites module arm main constexpr)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(suites module arm main constexpr)
endif()
foreach(suite IN LISTS suites)
  set(target native_test_arm_crypto_${suite})
  add_executable(${target} ${suite}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.arm.crypto.${suite} COMMAND ${target})
  set_tests_properties(native.arm.crypto.${suite} PROPERTIES
    LABELS "host;modules;arm;crypto" SKIP_RETURN_CODE 77)
endforeach()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

function(crypto_baseline target)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_features(${target} PRIVATE cxx_std_26)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2 /clang:-march=armv8-a)
  else()
    target_compile_options(${target} PRIVATE -O2 -march=armv8-a)
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
endfunction()
add_library(native_test_arm_crypto_codegen OBJECT codegen.cc)
crypto_baseline(native_test_arm_crypto_codegen)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(arm_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(arm_objdump NAMES llvm-objdump HINTS "${arm_compiler_dir}" REQUIRED)
add_test(NAME native.arm.crypto.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${arm_objdump}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/crypto-$<CONFIG>.disassembly"
  $<TARGET_OBJECTS:native_test_arm_crypto_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.crypto.codegen PROPERTIES LABELS "host;arm;crypto;codegen")

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/negatives.json")
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/negatives.json" negative_cases)
string(JSON negative_count LENGTH "${negative_cases}")
math(EXPR negative_last "${negative_count} - 1")
foreach(index RANGE ${negative_last})
  string(JSON kind MEMBER "${negative_cases}" ${index})
  string(JSON diagnostic GET "${negative_cases}" "${kind}")
  set(target native_test_arm_crypto_reject_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL ${kind}.cc)
  crypto_baseline(${target})
  add_test(NAME native.arm.crypto.reject_${kind} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fp16fml/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
    --config "$<CONFIG>" --diagnostic "${diagnostic}"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log")
  set_tests_properties(native.arm.crypto.reject_${kind} PROPERTIES
    LABELS "host;arm;crypto;negative" RUN_SERIAL TRUE)
endforeach()

# Every immediate-only vector overload must reject runtime operands, even
# though its call expression is visible to an unevaluated requires expression.
foreach(kind RANGE 0 35)
  set(target native_test_arm_crypto_constexpr_reject_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL constexpr_reject.cc)
  crypto_baseline(${target})
  target_compile_definitions(${target} PRIVATE NATIVE_ARM_CRYPTO_REJECT=${kind})
  add_test(NAME native.arm.crypto.constexpr_reject_${kind} COMMAND "${Python3_EXECUTABLE}"
    "${PROJECT_SOURCE_DIR}/tests/source_targets/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
    --config "$<CONFIG>" --diagnostic "call to consteval function.*is not a constant expression"
    --log "${CMAKE_CURRENT_BINARY_DIR}/constexpr-reject-${kind}-$<CONFIG>.log")
  set_tests_properties(native.arm.crypto.constexpr_reject_${kind} PROPERTIES
    LABELS "host;arm;crypto;constexpr;negative" RUN_SERIAL TRUE)
endforeach()
