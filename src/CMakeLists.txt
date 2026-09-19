# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Modules compile at the project minimum. Native operations carry their own
# Clang target attributes inside the shared hub.
function(simd_module_target target export_name)
  add_library(${target} STATIC)
  add_library(simd::${export_name} ALIAS ${target})
  simd_host_settings(${target})
  set_target_properties(${target} PROPERTIES EXPORT_NAME ${export_name}
    POSITION_INDEPENDENT_CODE ON CXX_SCAN_FOR_MODULES ON)
  target_link_libraries(${target} PUBLIC simd_headers)
  # Installed consumers rebuild BMIs with these private implementation inputs.
  # CMake records private include directories in imported module metadata.
  target_include_directories(${target} PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}/simd/include>")
endfunction()

simd_module_target(simd_minimal minimal)
# Project setup selects the common ABI. Defaults are modern platform baselines;
# the native compiler option list can be set before adding/configuring simd.
set(minimal_default_options)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  set(minimal_default_options -mavx2 -mfma -mbmi2)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    list(TRANSFORM minimal_default_options PREPEND "/clang:")
  endif()
endif()
set(SIMD_MINIMAL_COMPILE_OPTIONS "${minimal_default_options}" CACHE STRING
  "Compiler options defining the minimal project ABI (native compiler spelling)")
target_compile_options(simd_minimal PUBLIC ${SIMD_MINIMAL_COMPILE_OPTIONS})
# Export the selected minimum for admission/codegen checks in downstream tests.
# Include global toolchain flags in the probe key, so changing the minimum does
# not reuse a cached result from another configuration.
block()
  string(JOIN " " minimal_flags ${SIMD_MINIMAL_COMPILE_OPTIONS})
  string(APPEND CMAKE_REQUIRED_FLAGS " ${minimal_flags}")
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
  if(CMAKE_BUILD_TYPE)
    set(CMAKE_TRY_COMPILE_CONFIGURATION "${CMAKE_BUILD_TYPE}")
  endif()
  string(TOUPPER "${CMAKE_TRY_COMPILE_CONFIGURATION}" minimal_config)
  string(SHA256 minimal_key
    "${CMAKE_CXX_FLAGS};${CMAKE_BUILD_TYPE};${CMAKE_TRY_COMPILE_CONFIGURATION};${CMAKE_CXX_FLAGS_${minimal_config}};${CMAKE_CXX_FLAGS_DEBUG};${minimal_flags}")
  foreach(feature IN ITEMS AVX2 AVX512 AVX512_BF16 AVX512_FP16 NEON_FP16 NEON_BF16)
    if(feature STREQUAL "AVX2")
      set(condition "defined(__AVX2__)")
    elseif(feature STREQUAL "AVX512_BF16")
      set(condition "defined(__AVX512BF16__)")
    elseif(feature STREQUAL "NEON_BF16")
      set(condition "defined(__ARM_FEATURE_BF16_VECTOR_ARITHMETIC)")
    elseif(feature STREQUAL "AVX512_FP16")
      set(condition "defined(__AVX512FP16__)")
    elseif(feature STREQUAL "NEON_FP16")
      set(condition "defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) && defined(__ARM_FEATURE_FP16_SCALAR_ARITHMETIC)")
    else()
      set(condition "defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)")
    endif()
    set(probe "SIMD_MINIMAL_${feature}_${minimal_key}")
    if(feature STREQUAL "NEON_FP16")
      # Some AArch64 targets retain the FP16 macros via FP16FML after an explicit
      # -fullfp16. Test usable arithmetic, not just the advertised macros.
      string(APPEND probe "_intrinsics_v1")
      check_cxx_source_compiles([=[
        #include <arm_neon.h>
        float16x8_t probe(float16x8_t a, float16x8_t b, float16x8_t c) {
          return vfmaq_f16(vaddq_f16(a,b),vmulq_f16(b,c),c);
        }
      ]=] ${probe})
    else()
      check_cxx_source_compiles("#if !(${condition})
#error feature absent
#endif
int probe() { return 0; }" ${probe})
    endif()
    target_compile_definitions(simd_minimal PUBLIC SIMD_MINIMAL_HAS_${feature}=$<BOOL:${${probe}}>)
  endforeach()
endblock()

# Preserve the existing common target without creating a second provider.
add_library(simd_common ALIAS simd_minimal)
add_library(simd::common ALIAS simd_minimal)
# CMake exports these producer definitions for installed BMI regeneration.
# Header-only packages do not configure or depend on native-half capabilities.
target_compile_definitions(simd_minimal PRIVATE
  SIMD_HAS_NATIVE_FP16=$<BOOL:${SIMD_HAS_NATIVE_FP16}>
  SIMD_HAS_NATIVE_BF16=$<BOOL:${SIMD_HAS_NATIVE_BF16}>)
target_sources(simd_minimal PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES simd.scalar.ccm simd.wide.ccm simd.static_string.ccm
    simd.memory.ccm simd.types.ccm simd.numerics.ccm simd.cpu.ccm)

# Baseline x86 utilities are independent of every SIMD profile.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  target_sources(simd_minimal PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES simd.cpu.x86.ccm simd.wait.ccm)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  target_sources(simd_minimal PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES simd.cpu.arm.ccm)
endif()

simd_module_target(simd simd)
target_link_libraries(simd PUBLIC simd_minimal)
target_sources(simd PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES simd.ccm)

# Older build files may link these names. They all refer to the same hub and
# add neither another archive/BMI nor stronger compile options.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  set(simd_hub_profiles AVX2 AVX512 AVX512_BF16 AVX512_FP16)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(simd_hub_profiles NEON NEON_FP16 NEON_BF16)
else()
  set(simd_hub_profiles)
endif()
foreach(profile IN LISTS simd_hub_profiles)
  string(TOLOWER "${profile}" name)
  add_library(simd_${name} ALIAS simd)
  add_library(simd::${name} ALIAS simd)
endforeach()
set_property(TARGET simd PROPERTY SIMD_HUB_PROFILES "${simd_hub_profiles}")
# Retain the old metadata key for build-file compatibility; it no longer means
# that importing the hub requires the union of these instruction sets.
set_property(TARGET simd PROPERTY SIMD_OMNIBUS_PROFILES "${simd_hub_profiles}")
set_property(TARGET simd APPEND PROPERTY EXPORT_PROPERTIES
  SIMD_HUB_PROFILES SIMD_OMNIBUS_PROFILES)
install(TARGETS simd simd_minimal EXPORT simdHostTargets
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET cxx_modules DESTINATION "${CMAKE_INSTALL_LIBDIR}/simd/modules")
install(EXPORT simdHostTargets NAMESPACE simd:: CXX_MODULES_DIRECTORY cxx-modules
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/simd")
export(EXPORT simdHostTargets NAMESPACE simd:: CXX_MODULES_DIRECTORY cxx-modules
  FILE "${PROJECT_BINARY_DIR}/simdHostTargets.cmake")

# Preserve implementation inputs for consumer BMI regeneration without making
# them public headers or exposing their directory through simd::headers.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/simd/"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/simd/include/simd"
  FILES_MATCHING PATTERN "*.h" PATTERN "attributes.h" EXCLUDE PATTERN "config.h" EXCLUDE
    PATTERN "isa.h" EXCLUDE PATTERN "targets.h" EXCLUDE)
