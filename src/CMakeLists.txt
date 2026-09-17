# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Every object enters one archive. Module metadata stays with its owning target.
# Consumer BMI rebuilds retain producer namespace/profile definitions while
# CMake may synthesize them using the consumer's compiler options.
function(simd_module_target target export_name)
  add_library(${target} OBJECT)
  add_library(simd::${export_name} ALIAS ${target})
  simd_host_settings(${target})
  set_target_properties(${target} PROPERTIES EXPORT_NAME ${export_name}
    POSITION_INDEPENDENT_CODE ON CXX_SCAN_FOR_MODULES ON)
  target_link_libraries(${target} PUBLIC simd_headers)
endfunction()

simd_module_target(simd_common common)
target_sources(simd_common PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/cxx/modules"
  FILES cxx/modules/simd.scalar.ccm cxx/modules/simd.wide.ccm cxx/modules/simd.static_string.ccm
    cxx/modules/simd.memory.ccm cxx/modules/simd.types.ccm cxx/modules/simd.numerics.ccm)

# Baseline x86 utilities are independent of every SIMD profile.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  target_sources(simd_common PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/cxx/modules"
    FILES cxx/modules/simd.cpuid.ccm cxx/modules/simd.wait.ccm)
endif()

set(simd_module_targets simd_common)

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
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/cxx/modules" FILES cxx/modules/simd.${name}.ccm)
  target_link_libraries(simd_${name} PUBLIC simd_common)
  if(SIMD_ENABLE_PCH)
    target_precompile_headers(simd_${name} PRIVATE <simd/vec.h> <simd/simd/math/exp.h>)
  endif()
  list(APPEND simd_module_targets simd_${name})
endforeach()

add_library(simd STATIC)
add_library(simd::simd ALIAS simd)
simd_host_settings(simd)
set_target_properties(simd PROPERTIES POSITION_INDEPENDENT_CODE ON)
foreach(target IN LISTS simd_module_targets)
  target_sources(simd PRIVATE $<TARGET_OBJECTS:${target}>)
endforeach()
# Consumers link the provider(s) for modules they import. The final executable
# links this archive. Keeping those layers explicit avoids CMake's transitive
# synthetic-BMI ambiguity when two producers use different ISA options.
target_link_libraries(simd PUBLIC simd_headers)
install(TARGETS simd EXPORT simdHostTargets ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
# Object files are already in simd's archive. Install the module source/metadata
# targets as interfaces, without shipping compiler-specific PCMs or loose objects.
install(TARGETS ${simd_module_targets} EXPORT simdHostTargets
  FILE_SET cxx_modules DESTINATION "${CMAKE_INSTALL_LIBDIR}/simd/modules")
install(EXPORT simdHostTargets NAMESPACE simd:: CXX_MODULES_DIRECTORY cxx-modules
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/simd")
export(EXPORT simdHostTargets NAMESPACE simd:: CXX_MODULES_DIRECTORY cxx-modules
  FILE "${PROJECT_BINARY_DIR}/simdHostTargets.cmake")
