# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(simd_half_storage_consumer LANGUAGES CXX)
find_package(simd CONFIG REQUIRED COMPONENTS simd)
option(HALF_STORAGE_PCH "Precompile consumer standard headers" ON)
option(HALF_STORAGE_IPO "Enable consumer IPO" OFF)
add_library(half_storage_body OBJECT half_storage.cc omnibus.cc)
add_executable(half_storage main.cc $<TARGET_OBJECTS:half_storage_body>)
target_link_libraries(half_storage_body PRIVATE simd::simd simd::common)
target_link_libraries(half_storage PRIVATE simd::simd simd::common)
# The omnibus includes all configured native profiles, even for scalar use.
simd_target_omnibus(half_storage_body)
get_target_property(half_storage_profiles simd::simd SIMD_OMNIBUS_PROFILES)
foreach(profile IN LISTS half_storage_profiles)
  target_compile_definitions(half_storage PRIVATE HALF_REQUIRED_${profile}=1)
endforeach()
set_target_properties(half_storage PROPERTIES CXX_SCAN_FOR_MODULES ON CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION FALSE)
set_target_properties(half_storage_body PROPERTIES CXX_SCAN_FOR_MODULES ON CXX_EXTENSIONS OFF
  INTERPROCEDURAL_OPTIMIZATION ${HALF_STORAGE_IPO})
if(HALF_STORAGE_PCH)
  target_precompile_headers(half_storage_body PRIVATE <bit> <concepts> <cstdint> <limits> <type_traits>)
endif()
enable_testing()
add_test(NAME half.storage COMMAND half_storage)
set_tests_properties(half.storage PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 LABELS "host;modules;half;exact")

if(CMAKE_GENERATOR STREQUAL "Ninja")
  add_test(NAME half.shared_bmi COMMAND "${CMAKE_COMMAND}"
    "-DBUILD_DIR=${CMAKE_BINARY_DIR}" "-DMODULES=simd.scalar;simd.wide;simd.numerics"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/../profile_package/check_bmi.cmake")
  set_tests_properties(half.shared_bmi PROPERTIES LABELS "host;modules;half;profiles")
endif()
