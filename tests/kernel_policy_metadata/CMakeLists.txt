# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
add_executable(native_test_kernel_policy_metadata metadata.cc)
target_link_libraries(native_test_kernel_policy_metadata PRIVATE native::headers)
target_include_directories(native_test_kernel_policy_metadata PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_compile_features(native_test_kernel_policy_metadata PRIVATE cxx_std_20)
set_target_properties(native_test_kernel_policy_metadata PROPERTIES CXX_STANDARD 20)
if(COMMAND native_link_clang_cl_asan)
  native_link_clang_cl_asan(native_test_kernel_policy_metadata)
endif()
add_test(NAME native.kernel_policy.metadata COMMAND native_test_kernel_policy_metadata)
set_tests_properties(native.kernel_policy.metadata PROPERTIES LABELS "host;kernel_policy")
