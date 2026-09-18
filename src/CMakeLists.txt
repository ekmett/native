# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Each ABI owns its archive and module provider. Common modules use the
# platform baseline; stronger ISA options belong to profile sources only.
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
  foreach(feature IN ITEMS AVX2 AVX512 AVX512_BF16)
    if(feature STREQUAL "AVX2")
      set(condition "defined(__AVX2__)")
    elseif(feature STREQUAL "AVX512_BF16")
      set(condition "defined(__AVX512BF16__)")
    else()
      set(condition "defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)")
    endif()
    set(probe "SIMD_MINIMAL_${feature}_${minimal_key}")
    check_cxx_source_compiles("#if !(${condition})
#error feature absent
#endif
int probe() { return 0; }" ${probe})
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
    simd.memory.ccm simd.types.ccm simd.numerics.ccm)

# Baseline x86 utilities are independent of every SIMD profile.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  target_sources(simd_minimal PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES simd.cpuid.ccm simd.wait.ccm)
endif()

set(simd_module_targets simd_minimal)

foreach(profile IN LISTS SIMD_PROFILES)
  string(TOLOWER "${profile}" name)
  if(profile STREQUAL "AVX2")
    set(bits 256)
  elseif(profile STREQUAL "NEON")
    set(bits 128)
  else()
    set(bits 512)
  endif()
  simd_module_target(simd_${name} ${name})
  simd_target_profile(simd_${name} "${profile}")
  target_compile_definitions(simd_${name} PRIVATE
    SIMD_PROFILE=${bits})
  target_sources(simd_${name} PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES simd.${name}.ccm)
  target_link_libraries(simd_${name} PUBLIC simd_minimal)
  list(APPEND simd_module_targets simd_${name})
endforeach()

# The optional compatibility omnibus imports every selected profile; Clang
# requires its own source to enable their strongest ISA. Common stays baseline.
set(simd_omnibus_imports "export import simd.scalar;\nexport import simd.wide;\nexport import simd.types;\nexport import simd.memory;\nexport import simd.static_string;\nexport import simd.numerics;\n")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  string(APPEND simd_omnibus_imports "export import simd.cpuid;\nexport import simd.wait;\n")
endif()
foreach(profile IN LISTS SIMD_PROFILES)
  string(TOLOWER "${profile}" name)
  string(APPEND simd_omnibus_imports "export import simd.${name};\n")
endforeach()
configure_file(simd.ccm.in "${CMAKE_CURRENT_BINARY_DIR}/modules/simd.ccm" @ONLY)
add_library(simd STATIC)
add_library(simd::simd ALIAS simd)
simd_host_settings(simd)
set_target_properties(simd PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_SCAN_FOR_MODULES ON)
target_sources(simd PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
  BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/modules"
  FILES "${CMAKE_CURRENT_BINARY_DIR}/modules/simd.ccm")
if("AVX512_BF16" IN_LIST SIMD_PROFILES)
  simd_target_profile(simd AVX512_BF16)
elseif("AVX512" IN_LIST SIMD_PROFILES)
  simd_target_profile(simd AVX512)
elseif("AVX2" IN_LIST SIMD_PROFILES)
  simd_target_profile(simd AVX2)
elseif("NEON" IN_LIST SIMD_PROFILES)
  simd_target_profile(simd NEON)
endif()
# The archive carries omnibus module metadata, but no profile compile flags.
# Granular provider targets remain available for narrower imports.
target_link_libraries(simd PUBLIC simd_headers ${simd_module_targets})
install(TARGETS simd EXPORT simdHostTargets ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET cxx_modules DESTINATION "${CMAKE_INSTALL_LIBDIR}/simd/modules")
# Install each ABI archive with its module sources and metadata. BMIs remain
# local to the consuming compiler/configuration.
install(TARGETS ${simd_module_targets} EXPORT simdHostTargets
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
  FILES_MATCHING PATTERN "*.h" PATTERN "attributes.h" EXCLUDE PATTERN "config.h" EXCLUDE)
