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

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  add_test(NAME native.source_targets.platform COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/check_platform.py" --compiler "${CMAKE_CXX_COMPILER}"
    --include "${PROJECT_SOURCE_DIR}/src" --source "${CMAKE_CURRENT_SOURCE_DIR}/platform.cc"
    --output "${CMAKE_CURRENT_BINARY_DIR}/platform-$<CONFIG>.json")
  set_tests_properties(native.source_targets.platform PROPERTIES LABELS "host;baseline;source_targets")
endif()

# No per-variant compiler options, profile helpers, generated source, or extra BMI.
add_executable(native_test_source_metadata metadata.cc)
target_link_libraries(native_test_source_metadata PRIVATE native::native)
# Repeated global-module includes must preserve one property declaration for
# each ISA family, including foreign metadata reached through the full hub.
foreach(include_header RANGE 0 1)
  add_library(native_isa_properties_${include_header} OBJECT isa_properties.cc)
  target_link_libraries(native_isa_properties_${include_header} PRIVATE native::native)
  target_compile_definitions(native_isa_properties_${include_header} PRIVATE
    NATIVE_ISA_PROPERTIES_INCLUDE=${include_header})
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(native_isa_properties_${include_header} PRIVATE /WX)
  else()
    target_compile_options(native_isa_properties_${include_header} PRIVATE -Werror)
  endif()
endforeach()
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

foreach(kind IN ITEMS invalid foreign no_target)
  add_library(native_source_reject_${kind} OBJECT EXCLUDE_FROM_ALL ${kind}.cc)
  target_link_libraries(native_source_reject_${kind} PRIVATE native::native)
  if(kind STREQUAL "invalid" OR kind STREQUAL "foreign")
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
  native.source_targets.codegen native.source_targets.reject_invalid native.source_targets.reject_foreign native.source_targets.reject_no_target
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

# Import ISA metadata with independently configured consumer features. CMake may
# build a matching BMI when the compiler requires one for changed target options.
# These objects contain compile-time assertions only: higher ISA permissions do
# not require the test runner's CPU to implement those instructions.
add_executable(native_test_baseline baseline.cc)
target_link_libraries(native_test_baseline PRIVATE native::common)
target_compile_definitions(native_test_baseline PRIVATE NATIVE_BASELINE_CASE=0)
if(COMMAND native_link_clang_cl_asan)
  native_link_clang_cl_asan(native_test_baseline)
endif()
add_test(NAME native.source_targets.baseline COMMAND native_test_baseline)
set_tests_properties(native.source_targets.baseline PROPERTIES LABELS "host;modules;baseline;source_targets")

function(native_baseline_case name case)
  add_library(native_baseline_${name} OBJECT baseline.cc)
  target_link_libraries(native_baseline_${name} PRIVATE native::common)
  target_compile_definitions(native_baseline_${name} PRIVATE NATIVE_BASELINE_CASE=${case})
  set_target_properties(native_baseline_${name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  foreach(option IN LISTS ARGN)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(native_baseline_${name} PRIVATE "/clang:${option}")
    else()
      target_compile_options(native_baseline_${name} PRIVATE "${option}")
    endif()
  endforeach()
endfunction()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
  native_baseline_case(sse42_no_popcnt 1 -march=x86-64 -msse4.2 -mno-popcnt)
  native_baseline_case(crc32 2 -march=x86-64 -mcrc32)
  native_baseline_case(avx2 3 -march=x86-64 -mavx2 -mno-fma)
  native_baseline_case(unregistered 4 -march=x86-64 -mavx512vbmi)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  native_baseline_case(crc 5 -march=armv8-a+crc)
  native_baseline_case(aes 6 -march=armv8-a+aes)
  native_baseline_case(sha3 7 -march=armv8-a+sha3)
  native_baseline_case(scalar_fp16 8 -march=armv8-a+fp16+nosimd)
  if(WIN32)
    # The Windows ARM64 CRT includes NEON intrinsics even for ISA metadata.
    # Capture the real disabled-register macros before importing that metadata,
    # then check the expanded assertions with the ordinary Windows ABI.
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
      get_target_property(native_header_dirs native::headers INTERFACE_INCLUDE_DIRECTORIES)
      find_path(source_targets_include native/targets.h PATHS ${native_header_dirs}
        NO_DEFAULT_PATH NO_CACHE REQUIRED)
    else()
      set(source_targets_include "${PROJECT_SOURCE_DIR}/src")
    endif()
    set(general_regs_source "${CMAKE_CURRENT_BINARY_DIR}/baseline_general_regs.cc")
    add_custom_command(OUTPUT "${general_regs_source}"
      COMMAND "${CMAKE_COMMAND}"
        "-DCOMPILER=${CMAKE_CXX_COMPILER}"
        "-DCOMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}"
        "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/baseline.cc"
        "-DINCLUDE=${source_targets_include}"
        "-DOUTPUT=${general_regs_source}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/snapshot_general_regs.cmake"
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/baseline.cc" "${source_targets_include}/native/targets.h"
        "${source_targets_include}/native/config.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/snapshot_general_regs.cmake"
      VERBATIM)
    add_library(native_baseline_general_regs OBJECT "${general_regs_source}")
    target_link_libraries(native_baseline_general_regs PRIVATE native::common)
    set_target_properties(native_baseline_general_regs PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  else()
    native_baseline_case(general_regs 9 -march=armv8.3-a -mgeneral-regs-only)
  endif()
endif()
