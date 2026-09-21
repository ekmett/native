// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup arm_aes AES rounds
  /// Single-round AES transformations on a 16-byte state; no key expansion,
  /// padding or cipher mode is performed. Bytes use the ACLE state ordering.
  /// Arch requires FEAT_AES. The compiler target "aes" also enables PMULL;
  /// admit the complete target_features<arm>("aes") set before entering that leaf.
  /// These integer operations do not read or modify FPCR, FPSR or NZCV.
  /// \{

  /// Apply AddRoundKey, SubBytes and ShiftRows to one AES state.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  simd<std::uint8_t, 16, Arch> aese(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    auto result = detail::arm_aes::aese<Arch>(__builtin_bit_cast(uint8x16_t, state.to_native()), __builtin_bit_cast(uint8x16_t, round_key.to_native()));
    return simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint8_t, 16, Arch>::native_type, result));
  }

  /// Apply AddRoundKey, inverse ShiftRows and inverse SubBytes to one AES state.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  simd<std::uint8_t, 16, Arch> aesd(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    auto result = detail::arm_aes::aesd<Arch>(__builtin_bit_cast(uint8x16_t, state.to_native()), __builtin_bit_cast(uint8x16_t, round_key.to_native()));
    return simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint8_t, 16, Arch>::native_type, result));
  }

  /// Apply AES MixColumns independently to the four state columns.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  simd<std::uint8_t, 16, Arch> aesmc(simd<std::uint8_t, 16, Arch> state) noexcept {
    auto result = detail::arm_aes::aesmc<Arch>(__builtin_bit_cast(uint8x16_t, state.to_native()));
    return simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint8_t, 16, Arch>::native_type, result));
  }

  /// Apply inverse AES MixColumns independently to the four state columns.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  simd<std::uint8_t, 16, Arch> aesimc(simd<std::uint8_t, 16, Arch> state) noexcept {
    auto result = detail::arm_aes::aesimc<Arch>(__builtin_bit_cast(uint8x16_t, state.to_native()));
    return simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint8_t, 16, Arch>::native_type, result));
  }

  // Require exact semantic shapes and a consistent feature tag.
  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa<arm> Arch, class... T> void aese(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa<arm> Arch, class... T> void aesd(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa<arm> Arch, class... T> void aesmc(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa<arm> Arch, class... T> void aesimc(T...) = delete;

  /// \}
}
#endif
