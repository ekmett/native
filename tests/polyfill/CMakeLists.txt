# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
foreach(mode IN ITEMS header module)
  add_executable(native_test_polyfill_${mode} main.cc)
  target_link_libraries(native_test_polyfill_${mode} PRIVATE native::native)
  if(mode STREQUAL "header")
    target_compile_definitions(native_test_polyfill_${mode} PRIVATE NATIVE_POLYFILL_HEADERS=1)
  endif()
  add_test(NAME native.polyfill.${mode} COMMAND native_test_polyfill_${mode})
  set_tests_properties(native.polyfill.${mode} PROPERTIES LABELS "host;polyfill" TIMEOUT 30)
endforeach()
