# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_aes_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

function(native_aes_module_target target)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
endfunction()

function(native_aes_optimize target)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2)
  else()
    target_compile_options(${target} PRIVATE -O2)
  endif()
endfunction()

add_library(native_test_x86_aes_baseline OBJECT baseline.cc)
native_aes_module_target(native_test_x86_aes_baseline)
native_aes_optimize(native_test_x86_aes_baseline)

foreach(interface IN ITEMS module main)
  set(target native_test_x86_aes_${interface})
  add_executable(${target} ${interface}.cc $<TARGET_OBJECTS:native_test_x86_aes_baseline>)
  native_aes_module_target(${target})
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.aes.${interface} COMMAND ${target})
  set_tests_properties(native.x86.aes.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;aes")
endforeach()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(aes_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_AES_OBJDUMP NAMES llvm-objdump
  HINTS "${aes_compiler_directory}" REQUIRED)
add_test(NAME native.x86.aes.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_AES_OBJDUMP}"
    --baseline $<TARGET_OBJECTS:native_test_x86_aes_baseline>
    --output "${CMAKE_CURRENT_BINARY_DIR}/aes-baseline-$<CONFIG>.disassembly"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.aes.baseline PROPERTIES LABELS "host;codegen;baseline;x86;aes")

# Private register helpers are available only in the source tree. Installed
# consumers retain the public module, runtime and baseline-admission checks.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

function(native_aes_textual_target target)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  native_aes_optimize(${target})
  set(disabled_features -mno-aes -mno-vaes -mno-avx -mno-avx2 -mno-avx512f -mno-fma)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    foreach(option IN LISTS disabled_features)
      target_compile_options(${target} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${target} PRIVATE ${disabled_features})
  endif()
endfunction()

add_library(native_test_x86_aes_codegen OBJECT codegen.cc)
native_aes_textual_target(native_test_x86_aes_codegen)
add_library(native_test_x86_aes_codegen_public OBJECT codegen_public.cc)
native_aes_module_target(native_test_x86_aes_codegen_public)
native_aes_optimize(native_test_x86_aes_codegen_public)
add_test(NAME native.x86.aes.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_AES_OBJDUMP}"
    --baseline $<TARGET_OBJECTS:native_test_x86_aes_baseline>
    --raw $<TARGET_OBJECTS:native_test_x86_aes_codegen>
    --public $<TARGET_OBJECTS:native_test_x86_aes_codegen_public>
    --output "${CMAKE_CURRENT_BINARY_DIR}/aes-codegen-$<CONFIG>.disassembly"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.aes.codegen PROPERTIES LABELS "host;modules;codegen;baseline;x86;aes")
add_test(NAME native.x86.aes.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_AES_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_aes_codegen>
    --public $<TARGET_OBJECTS:native_test_x86_aes_codegen_public>
    --expected 8
    --output "${CMAKE_CURRENT_BINARY_DIR}/aes-paired-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.aes.codegen_pairs PROPERTIES LABELS "host;modules;codegen;baseline;x86;aes")

function(native_aes_rejection name kind operation diagnostic)
  foreach(control IN ITEMS positive reject)
    set(target native_test_x86_aes_${control}_${name})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL reject.cc)
    if(kind EQUAL 2)
      native_aes_textual_target(${target})
    else()
      native_aes_module_target(${target})
      native_aes_optimize(${target})
    endif()
    target_compile_definitions(${target} PRIVATE
      NATIVE_AES_CASE=${kind} NATIVE_AES_OPERATION=${operation})
    if(control STREQUAL "positive")
      target_compile_definitions(${target} PRIVATE NATIVE_AES_POSITIVE=1)
    endif()
  endforeach()
  add_test(NAME native.x86.aes.reject_${name}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --positive native_test_x86_aes_positive_${name}
      --negative native_test_x86_aes_reject_${name}
      --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/aes-reject-${name}-$<CONFIG>.log"
      --diagnostic "${diagnostic}")
  set_tests_properties(native.x86.aes.reject_${name} PROPERTIES
    RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;compile;negative;baseline;x86;aes")
endfunction()

# Clang may stop code generation at the first target mismatch. Compile each
# operation separately, with the same source also required to pass its control.
set(aes_operations aesenc aesenclast aesdec aesdeclast aesimc aeskeygenassist)
set(aes_operation_index 0)
foreach(operation IN LISTS aes_operations)
  native_aes_rejection(target_${operation} 1 ${aes_operation_index}
    "requires target feature|target specific option mismatch")
  native_aes_rejection(feature_${operation} 2 ${aes_operation_index}
    "call to deleted function|constraints not satisfied")
  native_aes_rejection(weak_runtime_${operation} 3 ${aes_operation_index}
    "call to consteval function.*is not a constant expression|immediate function.*is not a constant expression")
  native_aes_rejection(raw_shape_${operation} 4 ${aes_operation_index}
    "call to deleted function")
  math(EXPR aes_operation_index "${aes_operation_index}+1")
endforeach()
native_aes_rejection(float_shape 5 0 "call to deleted function")
native_aes_rejection(integer_shape 6 0 "call to deleted function")
native_aes_rejection(mixed_tag 7 0 "call to deleted function")
native_aes_rejection(immediate_range 8 5 "call to deleted function|constraints not satisfied")
native_aes_rejection(immediate_negative 9 5
  "invalid explicitly-specified argument|cannot be narrowed")
native_aes_rejection(immediate_overflow 10 5
  "invalid explicitly-specified argument|cannot be narrowed")
native_aes_rejection(immediate_runtime 11 5
  "invalid explicitly-specified argument|non-type template argument is not a constant expression")
