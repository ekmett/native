# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Keep producer flags and installed BMI regeneration in one mapping.
function(simd_profile_options profile output)
  set(flags)
  if(NOT profile STREQUAL "NEON")
    list(APPEND flags -mavx2 -mfma -mbmi2)
  endif()
  if(profile MATCHES "^AVX512")
    list(APPEND flags -mavx512f -mavx512dq -mavx512bw -mavx512vl)
  endif()
  if(profile STREQUAL "AVX512_BF16")
    list(APPEND flags -mavx512bf16)
  endif()
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    list(TRANSFORM flags PREPEND "/clang:")
  endif()
  set(${output} "${flags}" PARENT_SCOPE)
endfunction()

# Select the ISA of one producer/consumer target. Never propagate it to callers.
# Linking simd::simd alone does not select an ISA or a runtime dispatch policy.
function(simd_target_profile target profile)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "simd_target_profile requires an existing target: ${target}")
  endif()
  string(TOUPPER "${profile}" profile)
  if(NOT profile MATCHES "^(AVX2|AVX512|AVX512_BF16|NEON)$")
    message(FATAL_ERROR "Unknown simd target profile: ${profile}")
  endif()
  if(profile STREQUAL "NEON")
    if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
      message(FATAL_ERROR "The NEON profile requires an arm64 target toolchain.")
    endif()
  elseif(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    message(FATAL_ERROR "The ${profile} profile requires an x86-64 target toolchain.")
  endif()
  get_target_property(previous "${target}" SIMD_TARGET_PROFILE)
  if(previous AND NOT previous STREQUAL profile)
    message(FATAL_ERROR "Target ${target} already selects ${previous}, not ${profile}.")
  endif()
  if(previous)
    return()
  endif()
  set_property(TARGET "${target}" PROPERTY SIMD_TARGET_PROFILE "${profile}")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "The Windows simd profiles require clang-cl.")
  endif()
  simd_profile_options("${profile}" flags)
  # These are implementation options, not options for regenerating imported
  # dependencies. COMPILE_FLAGS also covers CMake's generated PCH source;
  # ordinary source properties alone would leave that PCH at the wrong ISA.
  # Clang's BMI and PCH compatibility validation remains enabled.
  string(JOIN " " implementation_flags ${flags})
  set_property(TARGET "${target}" APPEND_STRING PROPERTY COMPILE_FLAGS " ${implementation_flags}")
  set_property(TARGET "${target}" PROPERTY SIMD_SOURCE_ISA_OPTIONS "${flags}")
  cmake_language(EVAL CODE "cmake_language(DEFER CALL simd_source_profile [[${target}]])")
endfunction()

function(simd_source_profile target)
  get_target_property(module_sets "${target}" CXX_MODULE_SETS)
  get_target_property(options "${target}" SIMD_SOURCE_ISA_OPTIONS)
  foreach(module_set IN LISTS module_sets)
    get_target_property(module_sources "${target}" CXX_MODULE_SET_${module_set})
    # Synthetic providers do not inherit COMPILE_FLAGS. A module source owns
    # its ISA irrespective of the target importing it.
    set_property(SOURCE ${module_sources} TARGET_DIRECTORY "${target}" APPEND PROPERTY
      COMPILE_OPTIONS ${options})
  endforeach()
endfunction()
