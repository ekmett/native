# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

cmake_minimum_required(VERSION 4.4)
project(native VERSION 0.0.1 LANGUAGES NONE)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NATIVE_BUILD_HOST "Build the C++26 SIMD/module library" ON)
option(NATIVE_BUILD_TESTS "Build library tests" ${PROJECT_IS_TOP_LEVEL})
option(NATIVE_BUILD_DOCS "Build API documentation (requires Doxygen)" OFF)
# Retained as a compatibility setting; single-module providers build BMIs directly.
option(NATIVE_ENABLE_PCH "Compatibility setting (module providers do not use PCH)" OFF)
option(NATIVE_ENABLE_EXCEPTIONS "Enable C++ exceptions in the library and its consumers" OFF)
option(NATIVE_ENABLE_IPO "Enable supported host interprocedural optimization" OFF)
option(NATIVE_ENABLE_ASAN "Instrument host targets with AddressSanitizer" OFF)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(native_default_isa NEON)
else()
  set(native_default_isa AVX2)
endif()
set(NATIVE_TEST_ISA "${native_default_isa}" CACHE STRING "ISA used by the native regression tests")
set_property(CACHE NATIVE_TEST_ISA PROPERTY STRINGS AVX2 AVX512 NEON)
if(native_default_isa STREQUAL "NEON")
  set(native_default_profiles "NEON;NEON_FP16;NEON_BF16")
else()
  set(native_default_profiles "AVX2;AVX512;AVX512_BF16;AVX512_FP16")
endif()
set(NATIVE_PROFILES "${native_default_profiles}" CACHE STRING "Profiles exercised by the regression tests; the hub exposes all supported host profiles")
list(REMOVE_DUPLICATES NATIVE_PROFILES)
foreach(profile IN LISTS NATIVE_PROFILES)
  if(NOT profile MATCHES "^(AVX2|AVX512|AVX512_BF16|AVX512_FP16|NEON|NEON_FP16|NEON_BF16)$")
    message(FATAL_ERROR "NATIVE_PROFILES entries must be AVX2, AVX512, AVX512_BF16, AVX512_FP16, NEON, NEON_FP16, or NEON_BF16.")
  endif()
endforeach()
if(NATIVE_BUILD_HOST)
  enable_language(CXX)
else()
  # GNUInstallDirs cannot infer an architecture in this language-free package.
  set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Library installation directory")
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Configuration, attributes and source-level target generation are textual.
# Downstream libraries can consume them without enabling the host module build.
add_library(native_headers INTERFACE)
add_library(native::headers ALIAS native_headers)
set_target_properties(native_headers PROPERTIES EXPORT_NAME headers)
# Named swizzle properties use Clang's Microsoft property extension. Carry this
# requirement with the headers as well as modules, including relocated consumers.
target_compile_options(native_headers INTERFACE
  "$<$<AND:$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>,$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:-fms-extensions>")
target_sources(native_headers INTERFACE
  FILE_SET headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src"
  FILES src/native/config.h src/native/attributes.h src/native/isa.h src/native/targets.h
    src/native/mask_traits.h src/native/wasm/features.h)
