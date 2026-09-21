# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_floating_constexpr_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(families fp16fml fcma bf16)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  set(families f16c)
else()
  return()
endif()
foreach(family IN LISTS families)
  set(target native_constexpr_${family})
  add_executable(${target} ${family}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    # The driver must remain baseline-safe even when the compiler defaults to a
    # newer CPU; each runtime leaf owns its instruction target explicitly.
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(${target} PRIVATE /clang:-march=armv8-a)
    else()
      target_compile_options(${target} PRIVATE -march=armv8-a)
    endif()
  endif()
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON
    INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.constexpr.${family} COMMAND ${target})
  set_tests_properties(native.constexpr.${family} PROPERTIES SKIP_RETURN_CODE 77
    LABELS "host;modules;constexpr;property")
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  foreach(kind RANGE 0 19)
    set(target native_constexpr_float_reject_${kind})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL negative.cc)
    target_link_libraries(${target} PRIVATE native::native)
    target_compile_definitions(${target} PRIVATE NATIVE_FLOAT_REJECT=${kind})
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON
      INTERPROCEDURAL_OPTIMIZATION FALSE)
    add_test(NAME native.constexpr.float.reject_${kind}
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../arm_fp16fml/check_negative.py"
        --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${target}"
        --config "$<CONFIG>" --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log"
        --diagnostic "call to consteval function.*not a constant expression")
    set_tests_properties(native.constexpr.float.reject_${kind} PROPERTIES RUN_SERIAL TRUE
      LABELS "host;modules;constexpr;negative")
  endforeach()
endif()
