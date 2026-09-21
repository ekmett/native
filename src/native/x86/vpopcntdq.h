// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native::detail::x86_vpopcntdq {
// Internal register helpers for the native.x86.vpopcntdq module.


  /// Count set bits in each of the 16 32-bit lanes; results are in [0, 32].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  __m512i vpopcntd(__m512i value) noexcept {
    return _mm512_popcnt_epi32(value);
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  __m512i mask_vpopcntd(__m512i source, __mmask16 mask, __m512i value) noexcept {
    return _mm512_mask_popcnt_epi32(source, mask, value);
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  __m512i maskz_vpopcntd(__mmask16 mask, __m512i value) noexcept {
    return _mm512_maskz_popcnt_epi32(mask, value);
  }

  /// Count set bits in each of the 8 64-bit lanes; results are in [0, 64].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  __m512i vpopcntq(__m512i value) noexcept {
    return _mm512_popcnt_epi64(value);
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  __m512i mask_vpopcntq(__m512i source, __mmask8 mask, __m512i value) noexcept {
    return _mm512_mask_popcnt_epi64(source, mask, value);
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  __m512i maskz_vpopcntq(__mmask8 mask, __m512i value) noexcept {
    return _mm512_maskz_popcnt_epi64(mask, value);
  }

  /// Count set bits in each of the 4 32-bit lanes; results are in [0, 32].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m128i vpopcntd(__m128i value) noexcept {
    return _mm_popcnt_epi32(value);
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m128i mask_vpopcntd(__m128i source, __mmask8 mask, __m128i value) noexcept {
    return _mm_mask_popcnt_epi32(source, mask, value);
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m128i maskz_vpopcntd(__mmask8 mask, __m128i value) noexcept {
    return _mm_maskz_popcnt_epi32(mask, value);
  }

  /// Count set bits in each of the 2 64-bit lanes; results are in [0, 64].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m128i vpopcntq(__m128i value) noexcept {
    return _mm_popcnt_epi64(value);
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m128i mask_vpopcntq(__m128i source, __mmask8 mask, __m128i value) noexcept {
    return _mm_mask_popcnt_epi64(source, mask, value);
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m128i maskz_vpopcntq(__mmask8 mask, __m128i value) noexcept {
    return _mm_maskz_popcnt_epi64(mask, value);
  }

  /// Count set bits in each of the 8 32-bit lanes; results are in [0, 32].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m256i vpopcntd(__m256i value) noexcept {
    return _mm256_popcnt_epi32(value);
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m256i mask_vpopcntd(__m256i source, __mmask8 mask, __m256i value) noexcept {
    return _mm256_mask_popcnt_epi32(source, mask, value);
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m256i maskz_vpopcntd(__mmask8 mask, __m256i value) noexcept {
    return _mm256_maskz_popcnt_epi32(mask, value);
  }

  /// Count set bits in each of the 4 64-bit lanes; results are in [0, 64].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m256i vpopcntq(__m256i value) noexcept {
    return _mm256_popcnt_epi64(value);
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m256i mask_vpopcntq(__m256i source, __mmask8 mask, __m256i value) noexcept {
    return _mm256_mask_popcnt_epi64(source, mask, value);
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  __m256i maskz_vpopcntq(__mmask8 mask, __m256i value) noexcept {
    return _mm256_maskz_popcnt_epi64(mask, value);
  }

}
#endif
