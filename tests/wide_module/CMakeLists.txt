# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(simd_wide_module_check LANGUAGES CXX)
# CMake's module mapping already selects the language; clang-cl diagnoses -TP
# as unused for a .ccm module when strict warnings are errors.
string(REPLACE " -TP " " " CMAKE_CXX_COMPILE_OBJECT "${CMAKE_CXX_COMPILE_OBJECT}")
enable_testing()
get_filename_component(simd_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_library(wide_module STATIC)
target_sources(wide_module PUBLIC FILE_SET CXX_MODULES
  BASE_DIRS "${simd_root}/src/cxx/modules" FILES "${simd_root}/src/cxx/modules/simd.wide.ccm")
target_compile_features(wide_module PUBLIC cxx_std_26)
target_include_directories(wide_module PRIVATE "${simd_root}/src/cxx" "${simd_root}/src/shared")
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(wide_module PUBLIC /W4 /WX /EHs-c-)
  target_compile_definitions(wide_module PUBLIC _HAS_EXCEPTIONS=0)
else()
  target_compile_options(wide_module PUBLIC -Wall -Wextra -Werror -fno-exceptions)
endif()
set_target_properties(wide_module PROPERTIES CXX_EXTENSIONS OFF)
add_executable(wide_import import.cc no_headers.cc)
target_link_libraries(wide_import PRIVATE wide_module)
add_test(NAME wide_import COMMAND wide_import)

add_executable(wide_batch batch.cc)
target_link_libraries(wide_batch PRIVATE wide_module)
add_test(NAME wide_batch COMMAND wide_batch)
