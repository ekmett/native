# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(simd_kernel_half_requirements LANGUAGES CXX)
  find_package(simd CONFIG REQUIRED COMPONENTS simd headers)
  enable_testing()
endif()
add_executable(simd_test_kernel_half_requirements native.cc)
target_link_libraries(simd_test_kernel_half_requirements PRIVATE simd::simd)
if(COMMAND simd_link_clang_cl_asan)
  simd_link_clang_cl_asan(simd_test_kernel_half_requirements)
endif()
add_test(NAME simd.kernel_policy.half_requirements COMMAND simd_test_kernel_half_requirements)
set_tests_properties(simd.kernel_policy.half_requirements PROPERTIES
  LABELS "host;kernel_policy;modules" SKIP_RETURN_CODE 77)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
foreach(kind IN ITEMS fp16 bf16)
  add_library(simd_kernel_reject_${kind} OBJECT EXCLUDE_FROM_ALL reject.cc)
  target_link_libraries(simd_kernel_reject_${kind} PRIVATE simd::simd)
  target_compile_definitions(simd_kernel_reject_${kind} PRIVATE TEST_FP16=$<STREQUAL:${kind},fp16>)
  add_test(NAME simd.kernel_policy.reject_${kind} COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../source_targets/check_negative.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
    --target simd_kernel_reject_${kind} --config "$<CONFIG>"
    --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${kind}-$<CONFIG>.log"
    --diagnostic "requires target feature|target specific option mismatch")
  set_tests_properties(simd.kernel_policy.reject_${kind} PROPERTIES
    LABELS "host;kernel_policy;modules" RUN_SERIAL TRUE SKIP_RETURN_CODE 77)
endforeach()
