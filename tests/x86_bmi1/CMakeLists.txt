# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
foreach(suite IN ITEMS header import hub)
  set(target native_test_x86_bmi1_${suite})
  add_executable(${target} main.cc)
  if(suite STREQUAL "hub")
    target_link_libraries(${target} PRIVATE native::native)
    target_compile_definitions(${target} PRIVATE NATIVE_BMI1_TEST_HUB=1)
  else()
    target_link_libraries(${target} PRIVATE native::minimal)
    if(suite STREQUAL "header")
      target_compile_definitions(${target} PRIVATE NATIVE_BMI1_TEST_HEADER=1)
    endif()
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.bmi1.${suite} COMMAND ${target})
  set_tests_properties(native.x86.bmi1.${suite} PROPERTIES
    LABELS "host;modules;baseline;bmi1" SKIP_RETURN_CODE 77)
endforeach()

# Runtime operands keep each operation observable for the disassembly check.
add_library(native_test_x86_bmi1_codegen OBJECT codegen.cc)
target_link_libraries(native_test_x86_bmi1_codegen PRIVATE native::minimal)
set_target_properties(native_test_x86_bmi1_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_bmi1_codegen PRIVATE /O2)
else()
  target_compile_options(native_test_x86_bmi1_codegen PRIVATE -O2)
endif()
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bmi1-codegen-object.txt"
  CONTENT "$<TARGET_OBJECTS:native_test_x86_bmi1_codegen>\n")
get_filename_component(bmi1_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_BMI1_OBJDUMP NAMES llvm-objdump HINTS "${bmi1_compiler_directory}" REQUIRED)
add_test(NAME native.x86.bmi1.codegen COMMAND "${CMAKE_COMMAND}"
  "-DOBJDUMP=${NATIVE_BMI1_OBJDUMP}" "-DOBJECT=$<TARGET_OBJECTS:native_test_x86_bmi1_codegen>"
  "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/bmi1-$<CONFIG>.disassembly"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/codegen.cmake")
set_tests_properties(native.x86.bmi1.codegen PROPERTIES LABELS "host;baseline;bmi1;codegen")

foreach(suite IN ITEMS header import hub codegen)
  set(target native_test_x86_bmi1_${suite})
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE
      /clang:-mno-bmi /clang:-mno-bmi2 /clang:-mno-avx /clang:-mno-avx2 /clang:-mno-fma)
  else()
    target_compile_options(${target} PRIVATE -mno-bmi -mno-bmi2 -mno-avx -mno-avx2 -mno-fma)
  endif()
endforeach()
