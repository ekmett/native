# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
# Both consumers retain the configured project minimum. Native constructors are
# called only from attributed functions, without per-test ISA compiler flags.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
foreach(kind IN ITEMS header module)
  add_executable(simd_test_wide_construction_${kind} generic.cc native.cc)
  target_link_libraries(simd_test_wide_construction_${kind} PRIVATE simd::simd)
  if(kind STREQUAL "module")
    target_compile_definitions(simd_test_wide_construction_${kind} PRIVATE SIMD_TEST_IMPORT=1)
  endif()
  if(COMMAND simd_link_clang_cl_asan)
    simd_link_clang_cl_asan(simd_test_wide_construction_${kind})
  endif()
  add_test(NAME simd.wide_construction.${kind} COMMAND simd_test_wide_construction_${kind})
  set_tests_properties(simd.wide_construction.${kind} PROPERTIES LABELS "host;modules;construction" TIMEOUT 60)
  add_test(NAME simd.wide_construction.${kind}_throw COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/check_throw.py" "$<TARGET_FILE:simd_test_wide_construction_${kind}>")
  set_tests_properties(simd.wide_construction.${kind}_throw PROPERTIES LABELS "host;modules;construction;exceptions" TIMEOUT 60)
endforeach()
