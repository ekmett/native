# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_instruction_storage_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
add_executable(native_test_instruction_storage main.cc)
target_link_libraries(native_test_instruction_storage PRIVATE native::native)
set_target_properties(native_test_instruction_storage PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
add_test(NAME native.instruction_storage COMMAND native_test_instruction_storage)
set_tests_properties(native.instruction_storage PROPERTIES LABELS "host;modules;storage")

add_executable(native_test_instruction_storage_adapters adapters.cc)
target_link_libraries(native_test_instruction_storage_adapters PRIVATE native::native)
set_target_properties(native_test_instruction_storage_adapters PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
add_test(NAME native.instruction_storage.adapters COMMAND native_test_instruction_storage_adapters)
set_tests_properties(native.instruction_storage.adapters PROPERTIES LABELS "host;modules;storage")

# The package consumer exercises logical storage; private code-generation checks
# additionally prevent physical-padding normalization in register bit bridges.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64|x86_64|AMD64|amd64)$")
  return()
endif()
add_library(native_test_instruction_storage_codegen OBJECT codegen.cc)
target_link_libraries(native_test_instruction_storage_codegen PRIVATE native::native)
set_target_properties(native_test_instruction_storage_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_instruction_storage_codegen PRIVATE /O2 /clang:-fomit-frame-pointer)
else()
  target_compile_options(native_test_instruction_storage_codegen PRIVATE -O2 -fomit-frame-pointer)
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(instruction_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(instruction_objdump NAMES llvm-objdump HINTS "${instruction_compiler_dir}" REQUIRED)
add_test(NAME native.instruction_storage.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${instruction_objdump}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/storage-$<CONFIG>.disassembly"
  $<TARGET_OBJECTS:native_test_instruction_storage_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.instruction_storage.codegen PROPERTIES LABELS "host;storage;codegen")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  foreach(index RANGE 1 3)
    set(target native_instruction_storage_reject_adapter_${index})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_adapters.cc)
    target_link_libraries(${target} PRIVATE native::native)
    target_compile_definitions(${target} PRIVATE NATIVE_ADAPTER_REJECT=${index})
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
    add_test(NAME native.instruction_storage.reject_adapter_${index} COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../source_targets/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --target ${target} --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-adapter-${index}-$<CONFIG>.log"
      --diagnostic "no matching function for call to '(floor|convert|compress)'")
    set_tests_properties(native.instruction_storage.reject_adapter_${index} PROPERTIES
      LABELS "host;storage;modules;negative" RUN_SERIAL TRUE TIMEOUT 180)
  endforeach()
endif()
