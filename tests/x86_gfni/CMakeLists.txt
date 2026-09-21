# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_gfni_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

set(gfni_interfaces module main)
foreach(interface IN LISTS gfni_interfaces)
  set(target native_test_x86_gfni_${interface})
  add_executable(${target} ${interface}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.gfni.${interface} COMMAND ${target})
  set_tests_properties(native.x86.gfni.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;gfni")
endforeach()

add_library(native_test_x86_gfni_baseline OBJECT baseline.cc)
target_link_libraries(native_test_x86_gfni_baseline PRIVATE native::native)
set_target_properties(native_test_x86_gfni_baseline PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(gfni_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_GFNI_OBJDUMP NAMES llvm-objdump
  HINTS "${gfni_compiler_directory}" REQUIRED)
add_test(NAME native.x86.gfni.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_GFNI_OBJDUMP}" --baseline-only
    --output "${CMAKE_CURRENT_BINARY_DIR}/gfni-baseline-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_gfni_baseline> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.gfni.baseline PROPERTIES LABELS "host;codegen;baseline;x86;gfni")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

# Optional instruction requirements come from function attributes in textual
# probes. Module consumers retain the provider baseline so their BMIs agree.
set(gfni_disabled_features -mno-gfni -mno-avx -mno-avx2 -mno-avx512f
  -mno-avx512bw -mno-avx512vl -mno-fma)
function(native_gfni_textual_probe target)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2)
    foreach(option IN LISTS gfni_disabled_features)
      target_compile_options(${target} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${target} PRIVATE -O2 ${gfni_disabled_features})
  endif()
endfunction()

add_library(native_test_x86_gfni_codegen OBJECT codegen.cc)
native_gfni_textual_probe(native_test_x86_gfni_codegen)
add_test(NAME native.x86.gfni.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_GFNI_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/gfni-codegen-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_gfni_codegen>
    $<TARGET_OBJECTS:native_test_x86_gfni_baseline> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.gfni.codegen PROPERTIES LABELS "host;codegen;baseline;x86;gfni")

foreach(mode IN ITEMS baseline enabled)
  set(target native_test_x86_gfni_metadata_${mode})
  add_library(${target} OBJECT metadata.cc)
  native_gfni_textual_probe(${target})
  if(mode STREQUAL "enabled")
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(${target} PRIVATE /clang:-mgfni)
    else()
      target_compile_options(${target} PRIVATE -mgfni)
    endif()
  endif()
endforeach()

set(gfni_negative_names
  feature128 feature256 feature512 feature_mask128_vl feature_mask256_bw feature_mask512_bw
  target128 target256 target512 target_mask128_vl target_mask512_bw immediate_range immediate_runtime)
set(gfni_negative_index 0)
foreach(kind IN LISTS gfni_negative_names)
  math(EXPR gfni_negative_index "${gfni_negative_index}+1")
  set(target native_test_x86_gfni_reject_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL negative.cc)
  native_gfni_textual_probe(${target})
  target_compile_definitions(${target} PRIVATE NATIVE_GFNI_REJECT=${gfni_negative_index})
  if(kind MATCHES "^target")
    set(diagnostic "requires target feature|target specific option mismatch")
  elseif(kind STREQUAL "immediate_runtime")
    set(diagnostic "invalid explicitly-specified argument|non-type template argument is not a constant expression")
  else()
    set(diagnostic "constraints not satisfied")
  endif()
  add_test(NAME native.x86.gfni.reject_${kind}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${target}
      --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/gfni-reject-${kind}-$<CONFIG>.log"
      --diagnostic "${diagnostic}")
  set_tests_properties(native.x86.gfni.reject_${kind} PROPERTIES
    RUN_SERIAL TRUE LABELS "host;compile;negative;baseline;x86;gfni" TIMEOUT 180)
endforeach()

# Public instruction bindings must preserve the same opcodes and immediates.
add_library(native_test_x86_gfni_codegen_public OBJECT codegen_public.cc)
target_link_libraries(native_test_x86_gfni_codegen_public PRIVATE native::native)
set_target_properties(native_test_x86_gfni_codegen_public PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_gfni_codegen_public PRIVATE /O2)
else()
  target_compile_options(native_test_x86_gfni_codegen_public PRIVATE -O2)
endif()
add_test(NAME native.x86.gfni.codegen_public
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_GFNI_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/gfni-public-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_gfni_codegen_public>
    $<TARGET_OBJECTS:native_test_x86_gfni_baseline>
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.gfni.codegen_public PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;gfni")

foreach(index RANGE 1 3)
  set(target native_test_x86_gfni_reject_public_${index})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_public.cc)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_definitions(${target} PRIVATE NATIVE_PUBLIC_REJECT=${index})
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.x86.gfni.reject_public_${index}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${target}
      --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-public-${index}-$<CONFIG>.log"
      --diagnostic "requires target feature")
  set_tests_properties(native.x86.gfni.reject_public_${index} PROPERTIES
    RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;modules;compile;negative;x86;gfni")
endforeach()

add_test(NAME native.x86.gfni.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_GFNI_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_gfni_codegen>
    --public $<TARGET_OBJECTS:native_test_x86_gfni_codegen_public>
    --output "${CMAKE_CURRENT_BINARY_DIR}/gfni-paired-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.gfni.codegen_pairs PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;gfni")
