
namespace NATIVE_BACKEND_NAMESPACE::native {
  namespace detail {
    template<class V> struct fp32_bit_bridge;
    template<std::size_t N, ::native::isa Arch>
      requires NATIVE_ARCH_REQUIRES(Arch) && requires { typename ::native::vec<float,N,Arch>::bits_type; }
    struct fp32_bit_bridge<::native::vec<float,N,Arch>> {
      using value_type=::native::vec<float,N,Arch>;
      using bits_type=typename value_type::bits_type;
      static native_inline bits_type encode(value_type x) noexcept { return x.bits(); }
      static native_inline value_type decode(bits_type x) noexcept { return value_type::from_bits(x); }
    };
  } // namespace detail
  template <class V>
  concept fp32_bits_register = requires { typename detail::fp32_bit_bridge<V>::bits_type; };

} // namespace NATIVE_BACKEND_NAMESPACE::native

namespace NATIVE_BACKEND_NAMESPACE::native {
  namespace detail {
    template <unsigned_register U> native_inline U flush_clear_bits(U bits, U clear) noexcept {
      return bits & (clear ^ U(0xffffffffu));
    }
#if NATIVE_HAS_AVX2
    native_inline uint32x8 flush_clear_bits(uint32x8 bits, uint32x8 clear) noexcept {
      return uint32x8::from_native(_mm256_andnot_si256(clear.value, bits.value));
    }
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
    native_inline uint32x16 flush_clear_bits(uint32x16 bits, uint32x16 clear) noexcept {
      return uint32x16::from_native(_mm512_andnot_si512(clear.value, bits.value));
    }
#endif
#if NATIVE_HAS_ARM_NEON
    native_inline uint32x4 flush_clear_bits(uint32x4 bits, uint32x4 clear) noexcept {
      return uint32x4::from_native(vbicq_u8(bits.value, clear.value));
    }
#endif
  } // namespace detail

  /** \ingroup vector_math
   * \brief Replace binary32 subnormal lanes with signed zero.
   * Both zero signs, normal values, infinities and every NaN payload/sign are
   * preserved exactly. This changes the values, not the floating-point control
   * state, and does not raise floating-point exceptions. Empty arrays are valid.
   * \snippet api.cc bit_transport
   */
  template <fp32_bits_register V, std::size_t N>
  native_inline std::array<V, N> flush_to_zero(std::array<V, N> const & x) noexcept {
    if constexpr (N == 0) return {};
    else {
      auto const & [...value] = x;
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
      if constexpr (V::lanes == 16) {
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
  /// \ingroup vector_math
  /// Apply the same bit-preserving signed-zero flush to one register.
  template <fp32_bits_register V> native_inline V flush_to_zero(V x) noexcept {
    return flush_to_zero(std::array{x})[0];
  }
} // namespace NATIVE_BACKEND_NAMESPACE::native

namespace native {
  using ::NATIVE_BACKEND_NAMESPACE::native::flush_to_zero;
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
