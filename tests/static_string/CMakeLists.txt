# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_static_string_check LANGUAGES CXX)
  enable_testing()
  get_filename_component(native_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
  option(NATIVE_ENABLE_EXCEPTIONS "Enable the checked-access exception control" OFF)
  option(NATIVE_ENABLE_ASAN "Instrument the focused module test" OFF)
  include("${native_root}/etc/cmake/Compiler.cmake")
  add_library(static_string_module STATIC)
  native_host_settings(static_string_module)
  target_include_directories(static_string_module PRIVATE "${native_root}/src")
  target_sources(static_string_module PUBLIC FILE_SET CXX_MODULES
    BASE_DIRS "${native_root}/src"
    FILES "${native_root}/src/native.static_string.ccm")
  set(static_string_provider static_string_module)
else()
  # Keep baseline module metadata independent of the native omnibus. CMake may
  # otherwise select its highest-profile synthesized common BMI for this target.
  set(static_string_provider native::common)
endif()
# clang-cl module mapping already selects the language; redundant -TP is warned.
string(REPLACE " -TP " " " CMAKE_CXX_COMPILE_OBJECT "${CMAKE_CXX_COMPILE_OBJECT}")
add_executable(static_string_import main.cc peer.cc no_headers.cc)
native_host_settings(static_string_import)
set_target_properties(static_string_import PROPERTIES CXX_SCAN_FOR_MODULES ON)
target_link_libraries(static_string_import PRIVATE ${static_string_provider})
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  set_property(TARGET static_string_import APPEND_STRING PROPERTY COMPILE_FLAGS " /W4 /WX /clang:-Wpedantic")
else()
  target_compile_options(static_string_import PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
native_link_clang_cl_asan(static_string_import)
add_test(NAME static_string.import COMMAND static_string_import)
