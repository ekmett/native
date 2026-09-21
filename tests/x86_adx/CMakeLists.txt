# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_adx_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_ADX_OBJDUMP NAMES llvm-objdump HINTS "${compiler_dir}" REQUIRED)

function(native_adx_options target)
  set(options -O2 -ferror-limit=0 -fconstexpr-steps=16000000)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    foreach(option IN LISTS options)
      target_compile_options(${target} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${target} PRIVATE ${options})
  endif()
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
endfunction()

foreach(minimum RANGE 0 1)
  set(target native_test_x86_adx_metadata_${minimum})
  set(body ${target})
  if(minimum EQUAL 0)
    set(body ${target}_body)
  endif()
  add_library(${body} OBJECT metadata.cc)
  if(minimum EQUAL 0)
    add_executable(${target} $<TARGET_OBJECTS:${body}>)
    target_link_libraries(${target} PRIVATE native::headers)
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
    add_test(NAME native.x86.adx.metadata COMMAND ${target})
    if(COMMAND native_link_clang_cl_asan)
      native_link_clang_cl_asan(${target})
    endif()
  else()
    target_compile_definitions(${body} PRIVATE NATIVE_TEST_FEATURE_MINIMUM=${minimum})
  endif()
  target_link_libraries(${body} PRIVATE native::headers)
  target_compile_features(${body} PRIVATE cxx_std_26)
  set_target_properties(${body} PROPERTIES CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(minimum EQUAL 1)
    set(options -madx)
  else()
    set(options -mno-adx)
  endif()
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    foreach(option IN LISTS options)
      target_compile_options(${body} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${body} PRIVATE ${options})
  endif()
endforeach()

set(interface_number 1)
foreach(interface IN ITEMS module hub)
  set(target native_test_x86_adx_${interface})
  set(body ${target}_body)
  add_library(${body} OBJECT main.cc)
  if(interface STREQUAL "module")
    set(provider native::minimal)
  else()
    set(provider native::native)
  endif()
  target_link_libraries(${body} PRIVATE ${provider})
  target_compile_definitions(${body} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  set_target_properties(${body} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  native_adx_options(${body})
  add_executable(${target} $<TARGET_OBJECTS:${body}>)
  target_link_libraries(${target} PRIVATE ${provider})
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.adx.${interface} COMMAND ${target})
  set_tests_properties(native.x86.adx.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;adx")
  add_test(NAME native.x86.adx.${interface}.baseline
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_ADX_OBJDUMP}" --baseline-only
      --output "${CMAKE_CURRENT_BINARY_DIR}/${interface}-baseline-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${body}> COMMAND_EXPAND_LISTS)
  math(EXPR interface_number "${interface_number}+1")
endforeach()
# Inspect the linked input: PE executables need not retain COFF symbol names.
add_test(NAME native.x86.adx.admission.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_ADX_OBJDUMP}" --admission-only
    --output "${CMAKE_CURRENT_BINARY_DIR}/admission-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_adx_metadata_0_body> COMMAND_EXPAND_LISTS)


if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

add_test(NAME native.x86.adx.codegen_controls
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test_codegen.py")

foreach(interface IN ITEMS raw public)
  set(target native_test_x86_adx_codegen_${interface})
  if(interface STREQUAL "raw")
    add_library(${target} OBJECT codegen.cc)
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
  else()
    add_library(${target} OBJECT codegen_public.cc)
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  endif()
  native_adx_options(${target})
  add_test(NAME native.x86.adx.codegen_${interface}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_ADX_OBJDUMP}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/codegen-${interface}-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${target}> COMMAND_EXPAND_LISTS)
endforeach()
add_test(NAME native.x86.adx.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_ADX_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_adx_codegen_raw>
    --public $<TARGET_OBJECTS:native_test_x86_adx_codegen_public>
    --expected 5
    --output "${CMAKE_CURRENT_BINARY_DIR}/paired-$<CONFIG>.txt" COMMAND_EXPAND_LISTS)

function(native_adx_rejection_target target interface)
  if(interface STREQUAL "feature")
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
  else()
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  endif()
  native_adx_options(${target})
endfunction()


foreach(width IN ITEMS 32 64)
  foreach(interface IN ITEMS feature public)
    set(target native_test_x86_adx_positive_${interface}_${width})
    add_library(${target} OBJECT reject_public.cc)
    native_adx_rejection_target(${target} ${interface})
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_WIDTH=${width})
    if(interface STREQUAL "feature")
      target_compile_definitions(${target} PRIVATE NATIVE_TEST_PRIVATE=1)
    endif()
  endforeach()
  foreach(kind IN ITEMS feature target weak shape default)
    set(interface public)
    set(omissions 1)
    if(kind STREQUAL "feature")
      set(interface feature)
    elseif(kind STREQUAL "shape")
      set(omissions 1 2)
    endif()
    string(TOUPPER "${kind}" definition)
    foreach(omission IN LISTS omissions)
      set(target native_test_x86_adx_reject_${kind}_${width}_${omission})
      add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_public.cc)
      native_adx_rejection_target(${target} ${interface})
      target_compile_definitions(${target} PRIVATE NATIVE_TEST_WIDTH=${width} NATIVE_TEST_${definition}=${omission})
      if(interface STREQUAL "feature")
        target_compile_definitions(${target} PRIVATE NATIVE_TEST_PRIVATE=1)
      endif()
      add_test(NAME native.x86.adx.reject_${kind}_${width}_${omission}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
          --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
          --positive native_test_x86_adx_positive_${interface}_${width}
          --negative "${target}" --kind "${kind}" --config "$<CONFIG>"
          --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-${width}-${omission}-$<CONFIG>.log")
      set_tests_properties(native.x86.adx.reject_${kind}_${width}_${omission} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
    endforeach()
  endforeach()
endforeach()

get_directory_property(fixture_tests TESTS)
set_tests_properties(${fixture_tests} PROPERTIES LABELS "host;modules;x86;adx")
