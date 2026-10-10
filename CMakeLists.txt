# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

cmake_minimum_required(VERSION 4.4)
project(native VERSION 0.0.1 LANGUAGES NONE)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NATIVE_BUILD_HOST "Build the C++26 SIMD/module library" ON)
option(NATIVE_BUILD_TESTS "Build library tests" ${PROJECT_IS_TOP_LEVEL})
option(NATIVE_TEST_EXTENDED "Build exhaustive, codegen and compiler-rejection tests" OFF)
option(NATIVE_BUILD_DOCS "Build API documentation (requires Doxygen)" OFF)
# Retained as a compatibility setting; single-module providers build BMIs directly.
option(NATIVE_ENABLE_PCH "Compatibility setting (module providers do not use PCH)" OFF)
option(NATIVE_ENABLE_EXCEPTIONS "Enable C++ exceptions in the library and its consumers" ON)
option(NATIVE_ENABLE_IPO "Enable supported host interprocedural optimization" OFF)
option(NATIVE_ENABLE_ASAN "Instrument host targets with AddressSanitizer" OFF)
option(NATIVE_ENABLE_COVERAGE "Collect runtime test coverage with Clang and grcov" OFF)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^wasm(32|64)$")
  set(native_default_isa WASM_SIMD128)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(native_default_isa NEON)
else()
  set(native_default_isa AVX2)
endif()
set(NATIVE_TEST_ISA "${native_default_isa}" CACHE STRING "ISA used by the native regression tests")
set_property(CACHE NATIVE_TEST_ISA PROPERTY STRINGS AVX2 AVX512 NEON WASM_SIMD128 WASM_RELAXED_SIMD)
if(native_default_isa STREQUAL "WASM_SIMD128")
  set(native_default_profiles "WASM_SIMD128")
elseif(native_default_isa STREQUAL "NEON")
  set(native_default_profiles "NEON;NEON_FP16;NEON_BF16")
else()
  set(native_default_profiles "AVX2;AVX512;AVX512_BF16;AVX512_FP16")
endif()
set(NATIVE_PROFILES "${native_default_profiles}" CACHE STRING "Profiles exercised by the regression tests; the hub exposes all supported host profiles")
list(REMOVE_DUPLICATES NATIVE_PROFILES)
foreach(profile IN LISTS NATIVE_PROFILES)
  if(NOT profile MATCHES "^(AVX2|AVX512|AVX512_BF16|AVX512_FP16|NEON|NEON_FP16|NEON_BF16|WASM_SIMD128|WASM_RELAXED_SIMD)$")
    message(FATAL_ERROR "NATIVE_PROFILES entries must be AVX2, AVX512, AVX512_BF16, AVX512_FP16, NEON, NEON_FP16, NEON_BF16, WASM_SIMD128, or WASM_RELAXED_SIMD.")
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

if(NOT TARGET hint::hint)
  include(FetchContent)
  FetchContent_Declare(hint
    GIT_REPOSITORY https://github.com/ekmett/hint.git
    GIT_TAG main
    FIND_PACKAGE_ARGS 0.1 CONFIG)
  FetchContent_MakeAvailable(hint)
endif()

# Configuration, attributes and source-level target generation are textual.
# Downstream libraries can consume them without enabling the host module build.
add_library(native_headers INTERFACE)
add_library(native::headers ALIAS native_headers)
target_link_libraries(native_headers INTERFACE hint::hint)
set_target_properties(native_headers PROPERTIES EXPORT_NAME headers)
# Named swizzle properties use Clang's Microsoft property extension. Carry this
# requirement with the headers as well as modules, including relocated consumers.
target_compile_options(native_headers INTERFACE
  "$<$<AND:$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>,$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:-fms-extensions>")
target_sources(native_headers INTERFACE
  FILE_SET headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src"
  FILES src/native/config.h src/native/attributes.h src/native/isa.h src/native/targets.h
    src/native/mask_traits.h src/native/wasm/features.h
    src/native/detail/constexpr_float.h)
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
  if(NATIVE_ENABLE_COVERAGE)
    include(etc/cmake/Coverage.cmake)
  endif()
  add_subdirectory(src)
endif()
if(NATIVE_BUILD_DOCS)
  add_subdirectory(doc)
