# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(native_half_storage_consumer LANGUAGES CXX)
find_package(native CONFIG REQUIRED COMPONENTS native)
option(HALF_STORAGE_PCH "Precompile consumer standard headers" ON)
option(HALF_STORAGE_IPO "Enable consumer IPO" OFF)
add_library(half_storage_body OBJECT half_storage.cc omnibus.cc)
add_executable(half_storage main.cc $<TARGET_OBJECTS:half_storage_body>)
target_link_libraries(half_storage_body PRIVATE native::native native::common)
target_link_libraries(half_storage PRIVATE native::native native::common)
# Scalar storage conversions require only the configured project minimum.
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
    "-DBUILD_DIR=${CMAKE_BINARY_DIR}" "-DMODULES=native;native.wide;native.numerics"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/../profile_package/check_bmi.cmake")
  set_tests_properties(half.shared_bmi PROPERTIES LABELS "host;modules;half;profiles")
endif()
