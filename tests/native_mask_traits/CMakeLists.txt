# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Type mapping is independent of the executing CPU. Vector assertions remain
# compile-only, and the scalar smoke tests need no optional ISA profile.
foreach(suite IN ITEMS header import types scalar mixed)
  set(target native_test_mask_traits_${suite})
  add_executable(${target} ${suite}.cc)
  if(suite STREQUAL "types" OR suite STREQUAL "scalar")
    target_link_libraries(${target} PRIVATE native::minimal)
  else()
    target_link_libraries(${target} PRIVATE native::native)
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.mask_traits.${suite} COMMAND ${target})
  set_tests_properties(native.mask_traits.${suite} PROPERTIES
    LABELS "host;modules;baseline;mask_traits")
endforeach()
