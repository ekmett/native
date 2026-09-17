// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include <simd/vec.h>
#include <bit>

namespace simd {
  /// Repartition the bits of an integer register without conversion or a spill.
  /// Both element types must describe a supported shape of the same register.
  template <simd_integer_element To, simd_integer_element From, std::size_t N>
    requires (sizeof(From) * N % sizeof(To) == 0 && requires {
      typename vec<From,N,SIMD_ARCH>::native_type;
      typename vec<To,sizeof(From)*N/sizeof(To),SIMD_ARCH>::native_type;
    })
  simd_nodiscard simd_inline simd_const auto reinterpret_bits(vec<From,N,SIMD_ARCH> value) noexcept
      -> vec<To,sizeof(From)*N/sizeof(To),SIMD_ARCH> {
    using result = vec<To,sizeof(From)*N/sizeof(To),SIMD_ARCH>;
    return result::from_native(std::bit_cast<typename result::native_type>(value.to_native()));
  }

  /// Sum adjacent unsigned lanes into lanes twice as wide. No sum can overflow.
  /// The result occupies the same register and contains half as many lanes.
  template <simd_integer_element T, std::size_t N>
    requires (std::is_unsigned_v<T> && sizeof(T) <= 4 && N > 1 && N % 2 == 0 &&
      requires { typename vec<T,N,SIMD_ARCH>::native_type; })
  simd_nodiscard simd_inline simd_const auto pairwise_add_widened(vec<T,N,SIMD_ARCH> value) noexcept {
    using U = std::conditional_t<sizeof(T)==1,std::uint16_t,
              std::conditional_t<sizeof(T)==2,std::uint32_t,std::uint64_t>>;
    using result = vec<U,N/2,SIMD_ARCH>;
#if SIMD_HAS_ARM_NEON
    if constexpr (sizeof(T)==1)
      return result::from_native(vreinterpretq_u8_u16(vpaddlq_u8(value.to_native())));
    else if constexpr (sizeof(T)==2)
      return result::from_native(vreinterpretq_u8_u32(vpaddlq_u16(vreinterpretq_u16_u8(value.to_native()))));
    else return result::from_native(vreinterpretq_u8_u64(vpaddlq_u32(vreinterpretq_u32_u8(value.to_native()))));
#else
    // Two unsigned byte lanes sum to at most 510, well inside PMADDUBSW's
    // signed saturation bound. Wider lanes use exact register shifts/adds.
#if SIMD_HAS_AVX2
    if constexpr (sizeof(T)==1 && sizeof(T)*N==16)
      return result::from_native(_mm_maddubs_epi16(value.to_native(),_mm_set1_epi8(1)));
    else if constexpr (sizeof(T)==1 && sizeof(T)*N==32)
      return result::from_native(_mm256_maddubs_epi16(value.to_native(),_mm256_set1_epi8(1)));
    else
#endif
#if SIMD_HAS_AVX512BW
    if constexpr (sizeof(T)==1 && sizeof(T)*N==64)
      return result::from_native(_mm512_maddubs_epi16(value.to_native(),_mm512_set1_epi8(1)));
    else
#endif
    {
      auto words = reinterpret_bits<U>(value);
      return (words & result(U(std::numeric_limits<T>::max()))) + words.template right<8*sizeof(T)>();
    }
#endif
  }

