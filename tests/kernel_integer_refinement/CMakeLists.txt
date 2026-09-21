# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_kernel_integer_refinement LANGUAGES CXX)
  set(NATIVE_BUILD_TESTS OFF CACHE BOOL "Build the focused fixture only" FORCE)
  add_subdirectory(../.. native)
  enable_testing()
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(integer_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(integer_objdump NAMES llvm-objdump HINTS "${integer_compiler_dir}" REQUIRED)
add_test(NAME native.kernel_integer.codegen_controls COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --self-test)
foreach(kind IN ITEMS header import)
  set(integer_codegen_options)
  add_library(native_kernel_integer_${kind} OBJECT main.cc)
  target_link_libraries(native_kernel_integer_${kind} PRIVATE native::native)
  if(kind STREQUAL "import")
    target_compile_definitions(native_kernel_integer_${kind} PRIVATE NATIVE_TEST_IMPORT=1)
    set(integer_codegen_options --instruction-storage)
  endif()
  set_target_properties(native_kernel_integer_${kind} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_executable(native_test_kernel_integer_${kind} $<TARGET_OBJECTS:native_kernel_integer_${kind}>)
  target_link_libraries(native_test_kernel_integer_${kind} PRIVATE native::native)
  set_target_properties(native_test_kernel_integer_${kind} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(native_test_kernel_integer_${kind})
  endif()
  add_test(NAME native.kernel_integer.${kind} COMMAND native_test_kernel_integer_${kind})
  set_tests_properties(native.kernel_integer.${kind} PROPERTIES LABELS "host;integer;modules;exact" TIMEOUT 120)
  # Instrumentation inserts calls/branches; inspect the ordinary object only.
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$" AND NOT NATIVE_ENABLE_ASAN)
    add_test(NAME native.kernel_integer.${kind}_codegen COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${integer_objdump}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/${kind}-$<CONFIG>.disassembly"
      ${integer_codegen_options} $<TARGET_OBJECTS:native_kernel_integer_${kind}> COMMAND_EXPAND_LISTS)
    set_tests_properties(native.kernel_integer.${kind}_codegen PROPERTIES LABELS "host;integer;codegen" TIMEOUT 120)
  endif()
endforeach()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  foreach(index RANGE 1 3)
    set(target native_kernel_integer_reject_storage_${index})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_storage.cc)
    target_link_libraries(${target} PRIVATE native::native)
    target_compile_definitions(${target} PRIVATE NATIVE_STORAGE_REJECT=${index})
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
    add_test(NAME native.kernel_integer.reject_storage_${index} COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../source_targets/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --target ${target} --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-storage-${index}-$<CONFIG>.log"
      --diagnostic "no matching function for call to '(reinterpret_bits|narrow_concat)'")
    set_tests_properties(native.kernel_integer.reject_storage_${index} PROPERTIES
      LABELS "host;integer;modules;negative" RUN_SERIAL TRUE TIMEOUT 180)
  endforeach()
endif()
