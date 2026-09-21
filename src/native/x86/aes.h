// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
/// \cond NATIVE_DETAIL
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native::detail::x86_aes {

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  __m128i aesenc(__m128i state, __m128i round_key) noexcept {
    return _mm_aesenc_si128(state, round_key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  __m128i aesenclast(__m128i state, __m128i round_key) noexcept {
    return _mm_aesenclast_si128(state, round_key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  __m128i aesdec(__m128i state, __m128i round_key) noexcept {
    return _mm_aesdec_si128(state, round_key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  __m128i aesdeclast(__m128i state, __m128i round_key) noexcept {
    return _mm_aesdeclast_si128(state, round_key);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  __m128i aesimc(__m128i state) noexcept {
    return _mm_aesimc_si128(state);
  }

  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::aes) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("aes")
  __m128i aeskeygenassist(__m128i state) noexcept {
    return _mm_aeskeygenassist_si128(state, Imm8);
  }

  template<isa<x86> Arch, class... Args>
  void aesenc(Args...) = delete;

  template<isa<x86> Arch, class... Args>
  void aesenclast(Args...) = delete;

  template<isa<x86> Arch, class... Args>
  void aesdec(Args...) = delete;

  template<isa<x86> Arch, class... Args>
  void aesdeclast(Args...) = delete;

  template<isa<x86> Arch, class... Args>
  void aesimc(Args...) = delete;

  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void aeskeygenassist(Args...) = delete;
}
#endif
/// \endcond
