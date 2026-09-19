# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
add_executable(simd_test_kernel_policy_metadata metadata.cc)
target_link_libraries(simd_test_kernel_policy_metadata PRIVATE simd::headers)
target_include_directories(simd_test_kernel_policy_metadata PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_compile_features(simd_test_kernel_policy_metadata PRIVATE cxx_std_20)
set_target_properties(simd_test_kernel_policy_metadata PROPERTIES CXX_STANDARD 20)
if(COMMAND simd_link_clang_cl_asan)
  simd_link_clang_cl_asan(simd_test_kernel_policy_metadata)
endif()
add_test(NAME simd.kernel_policy.metadata COMMAND simd_test_kernel_policy_metadata)
set_tests_properties(simd.kernel_policy.metadata PROPERTIES LABELS "host;kernel_policy")
