# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(simd_cpuid_package_check LANGUAGES CXX)
find_package(simd CONFIG REQUIRED COMPONENTS simd)
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  message(FATAL_ERROR "The CPUID admission fixture requires x86-64")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()
add_executable(cpuid_package ../cpuid_module/admission.cc)
target_link_libraries(cpuid_package PRIVATE simd::common)
set_target_properties(cpuid_package PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
add_test(NAME cpuid.package COMMAND cpuid_package)