target_include_directories(native_headers INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
install(TARGETS native_headers EXPORT nativeHeaderTargets
  FILE_SET headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(EXPORT nativeHeaderTargets NAMESPACE native:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/native")
export(EXPORT nativeHeaderTargets NAMESPACE native:: FILE "${CMAKE_CURRENT_BINARY_DIR}/nativeHeaderTargets.cmake")

if(NATIVE_BUILD_HOST)
  include(etc/cmake/Compiler.cmake)
  if(NATIVE_BUILD_TESTS)
    enable_testing()
    # Boundary tests inspect provider commands as well as test executables.
    # Enable the database before creating the module targets.
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  endif()
  add_subdirectory(src)
endif()
if(NATIVE_BUILD_DOCS)
  add_subdirectory(doc)
endif()
if(NATIVE_BUILD_HOST AND NATIVE_BUILD_TESTS)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/core_regression/CMakeLists.txt")
    add_subdirectory(tests/core_regression)
  else()
    message(FATAL_ERROR "NATIVE_BUILD_TESTS requires tests/core_regression sources.")
  endif()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/isa_profiles/CMakeLists.txt")
    add_subdirectory(tests/isa_profiles)
  endif()
  add_subdirectory(tests/bf16_profile)
  add_subdirectory(tests/neon_fp16)
  add_subdirectory(tests/neon_bf16)
  add_subdirectory(tests/avx512_fp16)
  add_subdirectory(tests/static_string)
  add_subdirectory(tests/apple_headers)
  add_subdirectory(tests/windows_headers)
  add_subdirectory(tests/rounding)
  add_subdirectory(tests/compaction)
  add_subdirectory(tests/source_targets)
  add_subdirectory(tests/native_cutover)
  add_subdirectory(tests/native_mask_traits)
  add_subdirectory(tests/instruction_storage)
  add_subdirectory(tests/constexpr_float)
  add_subdirectory(tests/half_constexpr)
  add_subdirectory(tests/floating_instructions_constexpr)
  add_subdirectory(tests/float_constexpr)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    add_subdirectory(tests/x86_bmi1)
    add_subdirectory(tests/x86_bmi2)
    add_subdirectory(tests/x86_popcnt)
    add_subdirectory(tests/x86_lzcnt)
    add_subdirectory(tests/x86_crc32c)
    add_subdirectory(tests/x86_aes)
    add_subdirectory(tests/x86_gfni)
    add_subdirectory(tests/x86_vpopcntdq)
    add_subdirectory(tests/x86_avx512cd)
    add_subdirectory(tests/x86_bitalg)
    add_subdirectory(tests/x86_vbmi)
    add_subdirectory(tests/x86_vbmi2)
    add_subdirectory(tests/x86_sha)
    add_subdirectory(tests/x86_vaes)
    add_subdirectory(tests/x86_ifma)
    add_subdirectory(tests/x86_adx)
    add_subdirectory(tests/x86_pclmul)
    add_subdirectory(tests/x86_vnni)
    add_subdirectory(tests/x86_f16c)
    add_subdirectory(tests/x86_scalar_defaults)
    add_subdirectory(tests/x86_constexpr)
    add_subdirectory(tests/x86_integer_constexpr)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    add_subdirectory(tests/arm_neon)
    add_subdirectory(tests/arm_dotprod)
    add_subdirectory(tests/arm_rdm)
    add_subdirectory(tests/arm_bf16)
    add_subdirectory(tests/arm_fp16fml)
    add_subdirectory(tests/arm_fcma)
    add_subdirectory(tests/arm_i8mm)
    add_subdirectory(tests/arm_constexpr)
    add_subdirectory(tests/arm_scalar_defaults)
    add_subdirectory(tests/arm_crc)
    add_subdirectory(tests/arm_crypto)
    add_subdirectory(tests/arm_sm_crypto)
  endif()
  add_subdirectory(tests/wasm_features)
  add_subdirectory(tests/abi_lookup)
  add_subdirectory(tests/wide_construction)
  add_subdirectory(tests/exp_policy_refinement)
  add_subdirectory(tests/kernel_policy_metadata)
  add_subdirectory(tests/kernel_half_requirements)
  add_subdirectory(tests/kernel_integer_refinement)
  add_subdirectory(tests/kernel_policy_refinement)
endif()

configure_file(etc/cmake/nativeProfile.cmake "${CMAKE_CURRENT_BINARY_DIR}/nativeProfile.cmake" COPYONLY)
configure_package_config_file(etc/cmake/nativeConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/nativeConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/native"
  PATH_VARS CMAKE_INSTALL_DATADIR)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/nativeConfigVersion.cmake"
  VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/nativeProfile.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/nativeConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/nativeConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/native")

install(FILES LICENSE.md DESTINATION "${CMAKE_INSTALL_DATADIR}/native")

install(FILES src/native/wasm/features.mjs
  DESTINATION "${CMAKE_INSTALL_DATADIR}/native/wasm")
