# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

cmake_minimum_required(VERSION 4.4)
project(simd VERSION 0.0.1 LANGUAGES NONE)

option(SIMD_BUILD_HOST "Build the C++26 SIMD/module library" ON)
option(SIMD_BUILD_TESTS "Build focused new-core tests" ${PROJECT_IS_TOP_LEVEL})
option(SIMD_BUILD_DOCS "Build API documentation (requires Doxygen)" OFF)
option(SIMD_ENABLE_PCH "Precompile common host headers" OFF)
option(SIMD_ENABLE_EXCEPTIONS "Enable C++ exceptions in the new core and its consumers" OFF)
option(SIMD_ENABLE_IPO "Enable supported host interprocedural optimization" OFF)
option(SIMD_ENABLE_ASAN "Instrument host targets with AddressSanitizer" OFF)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(simd_default_isa NEON)
else()
  set(simd_default_isa AVX2)
endif()
set(SIMD_TEST_ISA "${simd_default_isa}" CACHE STRING "ISA used by the native regression tests")
set_property(CACHE SIMD_TEST_ISA PROPERTY STRINGS AVX2 AVX512 NEON)
if(simd_default_isa STREQUAL "NEON")
  set(simd_default_profiles "NEON")
else()
  set(simd_default_profiles "AVX2;AVX512")
endif()
set(SIMD_PROFILES "${simd_default_profiles}" CACHE STRING "Named module profiles included in the archive")
list(REMOVE_DUPLICATES SIMD_PROFILES)
foreach(profile IN LISTS SIMD_PROFILES)
  if(NOT profile MATCHES "^(AVX2|AVX512|NEON)$")
    message(FATAL_ERROR "SIMD_PROFILES entries must be AVX2, AVX512, or NEON.")
  endif()
endforeach()
if(SIMD_BUILD_HOST)
  enable_language(CXX)
else()
  # GNUInstallDirs cannot infer an architecture in this language-free package.
  set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Library installation directory")
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Macro definitions and module implementation inputs are ordinary headers.
# Downstream libraries can consume them without enabling the host module build.
add_library(simd_headers INTERFACE)
add_library(simd::headers ALIAS simd_headers)
set_target_properties(simd_headers PROPERTIES EXPORT_NAME headers)
# Named swizzle properties use Clang's Microsoft property extension. Carry this
# requirement with the headers as well as modules, including relocated consumers.
target_compile_options(simd_headers INTERFACE
  "$<$<AND:$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>,$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:-fms-extensions>")
file(GLOB_RECURSE SIMD_SHARED_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/shared/simd/*.h")
file(GLOB_RECURSE SIMD_CXX_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/cxx/simd/*.h")
target_sources(simd_headers INTERFACE
  FILE_SET shared_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/shared" FILES ${SIMD_SHARED_HEADERS}
  FILE_SET cxx_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/cxx" FILES ${SIMD_CXX_HEADERS})
target_include_directories(simd_headers INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/shared>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cxx>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/shared>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cxx>")
install(TARGETS simd_headers EXPORT simdHeaderTargets
  FILE_SET shared_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/shared"
  FILE_SET cxx_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cxx")
install(EXPORT simdHeaderTargets NAMESPACE simd:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/simd")
export(EXPORT simdHeaderTargets NAMESPACE simd:: FILE "${CMAKE_CURRENT_BINARY_DIR}/simdHeaderTargets.cmake")

if(SIMD_BUILD_HOST)
  include(etc/cmake/Compiler.cmake)
  if(SIMD_BUILD_TESTS)
    enable_testing()
  endif()
  add_subdirectory(src)
endif()
if(SIMD_BUILD_DOCS)
  add_subdirectory(doc)
endif()
if(SIMD_BUILD_HOST AND SIMD_BUILD_TESTS)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/core_regression/CMakeLists.txt")
    add_subdirectory(tests/core_regression)
  else()
    message(FATAL_ERROR "SIMD_BUILD_TESTS requires the new tests/core_regression source checkpoint.")
  endif()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/isa_profiles/CMakeLists.txt")
    add_subdirectory(tests/isa_profiles)
  endif()
  add_subdirectory(tests/static_string)
  add_subdirectory(tests/apple_headers)
  add_subdirectory(tests/rounding)
endif()

configure_file(etc/cmake/simdProfile.cmake "${CMAKE_CURRENT_BINARY_DIR}/simdProfile.cmake" COPYONLY)
configure_package_config_file(etc/cmake/simdConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/simdConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/simd")
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/simdConfigVersion.cmake"
  VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/simdProfile.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/simdConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/simdConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/simd")

install(FILES LICENSE.md DESTINATION "${CMAKE_INSTALL_DATADIR}/simd")
