// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native::detail::x86_vaes {
  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,aes")
  __m128i vaesenc(__m128i state, __m128i key) noexcept {
    return _mm_aesenc_si128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,vaes")
  __m256i vaesenc(__m256i state, __m256i key) noexcept {
    return _mm256_aesenc_epi128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx512f))
  native_nodiscard native_inline native_const native_target("avx512f,vaes")
  __m512i vaesenc(__m512i state, __m512i key) noexcept {
    return _mm512_aesenc_epi128(state, key);
  }

  template<isa<x86> Arch, class... Args>
  void vaesenc(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,aes")
  __m128i vaesenclast(__m128i state, __m128i key) noexcept {
    return _mm_aesenclast_si128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,vaes")
  __m256i vaesenclast(__m256i state, __m256i key) noexcept {
    return _mm256_aesenclast_epi128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx512f))
  native_nodiscard native_inline native_const native_target("avx512f,vaes")
  __m512i vaesenclast(__m512i state, __m512i key) noexcept {
    return _mm512_aesenclast_epi128(state, key);
  }

  template<isa<x86> Arch, class... Args>
  void vaesenclast(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,aes")
  __m128i vaesdec(__m128i state, __m128i key) noexcept {
    return _mm_aesdec_si128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,vaes")
  __m256i vaesdec(__m256i state, __m256i key) noexcept {
    return _mm256_aesdec_epi128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx512f))
  native_nodiscard native_inline native_const native_target("avx512f,vaes")
  __m512i vaesdec(__m512i state, __m512i key) noexcept {
    return _mm512_aesdec_epi128(state, key);
  }

  template<isa<x86> Arch, class... Args>
  void vaesdec(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,aes")
  __m128i vaesdeclast(__m128i state, __m128i key) noexcept {
    return _mm_aesdeclast_si128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,vaes")
  __m256i vaesdeclast(__m256i state, __m256i key) noexcept {
    return _mm256_aesdeclast_epi128(state, key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::vaes) && Arch.has(x86_feature::avx512f))
  native_nodiscard native_inline native_const native_target("avx512f,vaes")
  __m512i vaesdeclast(__m512i state, __m512i key) noexcept {
    return _mm512_aesdeclast_epi128(state, key);
  }

  template<isa<x86> Arch, class... Args>
  void vaesdeclast(Args...) = delete;

}
#endif
