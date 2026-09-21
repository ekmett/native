# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_ifma_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_IFMA_OBJDUMP NAMES llvm-objdump HINTS "${compiler_dir}" REQUIRED)

function(native_ifma_options target)
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

foreach(minimum RANGE 0 3)
  set(target native_test_x86_ifma_metadata_${minimum})
  set(body ${target})
  if(minimum EQUAL 0)
    set(body ${target}_body)
  endif()
  add_library(${body} OBJECT metadata.cc)
  if(minimum EQUAL 0)
    add_executable(${target} $<TARGET_OBJECTS:${body}>)
    target_link_libraries(${target} PRIVATE native::headers)
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
    add_test(NAME native.x86.ifma.metadata COMMAND ${target})
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
    set(options -mavxifma)
  elseif(minimum EQUAL 2)
    set(options -mavx512ifma)
  elseif(minimum EQUAL 3)
    set(options -mavx512ifma -mavx512vl)
  else()
    set(options -mno-avxifma -mno-avx512ifma)
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
  set(target native_test_x86_ifma_${interface})
  set(body ${target}_body)
  add_library(${body} OBJECT main.cc)
  target_link_libraries(${body} PRIVATE native::native)
  target_compile_definitions(${body} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  set_target_properties(${body} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  native_ifma_options(${body})
  add_executable(${target} $<TARGET_OBJECTS:${body}>)
  target_link_libraries(${target} PRIVATE native::native)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.ifma.${interface} COMMAND ${target})
  set_tests_properties(native.x86.ifma.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;ifma")
  add_test(NAME native.x86.ifma.${interface}.baseline
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_IFMA_OBJDUMP}" --baseline-only
      --output "${CMAKE_CURRENT_BINARY_DIR}/${interface}-baseline-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${body}> COMMAND_EXPAND_LISTS)
  math(EXPR interface_number "${interface_number}+1")
endforeach()
# Inspect the linked input: PE executables need not retain COFF symbol names.
add_test(NAME native.x86.ifma.admission.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_IFMA_OBJDUMP}" --admission-only
    --output "${CMAKE_CURRENT_BINARY_DIR}/admission-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_ifma_metadata_0_body> COMMAND_EXPAND_LISTS)


if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

foreach(interface IN ITEMS raw public)
  set(target native_test_x86_ifma_codegen_${interface})
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
  native_ifma_options(${target})
  add_test(NAME native.x86.ifma.codegen_${interface}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_IFMA_OBJDUMP}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/codegen-${interface}-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${target}> COMMAND_EXPAND_LISTS)
endforeach()
add_test(NAME native.x86.ifma.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_IFMA_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_ifma_codegen_raw>
    --public $<TARGET_OBJECTS:native_test_x86_ifma_codegen_public>
    --expected 23
    --output "${CMAKE_CURRENT_BINARY_DIR}/paired-$<CONFIG>.txt" COMMAND_EXPAND_LISTS)

function(native_ifma_rejection_target target interface)
  if(interface STREQUAL "feature")
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
  else()
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  endif()
  native_ifma_options(${target})
endfunction()


foreach(interface IN ITEMS raw public)
  set(target native_test_x86_ifma_codegen_${interface}_broad)
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
  native_ifma_options(${target})
endforeach()
add_test(NAME native.x86.ifma.codegen_pairs_broad
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_IFMA_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_ifma_codegen_raw_broad>
    --public $<TARGET_OBJECTS:native_test_x86_ifma_codegen_public_broad>
    --expected 23
    --output "${CMAKE_CURRENT_BINARY_DIR}/paired-broad-$<CONFIG>.txt" COMMAND_EXPAND_LISTS)


set(operation_sources)
foreach(operation RANGE 0 5)
  set(source "${CMAKE_CURRENT_BINARY_DIR}/reject-operation-${operation}.cc")
  file(GENERATE OUTPUT "${source}" CONTENT
    "#define NATIVE_TEST_OPERATION ${operation}\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/reject_public.cc\"\n")
  list(APPEND operation_sources "${source}")
endforeach()
set(form_number 0)
foreach(form IN ITEMS vex128 vex256 evex128 evex256 evex512)
  foreach(interface IN ITEMS feature public)
    set(target native_test_x86_ifma_positive_${interface}_${form})
    add_library(${target} OBJECT reject_${interface}.cc)
    native_ifma_rejection_target(${target} ${interface})
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_FORM=${form_number})
  endforeach()
  foreach(kind IN ITEMS feature target weak shape mask)
    if(kind STREQUAL "mask" AND form MATCHES "^vex")
      continue()
    endif()
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
      if(form MATCHES "^evex(128|256)$")
        list(APPEND omissions 3)
      endif()
    elseif(kind STREQUAL "shape")
      set(omissions 1 2 3 4)
    elseif(kind STREQUAL "mask")
      set(omissions 1 2 3)
    endif()
    string(TOUPPER "${kind}" definition)
    foreach(omission IN LISTS omissions)
      set(target native_test_x86_ifma_reject_${kind}_${form}_${omission})
      add_library(${target} OBJECT EXCLUDE_FROM_ALL ${sources})
      native_ifma_rejection_target(${target} ${interface})
      target_compile_definitions(${target} PRIVATE NATIVE_TEST_FORM=${form_number} NATIVE_TEST_${definition}=${omission})
      add_test(NAME native.x86.ifma.reject_${kind}_${form}_${omission}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
          --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
          --positive native_test_x86_ifma_positive_${interface}_${form}
          --negative "${target}" --kind "${kind}" --form "${form}" --config "$<CONFIG>"
          --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-${form}-${omission}-$<CONFIG>.log")
      set_tests_properties(native.x86.ifma.reject_${kind}_${form}_${omission} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
    endforeach()
  endforeach()
  math(EXPR form_number "${form_number}+1")
endforeach()

foreach(width IN ITEMS 128 256 512)
  foreach(kind IN ITEMS positive reject)
    set(target native_test_x86_ifma_${kind}_storage_${width})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_storage.cc)
    native_ifma_rejection_target(${target} public)
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_WIDTH=${width})
    if(kind STREQUAL "reject")
      target_compile_definitions(${target} PRIVATE NATIVE_TEST_STORAGE=1)
    endif()
  endforeach()
  add_test(NAME native.x86.ifma.reject_storage_${width}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --positive native_test_x86_ifma_positive_storage_${width}
      --negative native_test_x86_ifma_reject_storage_${width}
      --form storage --kind storage --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-storage-${width}-$<CONFIG>.log")
  set_tests_properties(native.x86.ifma.reject_storage_${width} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
endforeach()


get_directory_property(fixture_tests TESTS)
set_tests_properties(${fixture_tests} PROPERTIES LABELS "host;modules;x86;ifma")
