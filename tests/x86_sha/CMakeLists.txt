# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_sha_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

foreach(kind IN ITEMS baseline minimum)
  set(target native_test_x86_sha_metadata_${kind})
  set(body ${target})
  if(kind STREQUAL "minimum")
    add_library(${body} OBJECT metadata.cc)
  else()
    # Inspect the same object linked into the executable. A linked Windows PE
    # does not retain the COFF symbol names needed by the disassembly check.
    set(body ${target}_body)
    add_library(${body} OBJECT metadata.cc)
    add_executable(${target} $<TARGET_OBJECTS:${body}>)
    target_link_libraries(${target} PRIVATE native::headers)
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
    if(COMMAND native_link_clang_cl_asan)
      native_link_clang_cl_asan(${target})
    endif()
  endif()
  target_link_libraries(${body} PRIVATE native::headers)
  target_compile_features(${body} PRIVATE cxx_std_26)
  set_target_properties(${body} PROPERTIES CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(kind STREQUAL "minimum")
    target_compile_definitions(${body} PRIVATE NATIVE_TEST_FEATURE_MINIMUM=1)
    set(option -msha)
  else()
    set(option -mno-sha)
  endif()
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${body} PRIVATE "/clang:${option}")
  else()
    target_compile_options(${body} PRIVATE ${option})
  endif()
  if(kind STREQUAL "baseline")
    add_test(NAME native.x86.sha.metadata COMMAND ${target})
  endif()
endforeach()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_CRYPTO_OBJDUMP NAMES llvm-objdump HINTS "${compiler_dir}" REQUIRED)

