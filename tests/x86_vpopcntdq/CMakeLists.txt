# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_vpopcntdq_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(vpopcntdq_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_VPOPCNTDQ_OBJDUMP NAMES llvm-objdump
  HINTS "${vpopcntdq_compiler_directory}" REQUIRED)

set(disabled_features -mno-popcnt -mno-lzcnt -mno-bmi -mno-bmi2
  -mno-avx -mno-avx2 -mno-fma -mno-avx512f -mno-avx512vl
  -mno-avx512dq -mno-avx512bw -mno-avx512cd
  -mno-avx512bitalg -mno-avx512vpopcntdq)
function(native_vpopcntdq_disable_optional target)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    foreach(option IN LISTS disabled_features)
      target_compile_options(${target} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${target} PRIVATE ${disabled_features})
  endif()
endfunction()

set(interface_number 1)
set(interfaces module hub)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Installed consumers use the public modules and their relocated providers.
  set(interface_number 1)
  set(interfaces module hub)
endif()
foreach(interface IN LISTS interfaces)
  set(target native_test_x86_vpopcntdq_${interface})
  set(body ${target}_body)
  # Inspect the same object linked into the runtime consumer. PE executables
  # need not retain the COFF symbol names used by the disassembly check.
  add_library(${body} OBJECT main.cc)
  # Importers retain the provider baseline so all consumers share its BMI.
  # Textual probes below explicitly disable optional features.
  target_compile_definitions(${body} PRIVATE NATIVE_TEST_INTERFACE=${interface_number})
  set(provider native::native)
  target_link_libraries(${body} PRIVATE ${provider})
  set_target_properties(${body} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_executable(${target} $<TARGET_OBJECTS:${body}>)
  target_link_libraries(${target} PRIVATE ${provider})
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.vpopcntdq.${interface} COMMAND ${target})
  set_tests_properties(native.x86.vpopcntdq.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;vpopcntdq")
  add_test(NAME native.x86.vpopcntdq.${interface}.baseline
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
      --objdump "${NATIVE_VPOPCNTDQ_OBJDUMP}" --baseline-only
      --output "${CMAKE_CURRENT_BINARY_DIR}/${interface}-baseline-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${body}> COMMAND_EXPAND_LISTS)
  set_tests_properties(native.x86.vpopcntdq.${interface}.baseline PROPERTIES
    LABELS "host;modules;codegen;baseline;x86;vpopcntdq")
  math(EXPR interface_number "${interface_number}+1")
endforeach()

add_executable(native_test_x86_vpopcntdq_metadata metadata.cc)
native_vpopcntdq_disable_optional(native_test_x86_vpopcntdq_metadata)
target_link_libraries(native_test_x86_vpopcntdq_metadata PRIVATE native::headers)
target_compile_features(native_test_x86_vpopcntdq_metadata PRIVATE cxx_std_26)
set_target_properties(native_test_x86_vpopcntdq_metadata PROPERTIES
  CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
if(COMMAND native_link_clang_cl_asan)
  native_link_clang_cl_asan(native_test_x86_vpopcntdq_metadata)
endif()
add_test(NAME native.x86.vpopcntdq.metadata COMMAND native_test_x86_vpopcntdq_metadata)
set_tests_properties(native.x86.vpopcntdq.metadata PROPERTIES
  LABELS "host;baseline;x86;vpopcntdq")

# This object checks the preprocessor minimum for a stronger translation unit.
# It is never linked into an executable or called on the build machine.
add_library(native_test_x86_vpopcntdq_minimum OBJECT metadata.cc)
target_link_libraries(native_test_x86_vpopcntdq_minimum PRIVATE native::headers)
target_compile_features(native_test_x86_vpopcntdq_minimum PRIVATE cxx_std_26)
target_compile_definitions(native_test_x86_vpopcntdq_minimum PRIVATE
  NATIVE_TEST_EXPECT_VPOPCNTDQ_MINIMUM)
set_target_properties(native_test_x86_vpopcntdq_minimum PROPERTIES
  CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_vpopcntdq_minimum PRIVATE /clang:-mavx512vpopcntdq)
else()
  target_compile_options(native_test_x86_vpopcntdq_minimum PRIVATE -mavx512vpopcntdq)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

function(native_vpopcntdq_baseline_target target)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  native_vpopcntdq_disable_optional(${target})
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2 /clang:-ferror-limit=0)
  else()
    target_compile_options(${target} PRIVATE -O2 -ferror-limit=0)
  endif()
endfunction()

add_library(native_test_x86_vpopcntdq_codegen OBJECT codegen.cc)
native_vpopcntdq_baseline_target(native_test_x86_vpopcntdq_codegen)
add_test(NAME native.x86.vpopcntdq.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
    --objdump "${NATIVE_VPOPCNTDQ_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/vpopcntdq-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_vpopcntdq_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.vpopcntdq.codegen PROPERTIES
  LABELS "host;codegen;baseline;x86;vpopcntdq")

# Clang stops code generation after the first target-mismatch error. Separate
# translation units establish the target rejection for every operation.
set(target_negative_sources)
foreach(operation RANGE 0 5)
  set(source "${CMAKE_CURRENT_BINARY_DIR}/reject-operation-${operation}.cc")
  file(GENERATE OUTPUT "${source}" CONTENT
    "#define NATIVE_TEST_OPERATION ${operation}\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/reject.cc\"\n")
  list(APPEND target_negative_sources "${source}")
endforeach()

foreach(width IN ITEMS 128 256 512)
  set(positive native_test_x86_vpopcntdq_positive_${width})
  add_library(${positive} OBJECT reject.cc)
  native_vpopcntdq_baseline_target(${positive})
  target_compile_definitions(${positive} PRIVATE NATIVE_TEST_WIDTH=${width})
  foreach(kind IN ITEMS feature target)
    set(omissions 1 2 3)
    if(width EQUAL 512)
      set(omissions 1 2)
    endif()
    if(kind STREQUAL "target")
      list(APPEND omissions 4)
      set(definition NATIVE_TEST_TARGET)
      set(negative_sources ${target_negative_sources})
    else()
      set(definition NATIVE_TEST_FEATURE)
      set(negative_sources reject.cc)
    endif()
    foreach(omission IN LISTS omissions)
      set(negative native_test_x86_vpopcntdq_reject_${kind}_${width}_${omission})
      add_library(${negative} OBJECT EXCLUDE_FROM_ALL ${negative_sources})
      native_vpopcntdq_baseline_target(${negative})
      target_compile_definitions(${negative} PRIVATE
        NATIVE_TEST_WIDTH=${width} ${definition}=${omission})
      add_test(NAME native.x86.vpopcntdq.reject_${kind}_${width}_${omission}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_negative.py"
          --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
          --positive "${positive}" --negative "${negative}" --kind "${kind}"
          --config "$<CONFIG>"
          --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-${width}-${omission}-$<CONFIG>.log")
      set_tests_properties(native.x86.vpopcntdq.reject_${kind}_${width}_${omission}
        PROPERTIES RUN_SERIAL TRUE TIMEOUT 180
        LABELS "host;compile;baseline;x86;vpopcntdq")
    endforeach()
  endforeach()
endforeach()

# Public instruction bindings must preserve the same opcodes and immediates.
add_library(native_test_x86_vpopcntdq_codegen_public OBJECT codegen_public.cc)
target_link_libraries(native_test_x86_vpopcntdq_codegen_public PRIVATE native::native)
set_target_properties(native_test_x86_vpopcntdq_codegen_public PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_vpopcntdq_codegen_public PRIVATE /O2)
else()
  target_compile_options(native_test_x86_vpopcntdq_codegen_public PRIVATE -O2)
endif()
add_test(NAME native.x86.vpopcntdq.codegen_public
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_VPOPCNTDQ_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/vpopcntdq-public-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_vpopcntdq_codegen_public>
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.vpopcntdq.codegen_public PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;vpopcntdq")

foreach(index RANGE 1 3)
  set(target native_test_x86_vpopcntdq_reject_public_${index})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL reject_public.cc)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_definitions(${target} PRIVATE NATIVE_PUBLIC_REJECT=${index})
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.x86.vpopcntdq.reject_public_${index}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_public_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${target}
      --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-public-${index}-$<CONFIG>.log"
      --diagnostic "requires target feature")
  set_tests_properties(native.x86.vpopcntdq.reject_public_${index} PROPERTIES
    RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;modules;compile;negative;x86;vpopcntdq")
endforeach()

add_test(NAME native.x86.vpopcntdq.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_VPOPCNTDQ_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_vpopcntdq_codegen>
    --public $<TARGET_OBJECTS:native_test_x86_vpopcntdq_codegen_public>
    --output "${CMAKE_CURRENT_BINARY_DIR}/vpopcntdq-paired-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.vpopcntdq.codegen_pairs PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;vpopcntdq")

# Superset ISA tags select arithmetic predicates rather than storage-only masks.
# Keep their scalar extraction usable from each instruction's minimal target.
add_library(native_test_x86_vpopcntdq_codegen_broad_raw OBJECT codegen.cc)
native_vpopcntdq_baseline_target(native_test_x86_vpopcntdq_codegen_broad_raw)
add_library(native_test_x86_vpopcntdq_codegen_broad_public OBJECT codegen_public.cc)
target_link_libraries(native_test_x86_vpopcntdq_codegen_broad_public PRIVATE native::native)
set_target_properties(native_test_x86_vpopcntdq_codegen_broad_public PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
foreach(target IN ITEMS native_test_x86_vpopcntdq_codegen_broad_raw
    native_test_x86_vpopcntdq_codegen_broad_public)
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_BROAD_ARCH)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2)
  else()
    target_compile_options(${target} PRIVATE -O2)
  endif()
endforeach()
add_test(NAME native.x86.vpopcntdq.codegen_broad
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --broad-arch
    --objdump "${NATIVE_VPOPCNTDQ_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/vpopcntdq-broad-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_vpopcntdq_codegen_broad_public>
    COMMAND_EXPAND_LISTS)
add_test(NAME native.x86.vpopcntdq.codegen_broad_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_VPOPCNTDQ_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_vpopcntdq_codegen_broad_raw>
    --public $<TARGET_OBJECTS:native_test_x86_vpopcntdq_codegen_broad_public>
    --expected 19
    --output "${CMAKE_CURRENT_BINARY_DIR}/vpopcntdq-broad-paired-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.vpopcntdq.codegen_broad
  native.x86.vpopcntdq.codegen_broad_pairs PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;vpopcntdq")