  /// Count set bits independently in each unsigned integer lane.
  /// Byte populations use CNT on NEON and register nibble tables on x86.
  template <simd_integer_element T, std::size_t N>
    requires (std::is_unsigned_v<T> && requires { typename vec<T,N,SIMD_ARCH>::native_type; })
  simd_nodiscard simd_inline simd_const vec<T,N,SIMD_ARCH> popcount(vec<T,N,SIMD_ARCH> value) noexcept {
    using result = vec<T,N,SIMD_ARCH>;
    if constexpr (N==1) return result(T(std::popcount(value.to_native())));
    else if constexpr (sizeof(T)>1)
      return pairwise_add_widened(popcount(reinterpret_bits<
        std::conditional_t<sizeof(T)==2,std::uint8_t,
          std::conditional_t<sizeof(T)==4,std::uint16_t,std::uint32_t>>>(value)));
    else {
#if SIMD_HAS_ARM_NEON
      return result::from_native(vcntq_u8(value.to_native()));
#endif
#if SIMD_HAS_AVX2
      if constexpr (N==16) {
        auto table = _mm_setr_epi8(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
        auto mask = _mm_set1_epi8(15);
        auto data = value.to_native();
        return result::from_native(_mm_add_epi8(
          _mm_shuffle_epi8(table,_mm_and_si128(data,mask)),
          _mm_shuffle_epi8(table,_mm_and_si128(_mm_srli_epi16(data,4),mask))));
      } else if constexpr (N==32) {
        auto table = _mm256_broadcastsi128_si256(_mm_setr_epi8(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4));
        auto mask = _mm256_set1_epi8(15);
        auto data = value.to_native();
        return result::from_native(_mm256_add_epi8(
          _mm256_shuffle_epi8(table,_mm256_and_si256(data,mask)),
          _mm256_shuffle_epi8(table,_mm256_and_si256(_mm256_srli_epi16(data,4),mask))));
      }
#endif
#if SIMD_HAS_AVX512BW
      if constexpr (N==64) {
        auto table = _mm512_broadcast_i32x4(_mm_setr_epi8(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4));
        auto mask = _mm512_set1_epi8(15);
        auto data = value.to_native();
        return result::from_native(_mm512_add_epi8(
          _mm512_shuffle_epi8(table,_mm512_and_si512(data,mask)),
          _mm512_shuffle_epi8(table,_mm512_and_si512(_mm512_srli_epi16(data,4),mask))));
      }
#endif
    }
  }

  /// Sum unsigned 8-, 16-, or 32-bit lanes exactly into a 64-bit scalar.
  /// Reduction widens before adding: it never wraps at the input lane width.
  template <simd_integer_element T, std::size_t N>
    requires (std::is_unsigned_v<T> && sizeof(T)<=4 &&
      requires { typename vec<T,N,SIMD_ARCH>::native_type; })
  simd_nodiscard simd_inline simd_const std::uint64_t reduce_add_widened(vec<T,N,SIMD_ARCH> value) noexcept {
    if constexpr (N==1) return value.to_native();
    else {
#if SIMD_HAS_ARM_NEON
      if constexpr (sizeof(T)==1) return vaddlvq_u8(value.to_native());
      else if constexpr (sizeof(T)==2) return vaddlvq_u16(vreinterpretq_u16_u8(value.to_native()));
      else return vaddlvq_u32(vreinterpretq_u32_u8(value.to_native()));
#endif
#if SIMD_HAS_AVX2
      auto sums = [&] {
        if constexpr (sizeof(T)==1) {
          if constexpr (N==16) return _mm_sad_epu8(value.to_native(),_mm_setzero_si128());
          else if constexpr (N==32) return _mm256_sad_epu8(value.to_native(),_mm256_setzero_si256());
#if SIMD_HAS_AVX512BW
          else return _mm512_sad_epu8(value.to_native(),_mm512_setzero_si512());
#endif
        } else if constexpr (sizeof(T)==2)
          return pairwise_add_widened(pairwise_add_widened(value)).to_native();
        else return pairwise_add_widened(value).to_native();
      }();
      if constexpr (sizeof(T)*N==16)
        return std::uint64_t(_mm_cvtsi128_si64(_mm_add_epi64(sums,_mm_srli_si128(sums,8))));
      else if constexpr (sizeof(T)*N==32) {
        auto halves = _mm_add_epi64(_mm256_castsi256_si128(sums),_mm256_extracti128_si256(sums,1));
        return std::uint64_t(_mm_cvtsi128_si64(_mm_add_epi64(halves,_mm_srli_si128(halves,8))));
      }
#if SIMD_HAS_AVX512F
      else return std::uint64_t(_mm512_reduce_add_epi64(sums));
#endif
#endif
    }
  }
}
