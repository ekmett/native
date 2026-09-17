# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
foreach(kind IN ITEMS header import)
  set(target simd_profile_neon_${kind})
  add_library(${target} OBJECT bridge.cc)
  target_link_libraries(${target} PRIVATE simd::common simd::neon)
  simd_target_profile(${target} NEON)
  target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core_regression")
  if(kind STREQUAL "header")
    set(interface 0)
  else()
    set(interface 1)
  endif()
  target_compile_definitions(${target} PRIVATE SIMD_TEST_PROFILE=128
    SIMD_TEST_INTERFACE=${interface} SIMD_TEST_ENTRY=profile_neon_${kind})
endforeach()
add_executable(simd_test_neon neon.cc
  $<TARGET_OBJECTS:simd_profile_neon_header> $<TARGET_OBJECTS:simd_profile_neon_import>)
target_link_libraries(simd_test_neon PRIVATE simd::simd simd::common simd::neon)
simd_target_profile(simd_test_neon NEON)
add_test(NAME simd.profiles.neon COMMAND simd_test_neon
  "${CMAKE_CURRENT_BINARY_DIR}/neon.bin")
set_tests_properties(simd.profiles.neon PROPERTIES LABELS "host;modules;profiles;exact" TIMEOUT 180)

target_include_directories(simd_test_neon PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core_regression")
