# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

if(WIN32)
  foreach(order IN ITEMS sdk_first import_first)
    add_executable(native_test_windows_headers_${order} coexist.cc)
    target_link_libraries(native_test_windows_headers_${order} PRIVATE native::native)
    target_compile_definitions(native_test_windows_headers_${order} PRIVATE
      "NATIVE_WINDOWS_HEADER_FIRST=$<STREQUAL:${order},sdk_first>")
    set_target_properties(native_test_windows_headers_${order} PROPERTIES
      INTERPROCEDURAL_OPTIMIZATION FALSE)
    if(COMMAND native_link_clang_cl_asan)
      native_link_clang_cl_asan(native_test_windows_headers_${order})
    endif()
    add_test(NAME native.windows_headers.${order} COMMAND native_test_windows_headers_${order})
    set_tests_properties(native.windows_headers.${order} PROPERTIES
      SKIP_RETURN_CODE 77 LABELS "host;headers;windows;modules")
  endforeach()
endif()
