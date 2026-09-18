# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

if(WIN32)
  foreach(order IN ITEMS sdk_first import_first)
    add_executable(simd_test_windows_headers_${order} coexist.cc)
    target_link_libraries(simd_test_windows_headers_${order} PRIVATE simd::simd)
    target_compile_definitions(simd_test_windows_headers_${order} PRIVATE
      "SIMD_WINDOWS_HEADER_FIRST=$<STREQUAL:${order},sdk_first>")
    set_target_properties(simd_test_windows_headers_${order} PROPERTIES
      INTERPROCEDURAL_OPTIMIZATION FALSE)
    if(COMMAND simd_link_clang_cl_asan)
      simd_link_clang_cl_asan(simd_test_windows_headers_${order})
    endif()
    add_test(NAME simd.windows_headers.${order} COMMAND simd_test_windows_headers_${order})
    set_tests_properties(simd.windows_headers.${order} PROPERTIES
      SKIP_RETURN_CODE 77 LABELS "host;headers;windows;modules")
  endforeach()
endif()
