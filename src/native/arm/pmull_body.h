// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native::detail {
  template<isa<arm> Arch>
  constexpr simd<std::uint64_t,2,Arch> pmull_constant(std::uint64_t a,std::uint64_t b) noexcept {
    std::uint64_t low=0,high=0;
    for(unsigned bit=0;bit<64;++bit) if((b>>bit)&1) {
      low^=a<<bit;
      if(bit) high^=a>>(64-bit);
    }
    return simd<std::uint64_t,2,Arch>(low,high);
  }
}
export namespace native {
  /// \defgroup arm_pmull Polynomial multiplication
  /// Bit i is the coefficient of x^i. Products use XOR, without carry or
  /// reduction by a modulus. Byte products need NEON; runtime 64-bit products
  /// need FEAT_PMULL. The latter use the compiler "aes" target, whose full
  /// target_features<arm>("aes") set must be admitted before entering the leaf.
  /// These integer operations do not read or modify FPCR, FPSR or NZCV.
  /// Constant evaluation computes the same integer values. Feature-absent calls
  /// are immediate-only and still require complete SIMD storage for their Arch.
  /// \{

  /// Multiply two degree-at-most-63 polynomials over GF(2), returning all 128 coefficients.
  /// Arch defaults to the owning module's baseline, and is preserved in the result.
  /// Constant evaluation performs polynomial arithmetic; runtime evaluation uses PMULL.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint64_t, 2, Arch> pmull(std::uint64_t a, std::uint64_t b) noexcept {
    if consteval { return detail::pmull_constant<Arch>(a,b); }
    else {
      auto result = detail::arm_pmull::pmull<Arch>(a, b);
      return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, (uint64x2_t{
        static_cast<std::uint64_t>(result), static_cast<std::uint64_t>(result >> 64)})));
    }
  }

  /// Evaluate a 64-bit polynomial product at compile time using an available SIMD storage shape.
  template<isa<arm> Arch=NATIVE_BASELINE>
    requires(!Arch.has(arm_feature::pmull) && requires { sizeof(simd<std::uint64_t,2,Arch>); })
  native_nodiscard consteval simd<std::uint64_t,2,Arch> pmull(std::uint64_t a,std::uint64_t b) noexcept {
    return detail::pmull_constant<Arch>(a,b);
  }

  /// Multiply the high 64-bit polynomial lane of each operand; lower lanes are ignored.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint64_t, 2, Arch> pmull2(simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval { return detail::pmull_constant<Arch>(detail::arm_constant::lanes(a)[1], detail::arm_constant::lanes(b)[1]); }
    else {
      auto result = detail::arm_pmull::pmull2<Arch>(__builtin_bit_cast(poly64x2_t, a.to_native()), __builtin_bit_cast(poly64x2_t, b.to_native()));
      return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, (uint64x2_t{
        static_cast<std::uint64_t>(result), static_cast<std::uint64_t>(result >> 64)})));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::pmull) && requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> pmull2(simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return detail::pmull_constant<Arch>(detail::arm_constant::lanes(a)[1], detail::arm_constant::lanes(b)[1]);
  }

  /// Multiply eight pairs of byte polynomials into 16-bit products; requires only NEON.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::neon))
  native_nodiscard native_inline native_const
  constexpr simd<std::uint16_t, 8, Arch> pmull(simd<std::uint8_t, 8, Arch> a, simd<std::uint8_t, 8, Arch> b) noexcept {
    if consteval { return detail::arm_constant::pmull_bytes<simd<std::uint16_t, 8, Arch>, 0>(a, b); }
    else {
      auto result = detail::arm_pmull::pmull<Arch>(vreinterpret_p8_u8(a.to_native()), vreinterpret_p8_u8(b.to_native()));
      return simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint16_t, 8, Arch>::native_type, result));
    }
  }

  /// Multiply the high eight byte polynomials into 16-bit products; requires only NEON.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::neon))
  native_nodiscard native_inline native_const
  constexpr simd<std::uint16_t, 8, Arch> pmull2(simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    if consteval { return detail::arm_constant::pmull_bytes<simd<std::uint16_t, 8, Arch>, 8>(a, b); }
    else {
      auto result = detail::arm_pmull::pmull2<Arch>(vreinterpretq_p8_u8(a.to_native()), vreinterpretq_p8_u8(b.to_native()));
      return simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint16_t, 8, Arch>::native_type, result));
    }
  }

  // Require exact semantic shapes and a consistent feature tag.
  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa<arm> Arch=NATIVE_BASELINE, class... T> void pmull(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa<arm> Arch, class... T> void pmull2(T...) = delete;

  /// \}
}
#endif
