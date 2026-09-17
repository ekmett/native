#pragma once
#include "simd/config.h"
#include "simd/vec.h"
#include <array>

namespace SIMD_BACKEND_NAMESPACE::native {
  namespace detail {
    template <class V> struct fp32_bit_bridge;
    template <> struct fp32_bit_bridge<fp32x1> {
      using bits_type = uint32x1;
      static simd_inline bits_type encode(fp32x1 x) noexcept {
        return bits_type(std::bit_cast<std::uint32_t>(x.value));
      }
      static simd_inline fp32x1 decode(bits_type x) noexcept {
        return fp32x1(std::bit_cast<float>(x.value));
      }
    };
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON
    template <std::size_t N> requires (N == 2 || N == 3)
    struct fp32_bit_bridge<::simd::vec<float,N,SIMD_ARCH>> {
      using value_type = ::simd::vec<float,N,SIMD_ARCH>;
      using bits_type = typename value_type::bits_type;
      static simd_inline bits_type encode(value_type x) noexcept { return x.bits(); }
      static simd_inline value_type decode(bits_type x) noexcept { return value_type::from_bits(x); }
    };
#endif
#if SIMD_HAS_AVX2
    template <> struct fp32_bit_bridge<fp32x4> {
      using bits_type = uint32x4;
      static simd_inline bits_type encode(fp32x4 x) noexcept {
        return bits_type::from_native(_mm_castps_si128(x.value));
      }
      static simd_inline fp32x4 decode(bits_type x) noexcept {
        return fp32x4(_mm_castsi128_ps(x.value));
      }
    };
    template <> struct fp32_bit_bridge<fp32x8> {
      using bits_type = uint32x8;
      static simd_inline bits_type encode(fp32x8 x) noexcept {
        return bits_type::from_native(_mm256_castps_si256(x.value));
      }
      static simd_inline fp32x8 decode(bits_type x) noexcept {
        return fp32x8(_mm256_castsi256_ps(x.value));
      }
    };
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    template <> struct fp32_bit_bridge<fp32x16> {
      using bits_type = uint32x16;
      static simd_inline bits_type encode(fp32x16 x) noexcept {
        return bits_type::from_native(_mm512_castps_si512(x.value));
      }
      static simd_inline fp32x16 decode(bits_type x) noexcept {
        return fp32x16(_mm512_castsi512_ps(x.value));
      }
    };
#endif
#if SIMD_HAS_ARM_NEON
    template <> struct fp32_bit_bridge<fp32x4> {
      using bits_type = uint32x4;
      static simd_inline bits_type encode(fp32x4 x) noexcept {
        return bits_type::from_native(vreinterpretq_u8_f32(x.value));
      }
      static simd_inline fp32x4 decode(bits_type x) noexcept {
        return fp32x4(vreinterpretq_f32_u8(x.value));
      }
    };
#endif
  } // namespace detail
  template <class V>
  concept fp32_bits_register = requires { typename detail::fp32_bit_bridge<V>::bits_type; };

} // namespace SIMD_BACKEND_NAMESPACE::native

namespace SIMD_BACKEND_NAMESPACE::native {
  namespace detail {
    template <unsigned_register U> simd_inline U flush_clear_bits(U bits, U clear) noexcept {
      return bits & (clear ^ U(0xffffffffu));
    }
#if SIMD_HAS_AVX2
    simd_inline uint32x8 flush_clear_bits(uint32x8 bits, uint32x8 clear) noexcept {
      return uint32x8::from_native(_mm256_andnot_si256(clear.value, bits.value));
    }
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    simd_inline uint32x16 flush_clear_bits(uint32x16 bits, uint32x16 clear) noexcept {
      return uint32x16::from_native(_mm512_andnot_si512(clear.value, bits.value));
    }
#endif
#if SIMD_HAS_ARM_NEON
    simd_inline uint32x4 flush_clear_bits(uint32x4 bits, uint32x4 clear) noexcept {
      return uint32x4::from_native(vbicq_u8(bits.value, clear.value));
    }
#endif
  } // namespace detail

  // Clear the mantissa only when the exponent is zero. Both zero signs, all
  // normal values, infinities and every NaN payload/sign remain byte-exact.
  // Integer operations/bit casts, or AVX512 classification and masked bitwise AND;
  // no FP arithmetic, exceptions or environment change.
  template <fp32_bits_register V, std::size_t N>
  simd_inline std::array<V, N> flush_to_zero(std::array<V, N> const & x) noexcept {
    if constexpr (N == 0) return {};
    else {
      auto const & [...value] = x;
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
      if constexpr (std::same_as<V, fp32x16>) {
        // Include both signed zeros: DAZ may classify a denormal as zero.
        auto const [...tiny] = std::array{_mm512_fpclass_ps_mask(value.value, 0x26)...};
        auto const sign = _mm512_castsi512_ps(_mm512_set1_epi32(
          std::bit_cast<std::int32_t>(0x80000000u)));
        return {{V(_mm512_mask_and_ps(value.value, tiny, value.value, sign))...}};
      } else
#endif
      {
        using bridge = detail::fp32_bit_bridge<V>;
        using U = typename bridge::bits_type;
        auto [...bits] = std::array{bridge::encode(value)...};
        auto const [...exponent] = std::array{(bits & U(0x7f800000u))...};
        auto const [...zero] = std::array{mask_bits<uint32_t>(exponent == U(0))...};
        auto const [...clear] = std::array{(zero & U(0x007fffffu))...};
        ((bits = detail::flush_clear_bits(bits, clear)), ...);
        return {{bridge::decode(bits)...}};
      }
    }
  }
  template <fp32_bits_register V> simd_inline V flush_to_zero(V x) noexcept {
    return flush_to_zero(std::array{x})[0];
  }
} // namespace SIMD_BACKEND_NAMESPACE::native

namespace simd {
  using SIMD_BACKEND_NAMESPACE::native::flush_to_zero;
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Native bit projection and signed subnormal flushing.
 */
