# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Keep producer flags and installed BMI regeneration in one mapping.
function(simd_profile_options profile output)
  set(flags)
  if(NOT profile MATCHES "^NEON")
    list(APPEND flags -mavx2 -mfma -mbmi2)
  endif()
  if(profile MATCHES "^AVX512")
    list(APPEND flags -mavx512f -mavx512dq -mavx512bw -mavx512vl)
  endif()
  if(profile STREQUAL "AVX512_BF16")
    list(APPEND flags -mavx512bf16)
  endif()
  if(profile STREQUAL "NEON_BF16")
    # Add BF16 only; retain the configured CPU/architecture.
    list(APPEND flags -Xclang=-target-feature -Xclang=+bf16)
  endif()
  if(profile STREQUAL "NEON_FP16")
    # Add only half arithmetic; do not reset the configured CPU/architecture.
    list(APPEND flags -Xclang=-target-feature -Xclang=+fullfp16)
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
  if(NOT profile MATCHES "^(AVX2|AVX512|AVX512_BF16|NEON|NEON_FP16|NEON_BF16)$")
    message(FATAL_ERROR "Unknown simd target profile: ${profile}")
  endif()
  if(profile MATCHES "^NEON")
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
  # Keep implementation options out of target COMPILE_OPTIONS: CMake uses that
  # property to regenerate imported dependencies, which would clone common BMIs
  # at each caller's ISA. Late source options below also override an explicitly
  # disabled feature in the configured minimum. Clang's BMI/PCH checks stay on.
  string(JOIN " " implementation_flags ${flags})
  set_property(TARGET "${target}" APPEND_STRING PROPERTY COMPILE_FLAGS " ${implementation_flags}")
  set_property(TARGET "${target}" PROPERTY SIMD_SOURCE_ISA_OPTIONS "${flags}")
  cmake_language(EVAL CODE "cmake_language(DEFER CALL simd_source_profile [[${target}]])")
endfunction()

# Compile a consumer of import simd with exactly the installed producer's union.
# Unlike choosing one granular profile, this may enable independent extensions.
function(simd_target_omnibus target)
  if(NOT TARGET "${target}" OR NOT TARGET simd::simd)
    message(FATAL_ERROR "simd_target_omnibus requires its target and simd::simd.")
  endif()
  get_target_property(previous "${target}" SIMD_TARGET_PROFILE)
  if(previous AND NOT previous STREQUAL "OMNIBUS")
    message(FATAL_ERROR "Target ${target} already selects ${previous}, not OMNIBUS.")
  endif()
  if(previous)
    return()
  endif()
  get_target_property(profiles simd::simd SIMD_OMNIBUS_PROFILES)
  if(profiles STREQUAL "profiles-NOTFOUND")
    message(FATAL_ERROR "simd::simd lacks configured omnibus profile metadata.")
  endif()
  set(flags)
  foreach(profile IN LISTS profiles)
    simd_profile_options("${profile}" profile_flags)
    list(APPEND flags ${profile_flags})
  endforeach()
  set_property(TARGET "${target}" PROPERTY SIMD_TARGET_PROFILE OMNIBUS)
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
  get_target_property(sources "${target}" SOURCES)
  get_target_property(source_dir "${target}" SOURCE_DIR)
  foreach(source IN LISTS sources)
    # Module sources above retain their own literal provider options, including
    # when CMake creates a synthetic installed BMI target. Object inputs and
    # non-C++ source entries do not need ordinary translation-unit options.
    if(NOT source MATCHES "^\\$<" AND source MATCHES "[.](cc|cpp|cxx|C)$")
      cmake_path(ABSOLUTE_PATH source BASE_DIRECTORY "${source_dir}" NORMALIZE)
      simd_context_source_profile("${target}" "${source}")
    endif()
  endforeach()
  get_target_property(pch "${target}" PRECOMPILE_HEADERS)
  if(pch)
    # CMake has no target PCH compile-options property. These are the generated
    # C++ PCH source names for the qualified Ninja generators and Clang frontends;
    # keep creation and use at the same ISA even with negative minimum features.
    get_target_property(binary_dir "${target}" BINARY_DIR)
    if(CMAKE_LINK_PCH)
      set(pch_name cmake_pch.cxx)
    else()
      set(pch_name cmake_pch.hxx.cxx)
    endif()
    set(intermediate "${binary_dir}/CMakeFiles/${target}.dir")
    simd_context_source_profile("${target}" "${intermediate}/${pch_name}")
  endif()
endfunction()

# Source properties have directory scope, so a literal profile would contaminate
# a shared kernel compiled by two targets. Evaluate the owning target's options
# instead, and attach that expression only once to each source in this directory.
function(simd_context_source_profile target source)
  get_property(configured SOURCE "${source}" TARGET_DIRECTORY "${target}"
    PROPERTY SIMD_CONTEXT_PROFILE_OPTIONS_SET)
  if(NOT configured)
    get_property(existing SOURCE "${source}" TARGET_DIRECTORY "${target}" PROPERTY COMPILE_OPTIONS)
    # Source-specific overrides remain last (for example a deliberate no-VL
    # textual regression), after both the minimum and this target's profile.
    set_property(SOURCE "${source}" TARGET_DIRECTORY "${target}" PROPERTY
      COMPILE_OPTIONS "$<TARGET_PROPERTY:SIMD_SOURCE_ISA_OPTIONS>" ${existing})
    set_property(SOURCE "${source}" TARGET_DIRECTORY "${target}" PROPERTY
      SIMD_CONTEXT_PROFILE_OPTIONS_SET TRUE)
  endif()
endfunction()
