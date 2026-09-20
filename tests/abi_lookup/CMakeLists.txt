# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_abi_lookup_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native headers)
  enable_testing()
endif()
add_executable(native_test_abi_lookup_header header.cc)
target_link_libraries(native_test_abi_lookup_header PRIVATE native::headers)
target_compile_features(native_test_abi_lookup_header PRIVATE cxx_std_20)
# Keep this test at the metadata header's minimum dialect, even in the host build.
set_target_properties(native_test_abi_lookup_header PROPERTIES CXX_STANDARD 20)
add_executable(native_test_abi_lookup_module module.cc)
target_link_libraries(native_test_abi_lookup_module PRIVATE native::native)
foreach(kind IN ITEMS header module)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(native_test_abi_lookup_${kind})
  endif()
  add_test(NAME native.abi_lookup.${kind} COMMAND native_test_abi_lookup_${kind})
  set_tests_properties(native.abi_lookup.${kind} PROPERTIES LABELS "host;abi_lookup" TIMEOUT 30)
endforeach()

# The lookup diagnoses unknown inherited features before a fallback can match.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_library(native_abi_lookup_reject_minimum OBJECT EXCLUDE_FROM_ALL invalid_minimum.cc)
target_link_libraries(native_abi_lookup_reject_minimum PRIVATE native::headers)
target_compile_features(native_abi_lookup_reject_minimum PRIVATE cxx_std_20)
add_test(NAME native.abi_lookup.reject_minimum COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../source_targets/check_negative.py"
  --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
  --target native_abi_lookup_reject_minimum --config "$<CONFIG>"
  --log "${CMAKE_CURRENT_BINARY_DIR}/reject-minimum-$<CONFIG>.log"
  --diagnostic "ABI policy minimum contains an unregistered ISA feature")
set_tests_properties(native.abi_lookup.reject_minimum PROPERTIES
  LABELS "host;abi_lookup" RUN_SERIAL TRUE TIMEOUT 180)

foreach(case RANGE 0 4)
  add_library(native_target_reject_order_${case} OBJECT EXCLUDE_FROM_ALL invalid_order.cc)
  target_link_libraries(native_target_reject_order_${case} PRIVATE native::headers)
  target_compile_features(native_target_reject_order_${case} PRIVATE cxx_std_20)
  target_compile_definitions(native_target_reject_order_${case} PRIVATE INVALID_ORDER=${case})
  add_test(NAME native.abi_lookup.reject_order_${case} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../source_targets/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
    --target native_target_reject_order_${case} --config "$<CONFIG>"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-order-${case}-$<CONFIG>.log"
    --diagnostic "a later target is shadowed by an earlier one")
  set_tests_properties(native.abi_lookup.reject_order_${case} PROPERTIES
    LABELS "host;abi_lookup" RUN_SERIAL TRUE TIMEOUT 180)
endforeach()
