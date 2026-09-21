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

find_package(Python3 REQUIRED COMPONENTS Interpreter)
foreach(interface IN ITEMS header module)
  foreach(kind IN ITEMS isa enum)
    set(target native_kernel_reject_foreign_${interface}_${kind})
    add_library(${target} OBJECT EXCLUDE_FROM_ALL foreign.cc)
    target_compile_definitions(${target} PRIVATE
      NATIVE_TEST_IMPORT=$<STREQUAL:${interface},module>
      NATIVE_TEST_ENUM=$<STREQUAL:${kind},enum>)
    if(interface STREQUAL "header")
      target_link_libraries(${target} PRIVATE native::headers)
      set_target_properties(${target} PROPERTIES CXX_STANDARD 20 CXX_SCAN_FOR_MODULES OFF)
    else()
      target_link_libraries(${target} PRIVATE native::native)
    endif()
    add_test(NAME native.kernel_policy.reject_foreign_${interface}_${kind}
      COMMAND "${Python3_EXECUTABLE}"
      "${PROJECT_SOURCE_DIR}/tests/source_targets/check_negative.py"
      --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
      --target "${target}" --config "$<CONFIG>"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-foreign-${interface}-${kind}-$<CONFIG>.log"
      --diagnostic "value architecture must match the compiler target family")
    set_tests_properties(native.kernel_policy.reject_foreign_${interface}_${kind}
      PROPERTIES LABELS "host;kernel_policy;negative" RUN_SERIAL TRUE)
  endforeach()
endforeach()