function(native_sha_options target)
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
  set(target native_test_x86_sha_${interface})
  set(body ${target}_body)
  add_library(${body} OBJECT main.cc)
  target_link_libraries(${body} PRIVATE native::native)
  target_compile_definitions(${body} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  set_target_properties(${body} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  native_sha_options(${body})
  add_executable(${target} $<TARGET_OBJECTS:${body}>)
  target_link_libraries(${target} PRIVATE native::native)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.sha.${interface} COMMAND ${target})
  set_tests_properties(native.x86.sha.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;sha")
  add_test(NAME native.x86.sha.${interface}.baseline
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_CRYPTO_OBJDUMP}" --baseline-only
      --output "${CMAKE_CURRENT_BINARY_DIR}/${interface}-baseline-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${body}> COMMAND_EXPAND_LISTS)
  math(EXPR interface_number "${interface_number}+1")
endforeach()
add_test(NAME native.x86.sha.admission.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_CRYPTO_OBJDUMP}" --admission-only
    --output "${CMAKE_CURRENT_BINARY_DIR}/admission-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_sha_metadata_baseline_body> COMMAND_EXPAND_LISTS)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

foreach(interface IN ITEMS raw public)
  set(target native_test_x86_sha_codegen_${interface})
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
  native_sha_options(${target})
  add_test(NAME native.x86.sha.codegen_${interface}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_CRYPTO_OBJDUMP}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/codegen-${interface}-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${target}> COMMAND_EXPAND_LISTS)
endforeach()
add_test(NAME native.x86.sha.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_CRYPTO_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_sha_codegen_raw>
    --public $<TARGET_OBJECTS:native_test_x86_sha_codegen_public>
    --expected 11
    --output "${CMAKE_CURRENT_BINARY_DIR}/paired-$<CONFIG>.txt" COMMAND_EXPAND_LISTS)

function(native_sha_rejection_target target interface)
  if(interface STREQUAL "feature")
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
  else()
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  endif()
  native_sha_options(${target})
endfunction()

# Each operation reaches code generation in a separate target-mismatch TU.
set(operation_sources)
foreach(operation RANGE 0 9)
  set(source "${CMAKE_CURRENT_BINARY_DIR}/reject-operation-${operation}.cc")
  file(GENERATE OUTPUT "${source}" CONTENT
    "#define NATIVE_TEST_OPERATION ${operation}\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/reject_public.cc\"\n")
  list(APPEND operation_sources "${source}")
endforeach()
foreach(width IN ITEMS 128)
  foreach(interface IN ITEMS feature public)
    set(target native_test_x86_sha_positive_${interface}_${width})
    add_library(${target} OBJECT reject_${interface}.cc)
    native_sha_rejection_target(${target} ${interface})
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
      set(omissions 1)
    elseif(kind STREQUAL "shape")
      set(omissions 1 2 3 4)
    endif()
    string(TOUPPER "${kind}" definition)
    foreach(omission IN LISTS omissions)
      set(target native_test_x86_sha_reject_${kind}_${width}_${omission})
      add_library(${target} OBJECT EXCLUDE_FROM_ALL ${sources})
      native_sha_rejection_target(${target} ${interface})
      target_compile_definitions(${target} PRIVATE
        NATIVE_TEST_WIDTH=${width} NATIVE_TEST_${definition}=${omission})
      add_test(NAME native.x86.sha.reject_${kind}_${width}_${omission}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
          --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
          --positive native_test_x86_sha_positive_${interface}_${width}
          --negative "${target}" --kind "${kind}" --config "$<CONFIG>"
          --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-${width}-${omission}-$<CONFIG>.log")
      set_tests_properties(native.x86.sha.reject_${kind}_${width}_${omission} PROPERTIES
        RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;modules;compile;negative;x86;sha")
    endforeach()
  endforeach()
endforeach()

foreach(interface IN ITEMS raw public)
  set(target native_test_x86_sha_codegen_${interface}_broad)
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
  native_sha_options(${target})
endforeach()
add_test(NAME native.x86.sha.codegen_pairs_broad
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_CRYPTO_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_sha_codegen_raw_broad>
    --public $<TARGET_OBJECTS:native_test_x86_sha_codegen_public_broad>
    --expected 11
    --output "${CMAKE_CURRENT_BINARY_DIR}/paired-broad-$<CONFIG>.txt" COMMAND_EXPAND_LISTS)

foreach(width IN ITEMS 128)
  foreach(kind IN ITEMS positive reject)
    set(target native_test_x86_sha_${kind}_storage_${width})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_storage.cc)
    native_sha_rejection_target(${target} public)
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_WIDTH=${width})
    if(kind STREQUAL "reject")
      target_compile_definitions(${target} PRIVATE NATIVE_TEST_STORAGE=1)
    endif()
  endforeach()
  add_test(NAME native.x86.sha.reject_storage_${width}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --positive native_test_x86_sha_positive_storage_${width}
      --negative native_test_x86_sha_reject_storage_${width}
      --kind storage --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-storage-${width}-$<CONFIG>.log")
  set_tests_properties(native.x86.sha.reject_storage_${width} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
endforeach()

foreach(weak IN ITEMS 0 1)
  set(positive native_test_x86_sha_positive_immediate_${weak})
  add_library(${positive} OBJECT EXCLUDE_FROM_ALL reject_immediate.cc)
  native_sha_rejection_target(${positive} public)
  target_compile_definitions(${positive} PRIVATE NATIVE_TEST_WEAK=${weak})
  foreach(selector RANGE 1 4)
    set(negative native_test_x86_sha_reject_immediate_${weak}_${selector})
    add_library(${negative} OBJECT EXCLUDE_FROM_ALL reject_immediate.cc)
    native_sha_rejection_target(${negative} public)
    target_compile_definitions(${negative} PRIVATE NATIVE_TEST_WEAK=${weak} NATIVE_TEST_IMMEDIATE=${selector})
    add_test(NAME native.x86.sha.reject_immediate_${weak}_${selector}
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
        --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
        --positive "${positive}" --negative "${negative}" --kind immediate --config "$<CONFIG>"
        --log "${CMAKE_CURRENT_BINARY_DIR}/reject-immediate-${weak}-${selector}-$<CONFIG>.log")
    set_tests_properties(native.x86.sha.reject_immediate_${weak}_${selector} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
  endforeach()
endforeach()

get_directory_property(fixture_tests TESTS)
set_tests_properties(${fixture_tests} PROPERTIES LABELS "host;modules;x86;sha")
