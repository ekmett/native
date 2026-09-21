# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_vaes_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

foreach(kind IN ITEMS baseline minimum)
  set(target native_test_x86_vaes_metadata_${kind})
  if(kind STREQUAL "minimum")
    add_library(${target} OBJECT metadata.cc)
  else()
    add_executable(${target} metadata.cc)
    if(COMMAND native_link_clang_cl_asan)
      native_link_clang_cl_asan(${target})
    endif()
  endif()
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(kind STREQUAL "minimum")
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_FEATURE_MINIMUM=1)
    set(option -mvaes)
  else()
    set(option -mno-vaes)
  endif()
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE "/clang:${option}")
  else()
    target_compile_options(${target} PRIVATE ${option})
  endif()
  if(kind STREQUAL "baseline")
    add_test(NAME native.x86.vaes.metadata COMMAND ${target})
  endif()
endforeach()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_CRYPTO_OBJDUMP NAMES llvm-objdump HINTS "${compiler_dir}" REQUIRED)

function(native_vaes_options target)
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

set(interface_number 1)
foreach(interface IN ITEMS module hub)
  set(target native_test_x86_vaes_${interface})
  set(body ${target}_body)
  add_library(${body} OBJECT main.cc)
  target_link_libraries(${body} PRIVATE native::native)
  target_compile_definitions(${body} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  set_target_properties(${body} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  native_vaes_options(${body})
  add_executable(${target} $<TARGET_OBJECTS:${body}>)
  target_link_libraries(${target} PRIVATE native::native)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.vaes.${interface} COMMAND ${target})
  set_tests_properties(native.x86.vaes.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;vaes")
  add_test(NAME native.x86.vaes.${interface}.baseline
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_CRYPTO_OBJDUMP}" --baseline-only
      --output "${CMAKE_CURRENT_BINARY_DIR}/${interface}-baseline-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${body}> COMMAND_EXPAND_LISTS)
  math(EXPR interface_number "${interface_number}+1")
endforeach()
add_test(NAME native.x86.vaes.admission.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_CRYPTO_OBJDUMP}" --admission-only
    --output "${CMAKE_CURRENT_BINARY_DIR}/admission-$<CONFIG>.disassembly"
    $<TARGET_FILE:native_test_x86_vaes_metadata_baseline>)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

foreach(interface IN ITEMS raw public)
  set(target native_test_x86_vaes_codegen_${interface})
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
  native_vaes_options(${target})
  add_test(NAME native.x86.vaes.codegen_${interface}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_CRYPTO_OBJDUMP}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/codegen-${interface}-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${target}> COMMAND_EXPAND_LISTS)
endforeach()
add_test(NAME native.x86.vaes.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_CRYPTO_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_vaes_codegen_raw>
    --public $<TARGET_OBJECTS:native_test_x86_vaes_codegen_public>
    --expected 13
    --output "${CMAKE_CURRENT_BINARY_DIR}/paired-$<CONFIG>.txt" COMMAND_EXPAND_LISTS)

function(native_vaes_rejection_target target interface)
  if(interface STREQUAL "feature")
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
  else()
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  endif()
  native_vaes_options(${target})
endfunction()

# Each operation reaches code generation in a separate target-mismatch TU.
set(operation_sources)
foreach(operation RANGE 0 3)
  set(source "${CMAKE_CURRENT_BINARY_DIR}/reject-operation-${operation}.cc")
  file(GENERATE OUTPUT "${source}" CONTENT
    "#define NATIVE_TEST_OPERATION ${operation}\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/reject_public.cc\"\n")
  list(APPEND operation_sources "${source}")
endforeach()
foreach(width IN ITEMS 128 256 512)
  foreach(interface IN ITEMS feature public)
    set(target native_test_x86_vaes_positive_${interface}_${width})
    add_library(${target} OBJECT reject_${interface}.cc)
    native_vaes_rejection_target(${target} ${interface})
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_WIDTH=${width})
  endforeach()
  foreach(kind IN ITEMS feature target weak shape)
    set(interface public)
    set(sources reject_public.cc)
    set(omissions 1)
    if(kind STREQUAL "feature")
      set(interface feature)
      set(sources reject_feature.cc)
    elseif(kind STREQUAL "target")
      set(sources ${operation_sources})
    endif()
    if(kind STREQUAL "feature" OR kind STREQUAL "target")
      set(omissions 1 2)
    elseif(kind STREQUAL "shape")
      set(omissions 1 2 3 4)
    endif()
    string(TOUPPER "${kind}" definition)
    foreach(omission IN LISTS omissions)
      set(target native_test_x86_vaes_reject_${kind}_${width}_${omission})
      add_library(${target} OBJECT EXCLUDE_FROM_ALL ${sources})
      native_vaes_rejection_target(${target} ${interface})
      target_compile_definitions(${target} PRIVATE
        NATIVE_TEST_WIDTH=${width} NATIVE_TEST_${definition}=${omission})
      add_test(NAME native.x86.vaes.reject_${kind}_${width}_${omission}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
          --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
          --positive native_test_x86_vaes_positive_${interface}_${width}
          --negative "${target}" --kind "${kind}" --config "$<CONFIG>"
          --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-${width}-${omission}-$<CONFIG>.log")
      set_tests_properties(native.x86.vaes.reject_${kind}_${width}_${omission} PROPERTIES
        RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;modules;compile;negative;x86;vaes")
    endforeach()
  endforeach()
endforeach()

foreach(interface IN ITEMS raw public)
  set(target native_test_x86_vaes_codegen_${interface}_broad)
  if(interface STREQUAL "raw")
    add_library(${target} OBJECT codegen_broad.cc)
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
  else()
    add_library(${target} OBJECT codegen_public_broad.cc)
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  endif()
  native_vaes_options(${target})
endforeach()
add_test(NAME native.x86.vaes.codegen_pairs_broad
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_CRYPTO_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_vaes_codegen_raw_broad>
    --public $<TARGET_OBJECTS:native_test_x86_vaes_codegen_public_broad>
    --expected 13
    --output "${CMAKE_CURRENT_BINARY_DIR}/paired-broad-$<CONFIG>.txt" COMMAND_EXPAND_LISTS)

foreach(width IN ITEMS 128 256 512)
  foreach(kind IN ITEMS positive reject)
    set(target native_test_x86_vaes_${kind}_storage_${width})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_storage.cc)
    native_vaes_rejection_target(${target} public)
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_WIDTH=${width})
    if(kind STREQUAL "reject")
      target_compile_definitions(${target} PRIVATE NATIVE_TEST_STORAGE=1)
    endif()
  endforeach()
  add_test(NAME native.x86.vaes.reject_storage_${width}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --positive native_test_x86_vaes_positive_storage_${width}
      --negative native_test_x86_vaes_reject_storage_${width}
      --kind storage --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-storage-${width}-$<CONFIG>.log")
  set_tests_properties(native.x86.vaes.reject_storage_${width} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
endforeach()

get_directory_property(fixture_tests TESTS)
set_tests_properties(${fixture_tests} PROPERTIES LABELS "host;modules;x86;vaes")
