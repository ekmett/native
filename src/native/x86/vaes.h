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

#include "native/x86/aes_constant.h"

namespace native::detail::x86_vaes_constant {
  // Every 128-bit lane is a separate AES state with its own round key. Reuse
  // the AES substitution and field-mixing primitives without cross-lane work.
  template<bool Inverse, bool Last, class V>
  constexpr V round(V state, V key) noexcept {
    auto input = x86_aes_constant::lanes(state);
    auto round_key = x86_aes_constant::lanes(key);
    auto result = input;
    for (unsigned base = 0; base < V::lanes; base += 16) {
      std::array<std::uint8_t, 16> block{};
      for (unsigned column = 0; column < 4; ++column) {
        for (unsigned row = 0; row < 4; ++row) {
          auto source = 4 * ((column + (Inverse ? 4 - row : row)) % 4) + row;
          block[4 * column + row] = x86_aes_constant::substitute<Inverse>(input[base + source]);
        }
      }
      if constexpr (!Last) {
        block = x86_aes_constant::mix<Inverse>(block);
      }
      for (unsigned i = 0; i < 16; ++i) {
        result[base + i] = block[i] ^ round_key[base + i];
      }
    }
    return V::load(result.data());
  }
}
