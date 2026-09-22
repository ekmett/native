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

foreach(kind IN ITEMS header import)
  set(target native_test_arm_conversions_${kind})
  add_executable(${target} conversions.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  if(kind STREQUAL "header")
    target_compile_definitions(${target} PRIVATE NATIVE_CONVERSION_HEADER=1)
  endif()
  add_test(NAME native.arm.neon.conversions.${kind} COMMAND ${target})
  set_tests_properties(native.arm.neon.conversions.${kind} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "arm;neon;modules;constexpr;math")
endforeach()

foreach(kind IN ITEMS header import)
  set(target native_test_arm_comparisons_${kind})
  add_executable(${target} comparisons.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(kind STREQUAL "header")
    target_compile_definitions(${target} PRIVATE NATIVE_COMPARISON_HEADER=1)
  endif()
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2 /fp:strict)
  else()
    target_compile_options(${target} PRIVATE
      -O2 -Wall -Wextra -Werror -fno-fast-math -ffp-contract=off -frounding-math)
  endif()
  add_test(NAME native.arm.neon.comparisons.${kind} COMMAND ${target})
  set_tests_properties(native.arm.neon.comparisons.${kind} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "arm;neon;modules;constexpr;rounding;exact")
endforeach()


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
foreach(kind IN ITEMS header import)
  foreach(policy IN ITEMS ordinary rounding)
    set(target native_test_arm_comparisons_codegen_${kind}_${policy})
    add_library(${target} OBJECT comparisons_codegen.cc)
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES
      CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
    if(kind STREQUAL "header")
      target_compile_definitions(${target} PRIVATE NATIVE_COMPARISON_HEADER=1)
    endif()
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(${target} PRIVATE /O2)
      if(policy STREQUAL "rounding")
        target_compile_options(${target} PRIVATE /fp:strict)
      else()
        target_compile_options(${target} PRIVATE /fp:precise)
      endif()
    else()
      target_compile_options(${target} PRIVATE -O2 -fno-fast-math -ffp-contract=off)
      if(policy STREQUAL "rounding")
        target_compile_options(${target} PRIVATE -frounding-math)
      else()
        target_compile_options(${target} PRIVATE -fno-rounding-math)
      endif()
    endif()
    set(comparison_half_args)
    if(kind STREQUAL "import")
      list(APPEND comparison_half_args --half)
    endif()
    add_test(NAME native.arm.neon.comparisons.codegen.${kind}.${policy}
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_comparisons.py"
      --objdump "${neon_objdump}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/comparisons-${kind}-${policy}-$<CONFIG>.json"
      ${comparison_half_args} $<TARGET_OBJECTS:${target}> COMMAND_EXPAND_LISTS)
    set_tests_properties(native.arm.neon.comparisons.codegen.${kind}.${policy} PROPERTIES
      LABELS "arm;neon;modules;codegen;rounding")
  endforeach()
endforeach()

add_test(NAME native.arm.neon.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../arm_dotprod/check_equivalence.py"
  --objdump "${neon_objdump}" --output "${CMAKE_CURRENT_BINARY_DIR}/paired-$<CONFIG>.json"
  $<TARGET_OBJECTS:native_test_arm_neon_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.neon.codegen PROPERTIES LABELS "arm;neon;codegen")

add_library(native_test_arm_conversions_codegen OBJECT conversions_codegen.cc)
target_link_libraries(native_test_arm_conversions_codegen PRIVATE native::native)
set_target_properties(native_test_arm_conversions_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_arm_conversions_codegen PRIVATE /O2)
else()
  target_compile_options(native_test_arm_conversions_codegen PRIVATE -O2)
endif()
add_test(NAME native.arm.neon.conversions.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../arm_dotprod/check_equivalence.py"
  --objdump "${neon_objdump}" --output "${CMAKE_CURRENT_BINARY_DIR}/conversions-$<CONFIG>.json"
  $<TARGET_OBJECTS:native_test_arm_conversions_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.arm.neon.conversions.codegen PROPERTIES LABELS "arm;neon;codegen")

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

add_executable(native_test_arm_neon_shapes shapes.cc)
target_link_libraries(native_test_arm_neon_shapes PRIVATE native::native)
set_target_properties(native_test_arm_neon_shapes PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.arm.neon.shapes COMMAND native_test_arm_neon_shapes)
set_tests_properties(native.arm.neon.shapes PROPERTIES LABELS "arm;neon;constexpr")
