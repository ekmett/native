# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_avx512cd_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(native_avx512cd_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_AVX512CD_OBJDUMP NAMES llvm-objdump
  HINTS "${native_avx512cd_compiler_dir}" REQUIRED)

function(native_avx512cd_baseline_options target)
  set(disabled_features -mno-popcnt -mno-lzcnt -mno-bmi -mno-bmi2
    -mno-avx -mno-avx2 -mno-fma -mno-avx512f -mno-avx512vl
    -mno-avx512dq -mno-avx512bw -mno-avx512cd)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    foreach(option IN LISTS disabled_features)
      target_compile_options(${target} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${target} PRIVATE ${disabled_features})
  endif()
endfunction()

set(interface_number 1)
foreach(interface IN ITEMS module hub)
  set(target native_test_x86_avx512cd_${interface})
  set(body ${target}_body)
  add_library(${body} OBJECT main.cc)
  target_link_libraries(${body} PRIVATE native::native)
  target_compile_definitions(${body} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  # The complete lane/mask banks exceed Clang's default constexpr step budget.
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${body} PRIVATE /clang:-fconstexpr-steps=8000000)
  else()
    target_compile_options(${body} PRIVATE -fconstexpr-steps=8000000)
  endif()
  set_target_properties(${body} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_executable(${target} $<TARGET_OBJECTS:${body}>)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.avx512cd.${interface} COMMAND ${target})
  set_tests_properties(native.x86.avx512cd.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;avx512cd")
  add_test(NAME native.x86.avx512cd.${interface}.baseline
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_AVX512CD_OBJDUMP}" --baseline-only
      --output "${CMAKE_CURRENT_BINARY_DIR}/${interface}-baseline-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${body}> COMMAND_EXPAND_LISTS)
  set_tests_properties(native.x86.avx512cd.${interface}.baseline PROPERTIES
    LABELS "host;modules;codegen;baseline;x86;avx512cd")
  math(EXPR interface_number "${interface_number}+1")
endforeach()

add_executable(native_test_x86_avx512cd_metadata metadata.cc)
target_link_libraries(native_test_x86_avx512cd_metadata PRIVATE native::headers)
target_compile_features(native_test_x86_avx512cd_metadata PRIVATE cxx_std_26)
set_target_properties(native_test_x86_avx512cd_metadata PROPERTIES
  CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
native_avx512cd_baseline_options(native_test_x86_avx512cd_metadata)
if(COMMAND native_link_clang_cl_asan)
  native_link_clang_cl_asan(native_test_x86_avx512cd_metadata)
endif()
add_test(NAME native.x86.avx512cd.metadata COMMAND native_test_x86_avx512cd_metadata)
set_tests_properties(native.x86.avx512cd.metadata PROPERTIES LABELS "host;baseline;x86;avx512cd")

# Raw-register helpers are private implementation inputs; installed consumers
# exercise only the public module and hub above.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

add_library(native_test_x86_avx512cd_codegen OBJECT codegen.cc)
target_link_libraries(native_test_x86_avx512cd_codegen PRIVATE native::headers)
target_compile_features(native_test_x86_avx512cd_codegen PRIVATE cxx_std_26)
set_target_properties(native_test_x86_avx512cd_codegen PROPERTIES CXX_SCAN_FOR_MODULES OFF)
native_avx512cd_baseline_options(native_test_x86_avx512cd_codegen)

add_library(native_test_x86_avx512cd_codegen_public OBJECT codegen_public.cc)
target_link_libraries(native_test_x86_avx512cd_codegen_public PRIVATE native::native)
set_target_properties(native_test_x86_avx512cd_codegen_public PROPERTIES CXX_SCAN_FOR_MODULES ON)

add_library(native_test_x86_avx512cd_codegen_broad OBJECT codegen_broad.cc)
target_link_libraries(native_test_x86_avx512cd_codegen_broad PRIVATE native::headers)
target_compile_features(native_test_x86_avx512cd_codegen_broad PRIVATE cxx_std_26)
set_target_properties(native_test_x86_avx512cd_codegen_broad PROPERTIES CXX_SCAN_FOR_MODULES OFF)
native_avx512cd_baseline_options(native_test_x86_avx512cd_codegen_broad)

add_library(native_test_x86_avx512cd_codegen_public_broad OBJECT codegen_public_broad.cc)
target_link_libraries(native_test_x86_avx512cd_codegen_public_broad PRIVATE native::native)
set_target_properties(native_test_x86_avx512cd_codegen_public_broad PROPERTIES CXX_SCAN_FOR_MODULES ON)

foreach(target IN ITEMS native_test_x86_avx512cd_codegen native_test_x86_avx512cd_codegen_public
    native_test_x86_avx512cd_codegen_broad native_test_x86_avx512cd_codegen_public_broad)
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2)
  else()
    target_compile_options(${target} PRIVATE -O2)
  endif()
endforeach()

add_test(NAME native.x86.avx512cd.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_AVX512CD_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_avx512cd_codegen>
    --public $<TARGET_OBJECTS:native_test_x86_avx512cd_codegen_public>
    --expected 37
    --output "${CMAKE_CURRENT_BINARY_DIR}/avx512cd-paired-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.avx512cd.codegen_pairs PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;avx512cd")

add_test(NAME native.x86.avx512cd.codegen_pairs_broad
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_AVX512CD_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_avx512cd_codegen_broad>
    --public $<TARGET_OBJECTS:native_test_x86_avx512cd_codegen_public_broad>
    --expected 37
    --output "${CMAKE_CURRENT_BINARY_DIR}/avx512cd-paired-broad-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.avx512cd.codegen_pairs_broad PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;avx512cd")

foreach(form IN ITEMS codegen codegen_public codegen_broad codegen_public_broad)
  set(caller_options)
  if(form MATCHES "_broad$")
    set(caller_options --broad-target)
  endif()
  add_test(NAME native.x86.avx512cd.${form}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
      --objdump "${NATIVE_AVX512CD_OBJDUMP}"
      --output "${CMAKE_CURRENT_BINARY_DIR}/${form}-$<CONFIG>.disassembly"
      ${caller_options} $<TARGET_OBJECTS:native_test_x86_avx512cd_${form}> COMMAND_EXPAND_LISTS)
  set_tests_properties(native.x86.avx512cd.${form} PROPERTIES
    LABELS "host;modules;codegen;baseline;x86;avx512cd")
endforeach()

function(native_avx512cd_rejection_target target interface)
  if(interface STREQUAL "public")
    target_link_libraries(${target} PRIVATE native::native)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  else()
    target_link_libraries(${target} PRIVATE native::headers)
    target_compile_features(${target} PRIVATE cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
    native_avx512cd_baseline_options(${target})
  endif()
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2 /clang:-ferror-limit=0)
  else()
    target_compile_options(${target} PRIVATE -O2 -ferror-limit=0)
  endif()
endfunction()

# Separate target-mismatch operations so every rejection reaches code generation.
set(target_negative_sources)
foreach(operation RANGE 0 11)
  set(source "${CMAKE_CURRENT_BINARY_DIR}/reject-operation-${operation}.cc")
  file(GENERATE OUTPUT "${source}" CONTENT
    "#define NATIVE_TEST_OPERATION ${operation}\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/reject_public.cc\"\n")
  list(APPEND target_negative_sources "${source}")
endforeach()

foreach(width IN ITEMS 128 256 512)
  foreach(interface IN ITEMS feature public)
    set(positive native_test_x86_avx512cd_positive_${interface}_${width})
    add_library(${positive} OBJECT reject_${interface}.cc)
    native_avx512cd_rejection_target(${positive} ${interface})
    target_compile_definitions(${positive} PRIVATE NATIVE_TEST_WIDTH=${width})
  endforeach()
  foreach(kind IN ITEMS feature target weak shape mask)
    set(interface public)
    set(negative_sources reject_public.cc)
    set(omissions 1)
    if(kind STREQUAL "feature" OR kind STREQUAL "target")
      set(omissions 1 2 3)
      if(width EQUAL 512)
        set(omissions 1 2)
      endif()
      if(kind STREQUAL "feature")
        set(interface feature)
        set(negative_sources reject_feature.cc)
      else()
        list(APPEND omissions 4)
        set(negative_sources ${target_negative_sources})
      endif()
    elseif(kind STREQUAL "shape")
      set(omissions 1 2 3 4)
    elseif(kind STREQUAL "mask")
      set(omissions 1 2 3)
    endif()
    string(TOUPPER "${kind}" definition)
    foreach(omission IN LISTS omissions)
      set(negative native_test_x86_avx512cd_reject_${kind}_${width}_${omission})
      add_library(${negative} OBJECT EXCLUDE_FROM_ALL ${negative_sources})
      native_avx512cd_rejection_target(${negative} ${interface})
      target_compile_definitions(${negative} PRIVATE
        NATIVE_TEST_WIDTH=${width} NATIVE_TEST_${definition}=${omission})
      add_test(NAME native.x86.avx512cd.reject_${kind}_${width}_${omission}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
          --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
          --positive native_test_x86_avx512cd_positive_${interface}_${width}
          --negative "${negative}" --kind "${kind}" --config "$<CONFIG>"
          --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-${width}-${omission}-$<CONFIG>.log")
      set_tests_properties(native.x86.avx512cd.reject_${kind}_${width}_${omission} PROPERTIES
        RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;modules;compile;negative;x86;avx512cd")
    endforeach()
  endforeach()
endforeach()
