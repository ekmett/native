# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(simd_public_headers LANGUAGES CXX)
find_package(simd CONFIG REQUIRED COMPONENTS headers)
enable_testing()
add_executable(public_headers headers.cc)
target_link_libraries(public_headers PRIVATE simd::headers)
add_test(NAME headers.public_boundary COMMAND public_headers)
add_executable(public_metadata metadata.cc)
target_link_libraries(public_metadata PRIVATE simd::headers)
target_compile_features(public_metadata PRIVATE cxx_std_20)
add_test(NAME headers.metadata COMMAND public_metadata)
