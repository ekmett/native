# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()
add_executable(native_test_polyfill_x86_scalar main.cc)
target_link_libraries(native_test_polyfill_x86_scalar PRIVATE native::minimal)
target_compile_options(native_test_polyfill_x86_scalar PRIVATE
  "$<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/clang:>-march=x86-64"
  "$<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/clang:>-mno-bmi"
  "$<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/clang:>-mno-bmi2"
  "$<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/clang:>-mno-popcnt"
  "$<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/clang:>-mno-lzcnt"
  "$<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/clang:>-mno-crc32"
  "$<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/clang:>-mno-adx")
add_test(NAME native.polyfill.x86.scalar COMMAND native_test_polyfill_x86_scalar)
set_tests_properties(native.polyfill.x86.scalar PROPERTIES LABELS "host;polyfill;scalar;x86" TIMEOUT 30)

foreach(mode IN ITEMS native polyfill)
  add_library(native_polyfill_scalar_codegen_${mode} OBJECT codegen.cc)
  target_link_libraries(native_polyfill_scalar_codegen_${mode} PRIVATE native::minimal)
  target_compile_options(native_polyfill_scalar_codegen_${mode} PRIVATE
    "$<IF:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>,/O2,-O2>")
  if(mode STREQUAL "polyfill")
    target_compile_definitions(native_polyfill_scalar_codegen_${mode} PRIVATE NATIVE_CODEGEN_POLYFILL=1)
  endif()
endforeach()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(polyfill_scalar_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(polyfill_scalar_objdump NAMES llvm-objdump HINTS "${polyfill_scalar_compiler_dir}" REQUIRED)
add_test(NAME native.polyfill.x86.scalar.codegen COMMAND "${Python3_EXECUTABLE}"
  "${PROJECT_SOURCE_DIR}/tests/x86_codegen_pairs.py" --objdump "${polyfill_scalar_objdump}"
  --raw "$<TARGET_OBJECTS:native_polyfill_scalar_codegen_native>"
  --public "$<TARGET_OBJECTS:native_polyfill_scalar_codegen_polyfill>" --expected 8
  --output "${CMAKE_CURRENT_BINARY_DIR}/native-first-$<CONFIG>.txt")
set_tests_properties(native.polyfill.x86.scalar.codegen PROPERTIES LABELS "host;polyfill;scalar;x86;codegen")
