# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_f16c_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

set(f16c_interfaces module umbrella main)
foreach(interface IN LISTS f16c_interfaces)
  set(target native_test_x86_f16c_${interface})
  add_executable(${target} ${interface}.cc)
  target_link_libraries(${target} PRIVATE native::native)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.f16c.${interface} COMMAND ${target})
  set_tests_properties(native.x86.f16c.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;f16c")
endforeach()

# Saved signal context makes unmasked exception flags observable on Linux x86-64.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_executable(native_test_x86_f16c_traps traps.cc)
  target_link_libraries(native_test_x86_f16c_traps PRIVATE native::native)
  set_target_properties(native_test_x86_f16c_traps PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.x86.f16c.traps COMMAND native_test_x86_f16c_traps)
  set_tests_properties(native.x86.f16c.traps PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;f16c")
endif()

add_library(native_test_x86_f16c_baseline OBJECT baseline.cc)
target_link_libraries(native_test_x86_f16c_baseline PRIVATE native::native)
set_target_properties(native_test_x86_f16c_baseline PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(f16c_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_F16C_OBJDUMP NAMES llvm-objdump
  HINTS "${f16c_compiler_directory}" REQUIRED)
add_test(NAME native.x86.f16c.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_F16C_OBJDUMP}" --baseline-only
    --output "${CMAKE_CURRENT_BINARY_DIR}/f16c-baseline-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_f16c_baseline> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.f16c.baseline PROPERTIES LABELS "host;codegen;baseline;x86;f16c")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

# Optional instruction requirements come from function attributes in textual
# probes. Module consumers retain the provider baseline so their BMIs agree.
set(f16c_disabled_features -mno-f16c -mno-avx -mno-avx2 -mno-avx512f
  -mno-avx512fp16 -mno-fma)
function(native_f16c_textual_probe target)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2)
    foreach(option IN LISTS f16c_disabled_features)
      target_compile_options(${target} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${target} PRIVATE -O2 ${f16c_disabled_features})
  endif()
endfunction()

add_library(native_test_x86_f16c_codegen OBJECT codegen.cc)
native_f16c_textual_probe(native_test_x86_f16c_codegen)
add_test(NAME native.x86.f16c.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_F16C_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/f16c-codegen-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_f16c_codegen>
    $<TARGET_OBJECTS:native_test_x86_f16c_baseline> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.f16c.codegen PROPERTIES LABELS "host;codegen;baseline;x86;f16c")

add_library(native_test_x86_f16c_codegen_intel OBJECT codegen.cc)
native_f16c_textual_probe(native_test_x86_f16c_codegen_intel)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_f16c_codegen_intel PRIVATE /clang:-masm=intel)
else()
  target_compile_options(native_test_x86_f16c_codegen_intel PRIVATE -masm=intel)
endif()
add_test(NAME native.x86.f16c.codegen_intel
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_F16C_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/f16c-codegen-intel-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_f16c_codegen_intel>
    $<TARGET_OBJECTS:native_test_x86_f16c_baseline> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.f16c.codegen_intel PROPERTIES LABELS "host;codegen;baseline;x86;f16c")

foreach(mode IN ITEMS baseline enabled)
  set(target native_test_x86_f16c_metadata_${mode})
  add_library(${target} OBJECT metadata.cc)
  native_f16c_textual_probe(${target})
  if(mode STREQUAL "enabled")
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(${target} PRIVATE /clang:-mf16c)
    else()
      target_compile_options(${target} PRIVATE -mf16c)
    endif()
  endif()
endforeach()

set(f16c_negative_names
  feature_scalar feature4 feature8 feature_widen4 feature_widen8
  target_scalar target4 target8 target_widen1 target_widen4 target_widen8
  immediate_range immediate_negative immediate_runtime lanes missing_lanes
  shape_integer shape_double4 shape_double8 shape_widen4 shape_widen8 shape512 feature_widen1)
set(f16c_negative_index 0)
foreach(kind IN LISTS f16c_negative_names)
  math(EXPR f16c_negative_index "${f16c_negative_index}+1")
  if(kind MATCHES "^target")
    set(diagnostic "requires target feature|target specific option mismatch")
  elseif(kind STREQUAL "missing_lanes")
    set(diagnostic "couldn.t infer template argument.*Lanes")
  elseif(kind STREQUAL "immediate_runtime")
    set(diagnostic "invalid explicitly-specified argument|non-type template argument is not a constant expression")
  else()
    set(diagnostic "constraints not satisfied|call to deleted function")
  endif()
  foreach(interface IN ITEMS header module)
    if(interface STREQUAL "module")
      set(suffix module_${kind})
    else()
      set(suffix ${kind})
    endif()
    set(target native_test_x86_f16c_reject_${suffix})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL negative.cc)
    if(interface STREQUAL "module")
      # Imported templates must preserve target and exact-type requirements.
      # Keep the provider baseline when consuming its BMI.
      target_link_libraries(${target} PRIVATE native::native)
      target_compile_definitions(${target} PRIVATE NATIVE_F16C_IMPORT=1)
      set_target_properties(${target} PROPERTIES
        CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
    else()
      native_f16c_textual_probe(${target})
    endif()
    target_compile_definitions(${target} PRIVATE NATIVE_F16C_REJECT=${f16c_negative_index})
    add_test(NAME native.x86.f16c.reject_${suffix}
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
        --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${target}
        --config "$<CONFIG>"
        --log "${CMAKE_CURRENT_BINARY_DIR}/f16c-reject-${suffix}-$<CONFIG>.log"
        --diagnostic "${diagnostic}")
    set_tests_properties(native.x86.f16c.reject_${suffix} PROPERTIES
      RUN_SERIAL TRUE LABELS "host;compile;negative;baseline;x86;f16c" TIMEOUT 180)
  endforeach()
endforeach()

# Public instruction bindings must preserve the same opcodes and immediates.
add_library(native_test_x86_f16c_codegen_public OBJECT codegen_public.cc)
target_link_libraries(native_test_x86_f16c_codegen_public PRIVATE native::native)
set_target_properties(native_test_x86_f16c_codegen_public PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_f16c_codegen_public PRIVATE /O2)
else()
  target_compile_options(native_test_x86_f16c_codegen_public PRIVATE -O2)
endif()
add_test(NAME native.x86.f16c.codegen_public
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_F16C_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/f16c-public-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_f16c_codegen_public>
    $<TARGET_OBJECTS:native_test_x86_f16c_baseline>
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.f16c.codegen_public PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;f16c")

add_test(NAME native.x86.f16c.codegen_pairs
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../x86_codegen_pairs.py"
    --objdump "${NATIVE_F16C_OBJDUMP}"
    --raw $<TARGET_OBJECTS:native_test_x86_f16c_codegen>
    --public $<TARGET_OBJECTS:native_test_x86_f16c_codegen_public>
    --output "${CMAKE_CURRENT_BINARY_DIR}/f16c-paired-$<CONFIG>.txt"
    COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.f16c.codegen_pairs PROPERTIES
  LABELS "host;modules;codegen;baseline;x86;f16c")
