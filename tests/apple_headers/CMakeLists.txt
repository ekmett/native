# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# A CPU-only SDK coexistence check. Neither translation unit imports a module
# or links Metal: the SDK and installed implementation headers must coexist.
if(APPLE)
  string(TOLOWER "${NATIVE_TEST_ISA}" apple_test_architecture)
  foreach(order IN ITEMS sdk_first library_first)
    add_executable(native_test_apple_headers_${order} coexist.cc)
    native_host_settings(native_test_apple_headers_${order})
    native_target_profile(native_test_apple_headers_${order} "${NATIVE_TEST_ISA}")
    target_link_libraries(native_test_apple_headers_${order} PRIVATE native::headers)
    target_compile_definitions(native_test_apple_headers_${order} PRIVATE
      "NATIVE_APPLE_TEST_ARCH=native::${apple_test_architecture}")
    if(order STREQUAL "sdk_first")
      target_compile_definitions(native_test_apple_headers_${order} PRIVATE NATIVE_APPLE_HEADER_FIRST=1)
    else()
      target_compile_definitions(native_test_apple_headers_${order} PRIVATE NATIVE_APPLE_HEADER_FIRST=0)
    endif()
    add_test(NAME native.apple_headers.${order} COMMAND native_test_apple_headers_${order})
    set_tests_properties(native.apple_headers.${order} PROPERTIES LABELS "host;headers;apple")
  endforeach()
endif()
