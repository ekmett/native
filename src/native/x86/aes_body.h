// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
/** \defgroup x86_aes AES
 * AES-NI transformations of one 16-byte state in little-endian register order.
 * Bytes 4 * column + row form the AES state matrix. Encryption and decryption
 * rounds add the key last, unlike ARM AESE/AESD. AESDEC uses inverse-mixed
 * intermediate round keys; AESDECLAST uses the original initial key.
 * Each runtime operation requires AES; the SIMD storage requires SSE2.
 * Arch records requirements; the caller must separately enable and admit its
 * target. These integer operations do not modify floating-point status.
 * Constant evaluation computes the same values. Feature-absent tags are accepted
 * only at compile time and still require complete SIMD storage.
 * \{ */

  /// Apply ShiftRows, SubBytes, MixColumns and then AddRoundKey.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint8_t, 16, Arch> aesenc(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    if consteval { return detail::x86_aes_constant::round<false, false>(state, round_key); }
    else {
      return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_aes::aesenc<Arch>(state.to_native(), round_key.to_native()));
    }
  }

  /// Evaluate constant operands without the AES instruction feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::aes) &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> aesenc(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    return detail::x86_aes_constant::round<false, false>(state, round_key);
  }

  /// Apply ShiftRows, SubBytes and then AddRoundKey for the final encryption round.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint8_t, 16, Arch> aesenclast(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    if consteval { return detail::x86_aes_constant::round<false, true>(state, round_key); }
    else {
      return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_aes::aesenclast<Arch>(state.to_native(), round_key.to_native()));
    }
  }

  /// Evaluate constant operands without the AES instruction feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::aes) &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> aesenclast(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    return detail::x86_aes_constant::round<false, true>(state, round_key);
  }

  /// Apply inverse ShiftRows, inverse SubBytes, inverse MixColumns and then AddRoundKey.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint8_t, 16, Arch> aesdec(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    if consteval { return detail::x86_aes_constant::round<true, false>(state, round_key); }
    else {
      return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_aes::aesdec<Arch>(state.to_native(), round_key.to_native()));
    }
  }

  /// Evaluate constant operands without the AES instruction feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::aes) &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> aesdec(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    return detail::x86_aes_constant::round<true, false>(state, round_key);
  }

  /// Apply inverse ShiftRows, inverse SubBytes and then AddRoundKey for the final decryption round.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint8_t, 16, Arch> aesdeclast(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    if consteval { return detail::x86_aes_constant::round<true, true>(state, round_key); }
    else {
      return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_aes::aesdeclast<Arch>(state.to_native(), round_key.to_native()));
    }
  }

  /// Evaluate constant operands without the AES instruction feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::aes) &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> aesdeclast(simd<std::uint8_t, 16, Arch> state, simd<std::uint8_t, 16, Arch> round_key) noexcept {
    return detail::x86_aes_constant::round<true, true>(state, round_key);
  }

  /// Apply inverse MixColumns to the four state columns.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint8_t, 16, Arch> aesimc(simd<std::uint8_t, 16, Arch> state) noexcept {
    if consteval { return detail::x86_aes_constant::inverse_mix(state); }
    else {
      return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_aes::aesimc<Arch>(state.to_native()));
    }
  }

  /// Evaluate constant operands without the AES instruction feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::aes) &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> aesimc(simd<std::uint8_t, 16, Arch> state) noexcept {
    return detail::x86_aes_constant::inverse_mix(state);
  }

  /// Substitute the odd words and return their substituted and rotated forms with the round constant.
  /// Imm8 is an unsigned byte in [0,255], XORed into the rotated words.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::aes) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("aes")
  constexpr simd<std::uint8_t, 16, Arch> aeskeygenassist(simd<std::uint8_t, 16, Arch> state) noexcept {
    if consteval { return detail::x86_aes_constant::keygen<Imm8>(state); }
    else {
      return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_aes::aeskeygenassist<Arch, Imm8>(state.to_native()));
    }
  }

  /// Evaluate constant operands without the AES instruction feature.
  template<isa<x86> Arch, unsigned Imm8> requires(!Arch.has(x86_feature::aes) && Imm8 <= 255 &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> aeskeygenassist(simd<std::uint8_t, 16, Arch> state) noexcept {
    return detail::x86_aes_constant::keygen<Imm8>(state);
  }

  /// Reject mixed tags, wrong element types and implicit register conversions.
  template<isa<x86> Arch, class... Args>
  void aesenc(Args...) = delete;

  /// Reject mixed tags, wrong element types and implicit register conversions.
  template<isa<x86> Arch, class... Args>
  void aesenclast(Args...) = delete;

  /// Reject mixed tags, wrong element types and implicit register conversions.
  template<isa<x86> Arch, class... Args>
  void aesdec(Args...) = delete;

  /// Reject mixed tags, wrong element types and implicit register conversions.
  template<isa<x86> Arch, class... Args>
  void aesdeclast(Args...) = delete;

  /// Reject mixed tags, wrong element types and implicit register conversions.
  template<isa<x86> Arch, class... Args>
  void aesimc(Args...) = delete;

  /// Reject mixed tags, wrong element types and implicit register conversions.
  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void aeskeygenassist(Args...) = delete;

/// \}
}
#endif
