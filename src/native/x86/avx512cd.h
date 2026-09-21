// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native::detail::x86_avx512cd {
// Internal register helpers for native.x86.avx512cd.

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i vpconflictd(__m128i value) noexcept {
    return _mm_conflict_epi32(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i mask_vpconflictd(__m128i source, __mmask8 mask, __m128i value) noexcept {
    return _mm_mask_conflict_epi32(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i maskz_vpconflictd(__mmask8 mask, __m128i value) noexcept {
    return _mm_maskz_conflict_epi32(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i vplzcntd(__m128i value) noexcept {
    return _mm_lzcnt_epi32(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i mask_vplzcntd(__m128i source, __mmask8 mask, __m128i value) noexcept {
    return _mm_mask_lzcnt_epi32(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i maskz_vplzcntd(__mmask8 mask, __m128i value) noexcept {
    return _mm_maskz_lzcnt_epi32(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i vpconflictq(__m128i value) noexcept {
    return _mm_conflict_epi64(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i mask_vpconflictq(__m128i source, __mmask8 mask, __m128i value) noexcept {
    return _mm_mask_conflict_epi64(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i maskz_vpconflictq(__mmask8 mask, __m128i value) noexcept {
    return _mm_maskz_conflict_epi64(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i vplzcntq(__m128i value) noexcept {
    return _mm_lzcnt_epi64(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i mask_vplzcntq(__m128i source, __mmask8 mask, __m128i value) noexcept {
    return _mm_mask_lzcnt_epi64(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m128i maskz_vplzcntq(__mmask8 mask, __m128i value) noexcept {
    return _mm_maskz_lzcnt_epi64(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i vpconflictd(__m256i value) noexcept {
    return _mm256_conflict_epi32(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i mask_vpconflictd(__m256i source, __mmask8 mask, __m256i value) noexcept {
    return _mm256_mask_conflict_epi32(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i maskz_vpconflictd(__mmask8 mask, __m256i value) noexcept {
    return _mm256_maskz_conflict_epi32(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i vplzcntd(__m256i value) noexcept {
    return _mm256_lzcnt_epi32(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i mask_vplzcntd(__m256i source, __mmask8 mask, __m256i value) noexcept {
    return _mm256_mask_lzcnt_epi32(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i maskz_vplzcntd(__mmask8 mask, __m256i value) noexcept {
    return _mm256_maskz_lzcnt_epi32(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i vpconflictq(__m256i value) noexcept {
    return _mm256_conflict_epi64(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i mask_vpconflictq(__m256i source, __mmask8 mask, __m256i value) noexcept {
    return _mm256_mask_conflict_epi64(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i maskz_vpconflictq(__mmask8 mask, __m256i value) noexcept {
    return _mm256_maskz_conflict_epi64(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i vplzcntq(__m256i value) noexcept {
    return _mm256_lzcnt_epi64(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i mask_vplzcntq(__m256i source, __mmask8 mask, __m256i value) noexcept {
    return _mm256_mask_lzcnt_epi64(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  __m256i maskz_vplzcntq(__mmask8 mask, __m256i value) noexcept {
    return _mm256_maskz_lzcnt_epi64(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i vpconflictd(__m512i value) noexcept {
    return _mm512_conflict_epi32(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i mask_vpconflictd(__m512i source, __mmask16 mask, __m512i value) noexcept {
    return _mm512_mask_conflict_epi32(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i maskz_vpconflictd(__mmask16 mask, __m512i value) noexcept {
    return _mm512_maskz_conflict_epi32(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i vplzcntd(__m512i value) noexcept {
    return _mm512_lzcnt_epi32(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i mask_vplzcntd(__m512i source, __mmask16 mask, __m512i value) noexcept {
    return _mm512_mask_lzcnt_epi32(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i maskz_vplzcntd(__mmask16 mask, __m512i value) noexcept {
    return _mm512_maskz_lzcnt_epi32(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i vpconflictq(__m512i value) noexcept {
    return _mm512_conflict_epi64(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i mask_vpconflictq(__m512i source, __mmask8 mask, __m512i value) noexcept {
    return _mm512_mask_conflict_epi64(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i maskz_vpconflictq(__mmask8 mask, __m512i value) noexcept {
    return _mm512_maskz_conflict_epi64(mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i vplzcntq(__m512i value) noexcept {
    return _mm512_lzcnt_epi64(value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i mask_vplzcntq(__m512i source, __mmask8 mask, __m512i value) noexcept {
    return _mm512_mask_lzcnt_epi64(source, mask, value);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  __m512i maskz_vplzcntq(__mmask8 mask, __m512i value) noexcept {
    return _mm512_maskz_lzcnt_epi64(mask, value);
  }
}
#endif
