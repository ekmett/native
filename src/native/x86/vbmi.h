// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native::detail::x86_vbmi {
  // Internal register helpers for native.x86.vbmi.

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i vpermb(
    __m128i indices,
    __m128i value) noexcept {
    return _mm_permutexvar_epi8(indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i mask_vpermb(
    __m128i source,
    __mmask16 mask,
    __m128i indices,
    __m128i value) noexcept {
    return _mm_mask_permutexvar_epi8(source, mask, indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i maskz_vpermb(
    __mmask16 mask,
    __m128i indices,
    __m128i value) noexcept {
    return _mm_maskz_permutexvar_epi8(mask, indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i vpermt2b(
    __m128i a,
    __m128i indices,
    __m128i b) noexcept {
    return _mm_permutex2var_epi8(a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i mask_vpermt2b(
    __m128i a,
    __mmask16 mask,
    __m128i indices,
    __m128i b) noexcept {
    return _mm_mask_permutex2var_epi8(a, mask, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i maskz_vpermt2b(
    __mmask16 mask,
    __m128i a,
    __m128i indices,
    __m128i b) noexcept {
    return _mm_maskz_permutex2var_epi8(mask, a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i vpermi2b(
    __m128i indices,
    __m128i a,
    __m128i b) noexcept {
    return _mm_permutex2var_epi8(a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i mask_vpermi2b(
    __m128i indices,
    __mmask16 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_mask2_permutex2var_epi8(a, indices, mask, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i maskz_vpermi2b(
    __mmask16 mask,
    __m128i indices,
    __m128i a,
    __m128i b) noexcept {
    return _mm_maskz_permutex2var_epi8(mask, a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i vpmultishiftqb(
    __m128i control,
    __m128i value) noexcept {
    return _mm_multishift_epi64_epi8(control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i mask_vpmultishiftqb(
    __m128i source,
    __mmask16 mask,
    __m128i control,
    __m128i value) noexcept {
    return _mm_mask_multishift_epi64_epi8(source, mask, control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m128i maskz_vpmultishiftqb(
    __mmask16 mask,
    __m128i control,
    __m128i value) noexcept {
    return _mm_maskz_multishift_epi64_epi8(mask, control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i vpermb(
    __m256i indices,
    __m256i value) noexcept {
    return _mm256_permutexvar_epi8(indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i mask_vpermb(
    __m256i source,
    __mmask32 mask,
    __m256i indices,
    __m256i value) noexcept {
    return _mm256_mask_permutexvar_epi8(source, mask, indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i maskz_vpermb(
    __mmask32 mask,
    __m256i indices,
    __m256i value) noexcept {
    return _mm256_maskz_permutexvar_epi8(mask, indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i vpermt2b(
    __m256i a,
    __m256i indices,
    __m256i b) noexcept {
    return _mm256_permutex2var_epi8(a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i mask_vpermt2b(
    __m256i a,
    __mmask32 mask,
    __m256i indices,
    __m256i b) noexcept {
    return _mm256_mask_permutex2var_epi8(a, mask, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i maskz_vpermt2b(
    __mmask32 mask,
    __m256i a,
    __m256i indices,
    __m256i b) noexcept {
    return _mm256_maskz_permutex2var_epi8(mask, a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i vpermi2b(
    __m256i indices,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_permutex2var_epi8(a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i mask_vpermi2b(
    __m256i indices,
    __mmask32 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_mask2_permutex2var_epi8(a, indices, mask, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i maskz_vpermi2b(
    __mmask32 mask,
    __m256i indices,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_maskz_permutex2var_epi8(mask, a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i vpmultishiftqb(
    __m256i control,
    __m256i value) noexcept {
    return _mm256_multishift_epi64_epi8(control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i mask_vpmultishiftqb(
    __m256i source,
    __mmask32 mask,
    __m256i control,
    __m256i value) noexcept {
    return _mm256_mask_multishift_epi64_epi8(source, mask, control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
  __m256i maskz_vpmultishiftqb(
    __mmask32 mask,
    __m256i control,
    __m256i value) noexcept {
    return _mm256_maskz_multishift_epi64_epi8(mask, control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i vpermb(
    __m512i indices,
    __m512i value) noexcept {
    return _mm512_permutexvar_epi8(indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i mask_vpermb(
    __m512i source,
    __mmask64 mask,
    __m512i indices,
    __m512i value) noexcept {
    return _mm512_mask_permutexvar_epi8(source, mask, indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i maskz_vpermb(
    __mmask64 mask,
    __m512i indices,
    __m512i value) noexcept {
    return _mm512_maskz_permutexvar_epi8(mask, indices, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i vpermt2b(
    __m512i a,
    __m512i indices,
    __m512i b) noexcept {
    return _mm512_permutex2var_epi8(a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i mask_vpermt2b(
    __m512i a,
    __mmask64 mask,
    __m512i indices,
    __m512i b) noexcept {
    return _mm512_mask_permutex2var_epi8(a, mask, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i maskz_vpermt2b(
    __mmask64 mask,
    __m512i a,
    __m512i indices,
    __m512i b) noexcept {
    return _mm512_maskz_permutex2var_epi8(mask, a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i vpermi2b(
    __m512i indices,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_permutex2var_epi8(a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i mask_vpermi2b(
    __m512i indices,
    __mmask64 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_mask2_permutex2var_epi8(a, indices, mask, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i maskz_vpermi2b(
    __mmask64 mask,
    __m512i indices,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_maskz_permutex2var_epi8(mask, a, indices, b);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i vpmultishiftqb(
    __m512i control,
    __m512i value) noexcept {
    return _mm512_multishift_epi64_epi8(control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i mask_vpmultishiftqb(
    __m512i source,
    __mmask64 mask,
    __m512i control,
    __m512i value) noexcept {
    return _mm512_mask_multishift_epi64_epi8(source, mask, control, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi")
  __m512i maskz_vpmultishiftqb(
    __mmask64 mask,
    __m512i control,
    __m512i value) noexcept {
    return _mm512_maskz_multishift_epi64_epi8(mask, control, value);
  }
}
#endif
