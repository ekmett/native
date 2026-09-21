// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
/** \defgroup x86_vpclmul VPCLMULQDQ
 * Exact carry-less 64-by-64 multiplication within each 128-bit lane.
 * Imm8 bit 0 selects a's half and bit 4 selects b's half in every lane.
 * Other bits are ignored; the immediate must be in [0,255]. Each product
 * occupies its original 128-bit lane, with bit 127 zero. There are no
 * cross-lane products, carries, or polynomial reduction.
 * The 128-bit intrinsic needs PCLMUL and AVX, not the VPCLMULQDQ feature.
 * The 256-bit intrinsic needs VPCLMULQDQ and AVX, without AVX2 or AVX512VL.
 * The 512-bit intrinsic also needs AVX512F, without AVX512BW/DQ/VL.
 * Arch records requirements; callers separately enable and admit the target.
 * All forms are pure integer computations with no floating-point effects.
 * \{ */

  /// Multiply selected halves of one 128-bit lane using PCLMUL and AVX.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::pclmul) &&
      Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,pclmul")
  simd<std::uint64_t, 2, Arch> vpclmulqdq(simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_vpclmul::vpclmulqdq<Arch, Imm8>(a.to_native(), b.to_native()));
  }

  /// Multiply selected halves independently in two 128-bit lanes; AVX suffices.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::vpclmulqdq) &&
      Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,vpclmulqdq")
  simd<std::uint64_t, 4, Arch> vpclmulqdq(simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_vpclmul::vpclmulqdq<Arch, Imm8>(a.to_native(), b.to_native()));
  }

  /// Multiply selected halves independently in four 128-bit lanes; needs AVX512F.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::vpclmulqdq) &&
      Arch.has(x86_feature::avx512f) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,vpclmulqdq")
  simd<std::uint64_t, 8, Arch> vpclmulqdq(simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_vpclmul::vpclmulqdq<Arch, Imm8>(a.to_native(), b.to_native()));
  }

  // Reject implicit register conversions, mixed tags and wrong element types.
  template<isa Arch, unsigned Imm8, class... Args>
  void vpclmulqdq(Args...) = delete;

/// \}
}
#endif
