# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(native_package_tests LANGUAGES CXX)
find_package(native CONFIG REQUIRED COMPONENTS native headers)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

# One imported module graph serves the relocated header, hub and API consumers.
foreach(suite IN ITEMS public_headers omnibus api)
  add_subdirectory("../${suite}" "${suite}")
endforeach()

add_test(NAME package.shared_bmi COMMAND "${CMAKE_COMMAND}"
  "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
  "-DMODULES=native.wide;native.simd;native.numerics;native.features;native"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/../profile_package/check_bmi.cmake")
set_tests_properties(package.shared_bmi PROPERTIES LABELS "host;modules;package")
