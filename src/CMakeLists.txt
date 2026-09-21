# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Modules compile at the project minimum. Native operations carry their own
# Clang target attributes inside the shared hub.
function(native_module_target target export_name)
  add_library(${target} STATIC)
  add_library(native::${export_name} ALIAS ${target})
  native_host_settings(${target})
  set_target_properties(${target} PROPERTIES EXPORT_NAME ${export_name}
    POSITION_INDEPENDENT_CODE ON CXX_SCAN_FOR_MODULES ON)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^wasm(32|64)$")
    # WASI static modules and their consumers must agree on mutable-globals.
    # PIC changes that target feature in Clang; these archives do not need it.
    set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE OFF)
  endif()
  target_link_libraries(${target} PUBLIC native_headers)
  # Installed consumers rebuild BMIs with these private implementation inputs.
  # CMake records private include directories in imported module metadata.
  target_include_directories(${target} PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}/native/include>")
endfunction()

native_module_target(native_minimal minimal)
# Importing native and observing CPU features must remain safe at the toolchain's
# baseline. Applications may explicitly select a stronger common ABI.
set(NATIVE_MINIMAL_COMPILE_OPTIONS "" CACHE STRING
  "Compiler options defining the minimal project ABI (native compiler spelling)")
target_compile_options(native_minimal PUBLIC ${NATIVE_MINIMAL_COMPILE_OPTIONS})
# Export the selected minimum for admission/codegen checks in downstream tests.
# Include global toolchain flags in the probe key, so changing the minimum does
# not reuse a cached result from another configuration.
block()
  string(JOIN " " minimal_flags ${NATIVE_MINIMAL_COMPILE_OPTIONS})
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
    set(probe "NATIVE_MINIMAL_${feature}_${minimal_key}")
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
    target_compile_definitions(native_minimal PUBLIC NATIVE_MINIMAL_HAS_${feature}=$<BOOL:${${probe}}>)
  endforeach()
endblock()

# Preserve the existing common target without creating a second provider.
add_library(native_common ALIAS native_minimal)
add_library(native::common ALIAS native_minimal)
# CMake exports these producer definitions for installed BMI regeneration.
# Header-only packages do not configure or depend on native-half capabilities.
target_compile_definitions(native_minimal PRIVATE
  NATIVE_HAS_NATIVE_FP16=$<BOOL:${NATIVE_HAS_NATIVE_FP16}>
  NATIVE_HAS_NATIVE_BF16=$<BOOL:${NATIVE_HAS_NATIVE_BF16}>)
target_sources(native_minimal PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES native.isa.ccm native.scalar.ccm native.wide.ccm native.static_string.ccm
    native.memory.ccm native.types.ccm native.numerics.ccm native.features.ccm
    native.wasm.features.ccm)

# Architecture utilities are independent of every SIMD profile.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  target_sources(native_minimal PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES native.x86.features.ccm native.x86.wait.ccm native.x86.bmi1.ccm native.x86.bmi2.ccm
      native.x86.popcnt.ccm native.x86.lzcnt.ccm native.x86.crc32c.ccm)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  target_sources(native_minimal PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES native.arm.features.ccm native.arm.crc.ccm)
endif()

native_module_target(native native)
target_link_libraries(native PUBLIC native_minimal)
target_sources(native PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES native.simd.ccm native.math.ccm native.ccm)

# Vector instruction interfaces use the same simd types as the hub. Metadata
# and scalar utilities remain in the minimal target, without a vector dependency.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  target_sources(native PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES native.x86.gfni.ccm native.x86.vpopcntdq.ccm native.x86.pclmul.ccm
      native.x86.vpclmul.ccm native.x86.vnni.ccm native.x86.f16c.ccm
      native.x86.aes.ccm native.x86.avx512cd.ccm native.x86.ccm)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  target_sources(native PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES native.arm.dotprod.ccm native.arm.rdm.ccm native.arm.fp16fml.ccm
      native.arm.fcma.ccm native.arm.i8mm.ccm native.arm.bf16.ccm native.arm.aes.ccm
      native.arm.pmull.ccm native.arm.sha.ccm native.arm.ccm)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^wasm(32|64)$")
  target_sources(native PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES native.wasm.ccm)
endif()

# Older build files may link these names. They all refer to the same hub and
# add neither another archive/BMI nor stronger compile options.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  set(native_hub_profiles AVX2 AVX512 AVX512_BF16 AVX512_FP16)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(native_hub_profiles NEON NEON_FP16 NEON_BF16)
else()
  set(native_hub_profiles)
endif()
foreach(profile IN LISTS native_hub_profiles)
  string(TOLOWER "${profile}" name)
  add_library(native_${name} ALIAS native)
  add_library(native::${name} ALIAS native)
endforeach()
set_property(TARGET native PROPERTY NATIVE_HUB_PROFILES "${native_hub_profiles}")
# Retain the old metadata key for build-file compatibility; it no longer means
# that importing the hub requires the union of these instruction sets.
set_property(TARGET native PROPERTY NATIVE_OMNIBUS_PROFILES "${native_hub_profiles}")
set_property(TARGET native APPEND PROPERTY EXPORT_PROPERTIES
  NATIVE_HUB_PROFILES NATIVE_OMNIBUS_PROFILES)
install(TARGETS native native_minimal EXPORT nativeHostTargets
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET cxx_modules DESTINATION "${CMAKE_INSTALL_LIBDIR}/native/modules")
install(EXPORT nativeHostTargets NAMESPACE native:: CXX_MODULES_DIRECTORY cxx-modules
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/native")
export(EXPORT nativeHostTargets NAMESPACE native:: CXX_MODULES_DIRECTORY cxx-modules
  FILE "${PROJECT_BINARY_DIR}/nativeHostTargets.cmake")

# Preserve implementation inputs for consumer BMI regeneration without making
# them public headers or exposing their directory through native::headers.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/native/"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/native/include/native"
  FILES_MATCHING PATTERN "*.h" PATTERN "attributes.h" EXCLUDE PATTERN "config.h" EXCLUDE
    PATTERN "isa.h" EXCLUDE PATTERN "targets.h" EXCLUDE PATTERN "mask_traits.h" EXCLUDE)
