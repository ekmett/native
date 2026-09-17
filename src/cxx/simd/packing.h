// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include <simd/vec.h>
#include <bit>
#include <cstdint>
#include <type_traits>

namespace simd {
  /// Deposit the low popcount(mask) bits into mask's set positions, in order.
  /// The architecture tag selects an admitted instruction profile.
  simd_nodiscard simd_inline simd_const std::uint64_t deposit_bits(
      SIMD_ARCH, std::uint64_t value, std::uint64_t mask) noexcept {
#if SIMD_HAS_AVX2 && defined(__BMI2__)
    return _pdep_u64(value, mask);
#else
    std::uint64_t result = 0;
    while (mask) {
      auto bit = mask & (0 - mask);
      result |= (0 - (value & 1)) & bit;
      value >>= 1;
      mask &= mask - 1;
    }
    return result;
#endif
  }

#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON
  /// Truncate unsigned lanes to half their width and concatenate a then b.
  /// This preserves lane order and low bits; it does not saturate.
  template <simd_integer_element To, simd_integer_element From, std::size_t N>
    requires (std::is_unsigned_v<To> && std::is_unsigned_v<From> &&
      sizeof(From) == 2 * sizeof(To) && N * sizeof(From) >= 16 &&
      requires { sizeof(vec<From, N, SIMD_ARCH>); sizeof(vec<To, 2 * N, SIMD_ARCH>); })
  simd_nodiscard simd_inline simd_const vec<To, 2 * N, SIMD_ARCH> narrow_concat(
      vec<From, N, SIMD_ARCH> a, vec<From, N, SIMD_ARCH> b) noexcept {
    using result = vec<To, 2 * N, SIMD_ARCH>;
#if SIMD_HAS_ARM_NEON
    if constexpr (sizeof(From) == 8)
      return result::from_native(vreinterpretq_u8_u32(vcombine_u32(
        vmovn_u64(vreinterpretq_u64_u8(a.to_native())),
        vmovn_u64(vreinterpretq_u64_u8(b.to_native())))));
    else if constexpr (sizeof(From) == 4)
      return result::from_native(vreinterpretq_u8_u16(vcombine_u16(
        vmovn_u32(vreinterpretq_u32_u8(a.to_native())),
        vmovn_u32(vreinterpretq_u32_u8(b.to_native())))));
    else return result::from_native(vcombine_u8(
      vmovn_u16(vreinterpretq_u16_u8(a.to_native())),
      vmovn_u16(vreinterpretq_u16_u8(b.to_native()))));
#else
    if constexpr (N * sizeof(From) == 16) {
      if constexpr (sizeof(From) == 8)
        return result::from_native(_mm_unpacklo_epi64(
          _mm_shuffle_epi32(a.to_native(), _MM_SHUFFLE(2, 0, 2, 0)),
          _mm_shuffle_epi32(b.to_native(), _MM_SHUFFLE(2, 0, 2, 0))));
      else if constexpr (sizeof(From) == 4) {
        auto mask = _mm_set1_epi32(0xffff);
        return result::from_native(_mm_packus_epi32(
          _mm_and_si128(a.to_native(), mask), _mm_and_si128(b.to_native(), mask)));
      } else {
        auto mask = _mm_set1_epi16(0xff);
        return result::from_native(_mm_packus_epi16(
          _mm_and_si128(a.to_native(), mask), _mm_and_si128(b.to_native(), mask)));
      }
    } else if constexpr (N * sizeof(From) == 32) {
      if constexpr (sizeof(From) == 8) {
        auto order = _mm256_setr_epi32(0, 2, 4, 6, 0, 2, 4, 6);
        auto low = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(a.to_native(), order));
        auto high = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(b.to_native(), order));
        return result::from_native(_mm256_inserti128_si256(_mm256_castsi128_si256(low), high, 1));
      } else if constexpr (sizeof(From) == 4) {
        auto mask = _mm256_set1_epi32(0xffff);
        auto packed = _mm256_packus_epi32(_mm256_and_si256(a.to_native(), mask),
          _mm256_and_si256(b.to_native(), mask));
        return result::from_native(_mm256_permute4x64_epi64(packed, _MM_SHUFFLE(3, 1, 2, 0)));
      } else {
        auto mask = _mm256_set1_epi16(0xff);
        auto packed = _mm256_packus_epi16(_mm256_and_si256(a.to_native(), mask),
          _mm256_and_si256(b.to_native(), mask));
        return result::from_native(_mm256_permute4x64_epi64(packed, _MM_SHUFFLE(3, 1, 2, 0)));
      }
#if SIMD_HAS_AVX512F
    } else {
      auto truncate = [](auto x) {
        if constexpr (sizeof(From) == 8) return _mm512_cvtepi64_epi32(x);
        else if constexpr (sizeof(From) == 4) return _mm512_cvtepi32_epi16(x);
        else return _mm512_cvtepi16_epi8(x);
      };
      auto low = truncate(a.to_native()), high = truncate(b.to_native());
      return result::from_native(_mm512_inserti64x4(_mm512_castsi256_si512(low), high, 1));
#endif
    }
#endif
  }
#endif
}