endif()
if(NATIVE_BUILD_HOST AND NATIVE_BUILD_TESTS)
  include(etc/cmake/TestShards.cmake)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^wasm(32|64)$")
    native_test_directory(wasm_simd)
    native_test_directory(wasm_math)
    native_test_directory(wasm_relaxed)
    native_test_directory(wasm_boundaries)
  else()
    native_test_directory(smoke)
  endif()
  if(NATIVE_TEST_EXTENDED AND NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^wasm(32|64)$")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/core_regression/CMakeLists.txt")
      native_test_directory(core_regression)
    else()
      message(FATAL_ERROR "NATIVE_BUILD_TESTS requires tests/core_regression sources.")
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/isa_profiles/CMakeLists.txt")
      native_test_directory(isa_profiles)
    endif()
    native_test_directory(bf16_profile)
    native_test_directory(neon_fp16)
    native_test_directory(neon_bf16)
    native_test_directory(avx512_fp16)
    native_test_directory(static_string)
    native_test_directory(apple_headers)
    native_test_directory(windows_headers)
    native_test_directory(rounding)
    native_test_directory(scaleb)
    native_test_directory(compaction)
    native_test_directory(source_targets)
    native_test_directory(native_cutover)
    native_test_directory(native_mask_traits)
    native_test_directory(instruction_storage)
    native_test_directory(constexpr_float)
    native_test_directory(half_constexpr)
    native_test_directory(floating_instructions_constexpr)
    native_test_directory(float_constexpr)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
      native_test_directory(polyfill_x86_scalar)
      native_test_directory(polyfill_x86_integer)
      native_test_directory(polyfill_x86_float)
      native_test_directory(x86_bmi1)
      native_test_directory(x86_bmi2)
      native_test_directory(x86_popcnt)
      native_test_directory(x86_lzcnt)
      native_test_directory(x86_crc32c)
      native_test_directory(x86_aes)
      native_test_directory(x86_gfni)
      native_test_directory(x86_vpopcntdq)
      native_test_directory(x86_avx512cd)
      native_test_directory(x86_bitalg)
      native_test_directory(x86_vbmi)
      native_test_directory(x86_vbmi2)
      native_test_directory(x86_sha)
      native_test_directory(x86_vaes)
      native_test_directory(x86_ifma)
      native_test_directory(x86_adx)
      native_test_directory(x86_wait)
      native_test_directory(x86_extended_crypto)
      native_test_directory(x86_avxneconvert)
      native_test_directory(x86_memory)
      native_test_directory(x86_pclmul)
      native_test_directory(x86_vnni)
      native_test_directory(x86_f16c)
      native_test_directory(x86_scalar_defaults)
      native_test_directory(x86_constexpr)
      native_test_directory(x86_integer_constexpr)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
      native_test_directory(arm_neon)
      native_test_directory(arm_dotprod)
      native_test_directory(arm_rdm)
      native_test_directory(arm_bf16)
      native_test_directory(arm_fp16fml)
      native_test_directory(arm_fcma)
      native_test_directory(arm_i8mm)
      native_test_directory(arm_constexpr)
      native_test_directory(arm_scalar_defaults)
      native_test_directory(arm_crc)
      native_test_directory(arm_jscvt)
      native_test_directory(arm_crypto)
      native_test_directory(arm_sm_crypto)
    endif()
    native_test_directory(wasm_features)
    native_test_directory(abi_lookup)
    native_test_directory(polyfill)
    native_test_directory(wide_construction)
    native_test_directory(exp_policy_refinement)
    native_test_directory(kernel_policy_metadata)
    native_test_directory(kernel_half_requirements)
    native_test_directory(kernel_integer_refinement)
    native_test_directory(kernel_policy_refinement)
  endif()
  get_property(native_test_groups GLOBAL PROPERTY NATIVE_SELECTED_TEST_GROUPS)
  list(JOIN native_test_groups "\n" native_test_groups)
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/test-groups.txt" "${native_test_groups}\n")
endif()

configure_file(etc/cmake/nativeProfile.cmake "${CMAKE_CURRENT_BINARY_DIR}/nativeProfile.cmake" COPYONLY)
configure_package_config_file(etc/cmake/nativeConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/install/nativeConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/native"
  PATH_VARS CMAKE_INSTALL_DATADIR)
# Build-tree consumers also need to locate a source-built Hint package. Keep
# that path out of the installed, relocatable Native configuration.
if(hint_BINARY_DIR)
  set(native_hint_build_hint "\"${hint_BINARY_DIR}\"")
endif()
configure_package_config_file(etc/cmake/nativeConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/nativeConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/native"
  PATH_VARS CMAKE_INSTALL_DATADIR)
unset(native_hint_build_hint)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/nativeConfigVersion.cmake"
  VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/nativeProfile.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/install/nativeConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/nativeConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/native")

install(FILES LICENSE.md THIRD-PARTY-NOTICES.md
  DESTINATION "${CMAKE_INSTALL_DATADIR}/native")

install(FILES src/native/wasm/features.mjs
  DESTINATION "${CMAKE_INSTALL_DATADIR}/native/wasm")
