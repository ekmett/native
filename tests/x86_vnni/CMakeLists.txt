# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_vnni_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(vnni_compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_VNNI_OBJDUMP NAMES llvm-objdump
  HINTS "${vnni_compiler_directory}" REQUIRED)

set(vnni_interface_number 0)
set(vnni_interfaces header module x86 hub)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Installed consumers use modules from the package, including after relocation.
  set(vnni_interface_number 1)
  set(vnni_interfaces module x86 hub)
endif()
foreach(interface IN LISTS vnni_interfaces)
  set(target native_test_x86_vnni_${interface})
  set(objects ${target}_objects)
  add_library(${objects} OBJECT main.cc)
  add_executable(${target} $<TARGET_OBJECTS:${objects}>)
  target_compile_definitions(${objects} PRIVATE NATIVE_TEST_INTERFACE=${vnni_interface_number})
  if(interface STREQUAL "hub")
    set(provider native::native)
  else()
    set(provider native::minimal)
  endif()
  target_link_libraries(${objects} PRIVATE ${provider})
  target_link_libraries(${target} PRIVATE ${provider})
  # Importers retain the provider baseline; changing ISA flags here duplicates BMIs.
  set_target_properties(${objects} ${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  foreach(family IN ITEMS core evex512 evexvl int8 int16)
    add_test(NAME native.x86.vnni.${interface}.${family} COMMAND ${target} ${family})
    set_tests_properties(native.x86.vnni.${interface}.${family} PROPERTIES
      SKIP_RETURN_CODE 77 LABELS "host;modules;baseline;x86;vnni")
  endforeach()
  add_test(NAME native.x86.vnni.${interface}.baseline
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
      --objdump "${NATIVE_VNNI_OBJDUMP}" --baseline-only
      --output "${CMAKE_CURRENT_BINARY_DIR}/${interface}-baseline-$<CONFIG>.disassembly"
      $<TARGET_OBJECTS:${objects}> COMMAND_EXPAND_LISTS)
  set_tests_properties(native.x86.vnni.${interface}.baseline PROPERTIES
    LABELS "host;modules;codegen;baseline;x86;vnni")
  math(EXPR vnni_interface_number "${vnni_interface_number}+1")
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

# Optional-feature probes are isolated textual objects. Their compile flags do
# not flow into module providers or consumers.
set(vnni_disabled_features -mno-popcnt -mno-lzcnt -mno-bmi -mno-bmi2
  -mno-avx -mno-avx2 -mno-fma -mno-avx512f -mno-avx512vl
  -mno-avx512dq -mno-avx512bw -mno-avx512cd -mno-avx512bitalg
  -mno-avx512vpopcntdq -mno-avxvnni -mno-avx512vnni
  -mno-avxvnniint8 -mno-avxvnniint16 -mno-avxifma)
function(native_vnni_options target)
  foreach(option IN LISTS ARGN)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(${target} PRIVATE "/clang:${option}")
    else()
      target_compile_options(${target} PRIVATE "${option}")
    endif()
  endforeach()
endfunction()

function(native_vnni_textual_probe target)
  target_link_libraries(${target} PRIVATE native::headers)
  target_compile_features(${target} PRIVATE cxx_std_26)
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)
  native_vnni_options(${target} ${vnni_disabled_features} -O2 -ferror-limit=0)
endfunction()

add_executable(native_test_x86_vnni_metadata metadata.cc)
native_vnni_textual_probe(native_test_x86_vnni_metadata)
if(COMMAND native_link_clang_cl_asan)
  native_link_clang_cl_asan(native_test_x86_vnni_metadata)
endif()
add_test(NAME native.x86.vnni.metadata COMMAND native_test_x86_vnni_metadata)
set_tests_properties(native.x86.vnni.metadata PROPERTIES LABELS "host;baseline;x86;vnni")

# The stronger compiler-minimum probes are compiled only, never executed.
foreach(feature IN ITEMS avxvnni avx512vnni avxvnniint8 avxvnniint16 avxifma)
  set(target native_test_x86_vnni_minimum_${feature})
  add_library(${target} OBJECT metadata.cc)
  native_vnni_textual_probe(${target})
  native_vnni_options(${target} -m${feature})
  if(feature STREQUAL "avxifma")
    target_compile_definitions(${target} PRIVATE NATIVE_VNNI_EXPECT_UNKNOWN=1)
  endif()
endforeach()

add_library(native_test_x86_vnni_codegen OBJECT codegen.cc)
native_vnni_textual_probe(native_test_x86_vnni_codegen)
add_test(NAME native.x86.vnni.codegen
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_codegen.py"
    --objdump "${NATIVE_VNNI_OBJDUMP}"
    --output "${CMAKE_CURRENT_BINARY_DIR}/vnni-$<CONFIG>.disassembly"
    $<TARGET_OBJECTS:native_test_x86_vnni_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.vnni.codegen PROPERTIES
  LABELS "host;codegen;baseline;x86;vnni")

set(vnni_core_operations dpbusd dpbusds dpwssd dpwssds)
set(vnni_int8_operations dpbssd dpbssds dpbsud dpbsuds dpbuud dpbuuds)
set(vnni_int16_operations dpwsud dpwsuds dpwusd dpwusds dpwuud dpwuuds)

function(native_vnni_rejection label family width variants rejection kind)
  if(family EQUAL 0 OR family EQUAL 1)
    set(operations ${vnni_core_operations})
  elseif(family EQUAL 2)
    set(operations ${vnni_int8_operations})
  else()
    set(operations ${vnni_int16_operations})
  endif()
  set(sources)
  set(expected)
  foreach(operation IN LISTS operations)
    foreach(variant IN LISTS variants)
      set(source "${CMAKE_CURRENT_BINARY_DIR}/reject-${label}-${operation}-${variant}.cc")
      file(GENERATE OUTPUT "${source}" CONTENT
        "#define NATIVE_VNNI_OPERATION ${operation}\n#define NATIVE_VNNI_VARIANT ${variant}\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/negative.cc\"\n")
      list(APPEND sources "${source}")
      if(variant EQUAL 1)
        list(APPEND expected --operation mask_${operation})
      elseif(variant EQUAL 2)
        list(APPEND expected --operation maskz_${operation})
      else()
        list(APPEND expected --operation ${operation})
      endif()
    endforeach()
  endforeach()
  set(target native_test_x86_vnni_reject_${label})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL ${sources})
  if(ARGC GREATER 6 AND ARGV6 STREQUAL "module")
    # Match the module provider's baseline; instruction targets remain local to
    # the rejected functions, including explicit feature removal.
    target_link_libraries(${target} PRIVATE native::minimal)
    target_compile_definitions(${target} PRIVATE NATIVE_VNNI_IMPORT=1)
    set_target_properties(${target} PROPERTIES
      CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
    set(positive native_test_x86_vnni_module_objects)
  else()
    native_vnni_textual_probe(${target})
    set(positive native_test_x86_vnni_codegen)
  endif()
  target_compile_definitions(${target} PRIVATE NATIVE_VNNI_FAMILY=${family}
    NATIVE_VNNI_WIDTH=${width} NATIVE_VNNI_REJECT=${rejection})
  add_test(NAME native.x86.vnni.reject_${label}
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --positive ${positive} --negative ${target}
      --kind ${kind} ${expected} --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${label}-$<CONFIG>.log")
  set_tests_properties(native.x86.vnni.reject_${label} PROPERTIES
    RUN_SERIAL TRUE TIMEOUT 180 LABELS "host;compile;negative;baseline;x86;vnni")
endfunction()

foreach(width IN ITEMS 128 256)
  native_vnni_rejection(feature_vex${width} 0 ${width} "0" 1 feature)
  native_vnni_rejection(target_vex${width} 0 ${width} "0" 4 target)
  native_vnni_rejection(feature_evex_no_vl${width} 1 ${width} "0;1;2" 3 feature)
  native_vnni_rejection(target_evex_no_vl${width} 1 ${width} "0;1;2" 6 target)
  native_vnni_rejection(feature_masks_from_vex${width} 1 ${width} "1;2" 11 feature)
  foreach(family IN ITEMS 2 3)
    native_vnni_rejection(feature_extension${family}_${width} ${family} ${width} "0" 1 feature)
    native_vnni_rejection(target_extension${family}_${width} ${family} ${width} "0" 4 target)
  endforeach()
endforeach()

# Actual importer failures check that exported constraints, deleted conversion
# overloads and always-inline target requirements survive the module boundary.
foreach(family RANGE 0 3)
  set(variants "0")
  if(family EQUAL 1)
    set(variants "0;1;2")
  endif()
  native_vnni_rejection(module_feature${family} ${family} 256 "${variants}" 1 feature module)
  native_vnni_rejection(module_target${family} ${family} 256 "${variants}" 4 target module)
  native_vnni_rejection(module_shape${family} ${family} 256 "${variants}" 8 shape module)
endforeach()
foreach(width IN ITEMS 128 256 512)
  native_vnni_rejection(feature_evex${width} 1 ${width} "0;1;2" 1 feature)
  native_vnni_rejection(feature_evex_no_f${width} 1 ${width} "0;1;2" 2 feature)
  native_vnni_rejection(target_evex${width} 1 ${width} "0;1;2" 4 target)
endforeach()
native_vnni_rejection(target_evex_no_f512 1 512 "0;1;2" 5 target)
foreach(family IN ITEMS 2 3)
  native_vnni_rejection(feature_extension${family}_from_core ${family} 256 "0" 10 feature)
  native_vnni_rejection(feature_extension${family}_from_other ${family} 256 "0" 12 feature)
  native_vnni_rejection(shape_extension${family}_512 ${family} 512 "0" 13 shape)
endforeach()

# Same-size Clang vector conversions can otherwise choose a different overload.
# Compile each rejected operand position, not just a result-type concept.
foreach(family RANGE 0 3)
  set(variants "0")
  if(family EQUAL 1)
    set(variants "0;1;2")
    native_vnni_rejection(shape_float_evex512 1 512 "0;1;2" 7 shape)
  endif()
  native_vnni_rejection(shape_float${family}_128 ${family} 128 "${variants}" 7 shape)
  native_vnni_rejection(shape_float${family}_256 ${family} 256 "${variants}" 7 shape)
  native_vnni_rejection(shape_a${family}_256 ${family} 256 "${variants}" 8 shape)
  native_vnni_rejection(shape_acc${family}_256 ${family} 256 "${variants}" 14 shape)
  native_vnni_rejection(shape_b${family}_256 ${family} 256 "${variants}" 15 shape)
  native_vnni_rejection(shape_width${family}_256 ${family} 256 "${variants}" 9 shape)
endforeach()
