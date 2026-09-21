// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup arm_sm4 SM4 instructions
  /// Individual Advanced SIMD operations on four native-order uint32_t words.
  /// Runtime calls require the named instruction feature; the compiler's "sm4"
  /// target enables both SM3 and SM4. Admit target_features<arm>("sm4") before
  /// entering its compiled leaf. These operations do not modify FPCR, FPSR or NZCV.
  /// Feature-absent overloads are immediate-only and require NEON vector storage.
  /// \{

  /// Perform four SM4 data rounds with state words and successive round keys in ascending lanes.
  template<isa<arm> Arch>
    requires(Arch.has(arm_feature::sm4))
  native_nodiscard native_inline native_const
  native_target("sm4") constexpr simd<std::uint32_t, 4, Arch>
  sm4e(simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::arm_sm4_constant::sm4e(a, b);
    } else {
      auto result = detail::arm_sm4::sm4e<Arch>(__builtin_bit_cast(uint32x4_t, a.to_native()),
        __builtin_bit_cast(uint32x4_t, b.to_native()));
      return simd<std::uint32_t, 4, Arch>::from_native(
        __builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the SM4 instruction feature.
  template<isa<arm> Arch>
    requires(!Arch.has(arm_feature::sm4) && requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sm4e(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::arm_sm4_constant::sm4e(a, b);
  }

  /// Perform four SM4 key-schedule rounds with state words and successive constants in ascending
  /// lanes.
  template<isa<arm> Arch>
    requires(Arch.has(arm_feature::sm4))
  native_nodiscard native_inline native_const
  native_target("sm4") constexpr simd<std::uint32_t, 4, Arch>
  sm4ekey(simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::arm_sm4_constant::sm4ekey(a, b);
    } else {
      auto result = detail::arm_sm4::sm4ekey<Arch>(__builtin_bit_cast(uint32x4_t, a.to_native()),
        __builtin_bit_cast(uint32x4_t, b.to_native()));
      return simd<std::uint32_t, 4, Arch>::from_native(
        __builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the SM4 instruction feature.
  template<isa<arm> Arch>
    requires(!Arch.has(arm_feature::sm4) && requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sm4ekey(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::arm_sm4_constant::sm4ekey(a, b);
  }

  /// \}
} // namespace native
#endif
