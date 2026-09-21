# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_pclmul_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

set(pclmul_interfaces header module x86 main)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Installed consumers use the public granular module and omnibus interface.
  set(pclmul_interfaces module x86 main)
endif()
foreach(interface IN LISTS pclmul_interfaces)
  set(target native_test_x86_pclmul_${interface})
  add_executable(${target} ${interface}.cc)
  if(interface STREQUAL "main")
    target_link_libraries(${target} PRIVATE native::native)
  else()
    target_link_libraries(${target} PRIVATE native::minimal)
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.pclmul.${interface} COMMAND ${target})
  set_tests_properties(native.x86.pclmul.${interface} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;pclmul")
endforeach()

add_library(native_test_x86_pclmul_baseline OBJECT baseline.cc)
target_link_libraries(native_test_x86_pclmul_baseline PRIVATE native::native)
set_target_properties(native_test_x86_pclmul_baseline PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(pclmul_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_PCLMUL_OBJDUMP NAMES llvm-objdump
  HINTS "${pclmul_compiler_directory}" REQUIRED)
add_test(NAME native.x86.pclmul.baseline
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_PCLMUL_OBJDUMP}" --baseline-only
    --output "${CMAKE_CURRENT_BINARY_DIR}/pclmul-baseline-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_pclmul_baseline> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.pclmul.baseline PROPERTIES LABELS "host;codegen;baseline;x86;pclmul")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

# Optional instruction requirements come from function attributes in textual
# probes. Module consumers retain the provider baseline so their BMIs agree.
set(pclmul_disabled_features -mno-pclmul -mno-vpclmulqdq -mno-avx -mno-avx2 -mno-avx512f
  -mno-avx512bw -mno-avx512vl -mno-fma)
function(native_pclmul_textual_probe target)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /O2)
    foreach(option IN LISTS pclmul_disabled_features)
      target_compile_options(${target} PRIVATE "/clang:${option}")
    endforeach()
  else()
    target_compile_options(${target} PRIVATE -O2 ${pclmul_disabled_features})
  endif()
endfunction()

add_library(native_test_x86_pclmul_codegen OBJECT codegen.cc)
native_pclmul_textual_probe(native_test_x86_pclmul_codegen)
add_test(NAME native.x86.pclmul.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py"
    --objdump "${NATIVE_PCLMUL_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/pclmul-codegen-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_pclmul_codegen>
    $<TARGET_OBJECTS:native_test_x86_pclmul_baseline> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.pclmul.codegen PROPERTIES LABELS "host;codegen;baseline;x86;pclmul")

foreach(mode IN ITEMS baseline pclmul vpclmulqdq unknown)
  set(target native_test_x86_pclmul_metadata_${mode})
  add_library(${target} OBJECT metadata.cc)
  native_pclmul_textual_probe(${target})
  if(NOT mode STREQUAL "baseline")
    if(mode STREQUAL "unknown")
      set(feature vaes)
    else()
      set(feature ${mode})
    endif()
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(${target} PRIVATE /clang:-m${feature})
    else()
      target_compile_options(${target} PRIVATE -m${feature})
    endif()
  endif()
endforeach()

set(pclmul_negative_names
  feature_legacy
  feature_vex128_pclmul
  feature_vex128_avx
  feature_vex256_vpclmul
  feature_vex256_avx
  feature_evex512_vpclmul
  feature_evex512_f
  target_legacy
  target_vex128_avx
  target_vex256_vpclmul
  target_vex256_avx
  target_evex512_f
  target_evex512_vpclmul
  immediate_range
  immediate_overflow
  immediate_negative
  immediate_runtime
  shape_float128
  shape_float256
  shape_float512
  shape_mixed
  shape_scalar
  immediate_vex128
  immediate_vex256
  immediate_evex512)
set(pclmul_negative_index 0)
set(pclmul_module_negative_names
  target_legacy target_vex128_avx target_vex256_vpclmul target_evex512_f
  shape_float128 shape_float256)
foreach(kind IN LISTS pclmul_negative_names)
  math(EXPR pclmul_negative_index "${pclmul_negative_index}+1")
  set(target native_test_x86_pclmul_reject_${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL negative.cc)
  native_pclmul_textual_probe(${target})
  target_compile_definitions(${target} PRIVATE NATIVE_PCLMUL_REJECT=${pclmul_negative_index})
  if(kind MATCHES "^target")
    set(diagnostic "requires target feature|target specific option mismatch")
  elseif(kind MATCHES "^immediate_(runtime|overflow|negative)$")
    set(diagnostic "invalid explicitly-specified argument|non-type template argument is not a constant expression|cannot be narrowed")
  else()
    set(diagnostic "constraints not satisfied|call to deleted function")
  endif()
  add_test(NAME native.x86.pclmul.reject_${kind}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${target}
      --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/pclmul-reject-${kind}-$<CONFIG>.log"
      --diagnostic "${diagnostic}")
  set_tests_properties(native.x86.pclmul.reject_${kind} PROPERTIES
    RUN_SERIAL TRUE LABELS "host;compile;negative;baseline;x86;pclmul" TIMEOUT 180)

  if(kind IN_LIST pclmul_module_negative_names)
    # Recheck the caller target and deleted overloads through the granular BMIs.
    # These consumers must retain the same baseline options as their providers.
    set(target native_test_x86_pclmul_module_reject_${kind})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL negative.cc)
    target_link_libraries(${target} PRIVATE native::minimal)
    set_target_properties(${target} PROPERTIES
      CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
    target_compile_definitions(${target} PRIVATE NATIVE_PCLMUL_MODULE_NEGATIVE=1
      NATIVE_PCLMUL_REJECT=${pclmul_negative_index})
    add_test(NAME native.x86.pclmul.module_reject_${kind}
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py"
        --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${target}
        --config "$<CONFIG>"
        --log "${CMAKE_CURRENT_BINARY_DIR}/pclmul-module-reject-${kind}-$<CONFIG>.log"
        --diagnostic "${diagnostic}")
    set_tests_properties(native.x86.pclmul.module_reject_${kind} PROPERTIES
      RUN_SERIAL TRUE LABELS "host;compile;negative;modules;baseline;x86;pclmul" TIMEOUT 180)
  endif()
endforeach()
