# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_source_targets_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(source_targets_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(source_targets_objdump NAMES llvm-objdump HINTS "${source_targets_compiler_dir}" REQUIRED)

# No per-variant compiler options, profile helpers, generated source, or extra BMI.
add_executable(native_test_source_metadata metadata.cc)
target_link_libraries(native_test_source_metadata PRIVATE native::native)
add_library(native_source_variants OBJECT macros.cc scalar.cc)
target_link_libraries(native_source_variants PRIVATE native::native)
add_executable(native_test_source_variants $<TARGET_OBJECTS:native_source_variants>)
target_link_libraries(native_test_source_variants PRIVATE native::native)
foreach(target IN ITEMS native_test_source_metadata native_source_variants native_test_source_variants)
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
endforeach()
# Runtime linkage and DLL deployment belong to the final executables. Object
# libraries have no link step and cannot carry a POST_BUILD deployment command.
foreach(target IN ITEMS native_test_source_metadata native_test_source_variants)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
endforeach()
add_test(NAME native.source_targets.metadata COMMAND native_test_source_metadata)
add_test(NAME native.source_targets.native COMMAND native_test_source_variants)
set_tests_properties(native.source_targets.native PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME native.source_targets.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${source_targets_objdump}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/variants-$<CONFIG>.disassembly"
  $<TARGET_OBJECTS:native_source_variants> COMMAND_EXPAND_LISTS)

foreach(kind IN ITEMS invalid no_target)
  add_library(native_source_reject_${kind} OBJECT EXCLUDE_FROM_ALL ${kind}.cc)
  target_link_libraries(native_source_reject_${kind} PRIVATE native::native)
  if(kind STREQUAL "invalid")
    set(diagnostic "source target contains an unregistered ISA feature")
  else()
    set(diagnostic "requires target feature|target specific option mismatch")
  endif()
  add_test(NAME native.source_targets.reject_${kind} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py" --cmake "${CMAKE_COMMAND}"
    --build "${CMAKE_BINARY_DIR}" --target native_source_reject_${kind} --config "$<CONFIG>"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log" --diagnostic "${diagnostic}")
  set_tests_properties(native.source_targets.reject_${kind} PROPERTIES SKIP_RETURN_CODE 77 RUN_SERIAL TRUE)
endforeach()
set_tests_properties(native.source_targets.metadata native.source_targets.native
  native.source_targets.codegen native.source_targets.reject_invalid native.source_targets.reject_no_target
  PROPERTIES LABELS "host;modules;source_targets" TIMEOUT 180)

# Import only the capability module; admission must not require the SIMD hub.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  add_executable(native_test_arm_admission arm.cc)
  target_link_libraries(native_test_arm_admission PRIVATE native::common)
  set_target_properties(native_test_arm_admission PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(native_test_arm_admission)
  endif()
  add_test(NAME native.arm.features.admission COMMAND native_test_arm_admission)
  set_tests_properties(native.arm.features.admission PROPERTIES LABELS "host;modules;baseline;source_targets")
endif()

# The public CPU umbrella is usable without linking the vector hub.
add_executable(native_test_cpu_import cpu.cc)
target_link_libraries(native_test_cpu_import PRIVATE native::common)
set_target_properties(native_test_cpu_import PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
if(COMMAND native_link_clang_cl_asan)
  native_link_clang_cl_asan(native_test_cpu_import)
endif()
add_test(NAME native.features.import COMMAND native_test_cpu_import)
set_tests_properties(native.features.import PROPERTIES LABELS "host;modules;baseline;source_targets")
