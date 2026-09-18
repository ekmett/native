# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# A CPU-only SDK coexistence check. Neither translation unit imports a module
# or links Metal: the SDK and installed implementation headers must coexist.
if(APPLE)
  string(TOLOWER "${SIMD_TEST_ISA}" apple_test_architecture)
  foreach(order IN ITEMS sdk_first library_first)
    add_executable(simd_test_apple_headers_${order} coexist.cc)
    simd_host_settings(simd_test_apple_headers_${order})
    simd_target_profile(simd_test_apple_headers_${order} "${SIMD_TEST_ISA}")
    target_link_libraries(simd_test_apple_headers_${order} PRIVATE simd::headers)
    target_compile_definitions(simd_test_apple_headers_${order} PRIVATE
      "SIMD_APPLE_TEST_ARCH=simd::${apple_test_architecture}")
    if(order STREQUAL "sdk_first")
      target_compile_definitions(simd_test_apple_headers_${order} PRIVATE SIMD_APPLE_HEADER_FIRST=1)
    else()
      target_compile_definitions(simd_test_apple_headers_${order} PRIVATE SIMD_APPLE_HEADER_FIRST=0)
    endif()
    add_test(NAME simd.apple_headers.${order} COMMAND simd_test_apple_headers_${order})
    set_tests_properties(simd.apple_headers.${order} PROPERTIES LABELS "host;headers;apple")
  endforeach()
endif()
