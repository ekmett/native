// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native::detail::x86_bitalg {
  // Internal register helpers for native.x86.bitalg.

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m128i vpopcntb(__m128i value) noexcept {
    return _mm_popcnt_epi8(value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m128i mask_vpopcntb(__m128i source, __mmask16 mask, __m128i value) noexcept {
    return _mm_mask_popcnt_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m128i maskz_vpopcntb(__mmask16 mask, __m128i value) noexcept {
    return _mm_maskz_popcnt_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m128i vpopcntw(__m128i value) noexcept {
    return _mm_popcnt_epi16(value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m128i mask_vpopcntw(__m128i source, __mmask8 mask, __m128i value) noexcept {
    return _mm_mask_popcnt_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m128i maskz_vpopcntw(__mmask8 mask, __m128i value) noexcept {
    return _mm_maskz_popcnt_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __mmask16 vpshufbitqmb(__m128i value, __m128i control) noexcept {
    return _mm_bitshuffle_epi64_mask(value, control);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __mmask16 mask_vpshufbitqmb(__mmask16 mask, __m128i value, __m128i control) noexcept {
    return _mm_mask_bitshuffle_epi64_mask(mask, value, control);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m256i vpopcntb(__m256i value) noexcept {
    return _mm256_popcnt_epi8(value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m256i mask_vpopcntb(__m256i source, __mmask32 mask, __m256i value) noexcept {
    return _mm256_mask_popcnt_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m256i maskz_vpopcntb(__mmask32 mask, __m256i value) noexcept {
    return _mm256_maskz_popcnt_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m256i vpopcntw(__m256i value) noexcept {
    return _mm256_popcnt_epi16(value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m256i mask_vpopcntw(__m256i source, __mmask16 mask, __m256i value) noexcept {
    return _mm256_mask_popcnt_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __m256i maskz_vpopcntw(__mmask16 mask, __m256i value) noexcept {
    return _mm256_maskz_popcnt_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __mmask32 vpshufbitqmb(__m256i value, __m256i control) noexcept {
    return _mm256_bitshuffle_epi64_mask(value, control);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
  __mmask32 mask_vpshufbitqmb(__mmask32 mask, __m256i value, __m256i control) noexcept {
    return _mm256_mask_bitshuffle_epi64_mask(mask, value, control);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __m512i vpopcntb(__m512i value) noexcept {
    return _mm512_popcnt_epi8(value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __m512i mask_vpopcntb(__m512i source, __mmask64 mask, __m512i value) noexcept {
    return _mm512_mask_popcnt_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __m512i maskz_vpopcntb(__mmask64 mask, __m512i value) noexcept {
    return _mm512_maskz_popcnt_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __m512i vpopcntw(__m512i value) noexcept {
    return _mm512_popcnt_epi16(value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __m512i mask_vpopcntw(__m512i source, __mmask32 mask, __m512i value) noexcept {
    return _mm512_mask_popcnt_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __m512i maskz_vpopcntw(__mmask32 mask, __m512i value) noexcept {
    return _mm512_maskz_popcnt_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __mmask64 vpshufbitqmb(__m512i value, __m512i control) noexcept {
    return _mm512_bitshuffle_epi64_mask(value, control);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512bitalg))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512bitalg")
  __mmask64 mask_vpshufbitqmb(__mmask64 mask, __m512i value, __m512i control) noexcept {
    return _mm512_mask_bitshuffle_epi64_mask(mask, value, control);
  }
}
#endif

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace native::detail::x86_bitalg_constant {
  template<class V>
  constexpr V population(V value, V source, std::uint64_t mask) noexcept {
    using value_type = typename V::value_type;
    std::array<value_type, V::lanes> input{};
    std::array<value_type, V::lanes> result{};
    value.store(input.data());
    source.store(result.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        result[lane] = static_cast<value_type>(std::popcount(input[lane]));
      }
    }
    return V::load(result.data());
  }

  template<class P, class V, class C>
  constexpr P bitshuffle(V value, C control, std::uint64_t mask) noexcept {
    std::array<std::uint64_t, V::lanes> words{};
    std::array<std::uint8_t, C::lanes> selectors{};
    value.store(words.data());
    control.store(selectors.data());
    std::uint64_t result = 0;
    for (std::size_t lane = 0; lane < C::lanes; ++lane) {
      // Each control byte selects within its own source qword. Bits 6/7 are ignored.
      auto bit = (words[lane / 8] >> (selectors[lane] & 63)) & 1;
      result |= bit << lane;
    }
    return P::from_bitset(result & mask);
  }
}
