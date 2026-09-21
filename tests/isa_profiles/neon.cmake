# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
foreach(kind IN ITEMS header import)
  set(target native_profile_neon_${kind})
  add_library(${target} OBJECT bridge.cc)
  target_link_libraries(${target} PRIVATE native::common native::neon)
  native_target_profile(${target} NEON)
  target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core_regression")
  if(kind STREQUAL "header")
    set(interface 0)
  else()
    set(interface 1)
  endif()
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_PROFILE=128
    NATIVE_TEST_INTERFACE=${interface} NATIVE_TEST_ENTRY=profile_neon_${kind})
endforeach()
add_executable(native_test_neon neon.cc
  $<TARGET_OBJECTS:native_profile_neon_header> $<TARGET_OBJECTS:native_profile_neon_import>)
target_link_libraries(native_test_neon PRIVATE native::native native::common native::neon)
native_target_profile(native_test_neon NEON)
add_test(NAME native.profiles.neon COMMAND native_test_neon
  "${CMAKE_CURRENT_BINARY_DIR}/neon.bin")
set_tests_properties(native.profiles.neon PROPERTIES LABELS "host;modules;profiles;exact" TIMEOUT 180)

target_include_directories(native_test_neon PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core_regression")
