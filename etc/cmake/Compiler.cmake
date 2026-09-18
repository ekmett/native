# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Require the feature used by the new consumer interface, not just a flag name.
include(CheckCXXSourceCompiles)
block()
  set(CMAKE_CXX_STANDARD 26)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
  check_cxx_source_compiles([=[
    #include <array>
    template<unsigned N> constexpr int sum(std::array<int, N> a) {
      auto [...x] = a;
      return (0 + ... + x);
    }
    static_assert(sum<0>({}) == 0);
    static_assert(sum<2>({1, 2}) == 3);
  ]=] SIMD_HAS_CXX26_PACK_BINDINGS)
  if(NOT SIMD_HAS_CXX26_PACK_BINDINGS)
    message(FATAL_ERROR "simd requires C++26 structured-binding packs (qualified: upstream Clang 23).")
  endif()
endblock()
block()
  set(CMAKE_CXX_STANDARD 26)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
  if(NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    string(APPEND CMAKE_REQUIRED_FLAGS " -fms-extensions")
  endif()
  check_cxx_source_compiles([=[
    struct members {
      template<class Self> constexpr int get_x(this Self const &self) {
        return self.value;
      }
      template<class Self> constexpr void set_x(this Self &self, int x) {
        self.value = x;
      }
      __declspec(property(get=get_x, put=set_x)) int x;
    };
    struct vector : members { int value; };
    static_assert(sizeof(vector) == sizeof(int));
    int test() { vector v{{}, 1}; v.x = 2; return v.x; }
  ]=] SIMD_HAS_SWIZZLE_PROPERTIES)
  if(NOT SIMD_HAS_SWIZZLE_PROPERTIES)
    message(FATAL_ERROR "simd requires Clang named properties and explicit object parameters (-fms-extensions).")
  endif()
endblock()
# Native half syntax and by-value ABI support are independent target features.
# Probe representation transport only; float conversion uses our integer graph.
block()
  set(CMAKE_CXX_STANDARD 26)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
  foreach(format IN ITEMS FP16 BF16)
    if(format STREQUAL "FP16")
      set(native_type _Float16)
    else()
      set(native_type __bf16)
    endif()
    check_cxx_source_compiles("
      using native_type = ${native_type};
      static_assert(sizeof(native_type) == sizeof(unsigned short));
      static_assert(sizeof(native_type) == 2);
      native_type transport(native_type value) {
        auto bits = __builtin_bit_cast(unsigned short, value);
        return __builtin_bit_cast(native_type, bits);
      }
    " SIMD_HAS_NATIVE_${format})
  endforeach()
endblock()
if(SIMD_BUILD_TESTS)
  if(NOT SIMD_TEST_ISA MATCHES "^(AVX2|AVX512|NEON)$")
    message(FATAL_ERROR "SIMD_TEST_ISA must be AVX2, AVX512, or NEON.")
  endif()
  if(SIMD_TEST_ISA STREQUAL "NEON")
    if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
      message(FATAL_ERROR "SIMD_TEST_ISA=NEON requires an arm64 target toolchain.")
    endif()
  elseif(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    message(FATAL_ERROR "SIMD_TEST_ISA=${SIMD_TEST_ISA} requires an x86-64 target toolchain.")
  endif()
endif()
if(SIMD_BUILD_HOST AND NOT CMAKE_GENERATOR MATCHES "^Ninja")
  message(FATAL_ERROR "The qualified simd module build requires Ninja or Ninja Multi-Config.")
endif()
if(SIMD_ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT simd_ipo_ok OUTPUT simd_ipo_error LANGUAGES CXX)
  if(NOT simd_ipo_ok)
    message(FATAL_ERROR "Requested IPO is unavailable: ${simd_ipo_error}")
  endif()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/simdProfile.cmake")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  block()
    simd_profile_options(AVX512_BF16 bf16_options)
    string(JOIN " " bf16_flags ${bf16_options})
    string(APPEND CMAKE_REQUIRED_FLAGS " ${bf16_flags}")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    check_cxx_source_compiles([=[
      #include <immintrin.h>
      #ifndef __AVX512BF16__
      #error BF16 target support missing
      #endif
      __m512 probe(__m512 c, __m512bh a, __m512bh b) {
        return _mm512_dpbf16_ps(c, a, b);
      }
    ]=] SIMD_HAS_AVX512_BF16_INTRINSICS)
    if(NOT SIMD_HAS_AVX512_BF16_INTRINSICS)
      message(FATAL_ERROR "AVX512_BF16 requires compiler support for native BF16 dot products.")
    endif()
  endblock()
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  block()
    simd_profile_options(NEON_BF16 bf16_options)
    string(JOIN " " bf16_flags ${bf16_options})
    string(APPEND CMAKE_REQUIRED_FLAGS " ${bf16_flags}")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    check_cxx_source_compiles([=[
      #include <arm_neon.h>
      #ifndef __ARM_FEATURE_BF16_VECTOR_ARITHMETIC
      #error BF16 target support missing
      #endif
      float32x4_t probe(float32x4_t c, bfloat16x8_t a, bfloat16x8_t b) {
        return vbfdotq_f32(c,a,b);
      }
    ]=] SIMD_HAS_NEON_BF16_INTRINSICS)
    if(NOT SIMD_HAS_NEON_BF16_INTRINSICS)
      message(FATAL_ERROR "NEON_BF16 requires compiler support for native BF16 dot products.")
    endif()
  endblock()
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  block()
    simd_profile_options(AVX512_FP16 fp16_options)
    string(JOIN " " fp16_flags ${fp16_options})
    string(APPEND CMAKE_REQUIRED_FLAGS " ${fp16_flags}")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    check_cxx_source_compiles([=[
      #include <immintrin.h>
      #ifndef __AVX512FP16__
      #error Native FP16 arithmetic support missing
      #endif
      __m512h probe(__m512h a, __m512h b, __m512h c) {
        return _mm512_fmadd_ph(_mm512_add_ph(a,b),_mm512_mul_ph(b,c),c);
      }
    ]=] SIMD_HAS_AVX512_FP16_INTRINSICS)
    if(NOT SIMD_HAS_AVX512_FP16_INTRINSICS)
      message(FATAL_ERROR "AVX512_FP16 requires compiler support for native FP16 arithmetic.")
    endif()
  endblock()
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  block()
    simd_profile_options(NEON_FP16 fp16_options)
    string(JOIN " " fp16_flags ${fp16_options})
    string(APPEND CMAKE_REQUIRED_FLAGS " ${fp16_flags}")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    check_cxx_source_compiles([=[
      #include <arm_neon.h>
      #if !defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) || !defined(__ARM_FEATURE_FP16_SCALAR_ARITHMETIC)
      #error Native FP16 arithmetic support missing
      #endif
      float16x8_t probe(float16x8_t a, float16x8_t b, float16x8_t c) {
        return vfmaq_f16(vaddq_f16(a, b), vmulq_f16(b, c), c);
      }
    ]=] SIMD_HAS_NEON_FP16_INTRINSICS)
    if(NOT SIMD_HAS_NEON_FP16_INTRINSICS)
      message(FATAL_ERROR "NEON_FP16 requires compiler support for native FP16 vector arithmetic.")
    endif()
  endblock()
endif()

function(simd_host_settings target)
  target_compile_features(${target} PUBLIC cxx_std_26)
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      message(FATAL_ERROR "The Windows host configuration requires clang-cl.")
    endif()
    target_compile_options(${target} PUBLIC /fp:strict)
    set_property(TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS " /W4 /clang:-Wno-unqualified-std-cast-call")
    if(SIMD_ENABLE_ASAN)
      target_compile_options(${target} PUBLIC /fsanitize=address)
      target_link_options(${target} PUBLIC /INCREMENTAL:NO)
    endif()
  else()
    target_compile_options(${target} PUBLIC -fno-fast-math -ffp-contract=off -frounding-math)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wno-unqualified-std-cast-call)
    if(SIMD_ENABLE_ASAN)
      target_compile_options(${target} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
      target_link_options(${target} PUBLIC -fsanitize=address)
    endif()
  endif()
  if(NOT SIMD_ENABLE_EXCEPTIONS)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      # /clang:-fno-exceptions does not override clang-cl /EHsc. Both language
      # unwind and MSVC STL modes must agree in producer and BMI consumers.
      target_compile_options(${target} PUBLIC /EHs-c-)
      target_compile_definitions(${target} PUBLIC _HAS_EXCEPTIONS=0)
    else()
      target_compile_options(${target} PUBLIC -fno-exceptions)
    endif()
  endif()
  if(SIMD_ENABLE_IPO)
    set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endfunction()

# CMake links clang-cl targets directly, so select the matching /MD ASan runtime
# from this compiler rather than silently borrowing an installed MSVC runtime.
function(simd_link_clang_cl_asan target)
  if(NOT SIMD_ENABLE_ASAN)
    return()
  endif()
  if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    return()
  endif()
  if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    message(FATAL_ERROR "The clang-cl ASan helper currently supports Windows x64 only.")
  endif()
  execute_process(COMMAND "${CMAKE_CXX_COMPILER}" --print-runtime-dir
    OUTPUT_VARIABLE runtime_dir OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)
  file(TO_CMAKE_PATH "${runtime_dir}" runtime_dir)
  set(asan "${runtime_dir}/clang_rt.asan_dynamic-x86_64")
  set(thunk "${runtime_dir}/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib")
  foreach(path "${asan}.lib" "${asan}.dll" "${thunk}")
    if(NOT EXISTS "${path}")
      message(FATAL_ERROR "Missing the selected clang-cl ASan runtime: ${path}")
    endif()
  endforeach()
  target_link_libraries(${target} PRIVATE "${asan}.lib")
  target_link_options(${target} PRIVATE "/wholearchive:${thunk}" /include:__asan_seh_interceptor /INCREMENTAL:NO)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${asan}.dll" "$<TARGET_FILE_DIR:${target}>"
    VERBATIM)
endfunction()
