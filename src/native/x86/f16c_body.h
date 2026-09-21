// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
/** \defgroup x86_f16c F16C
 * Binary32 / IEEE binary16 conversions using VEX VCVTPS2PH and VCVTPH2PS.
 * Arch must contain f16c; its compiler prerequisite is AVX. Callers must enable
 * a matching target and admit CPU support and XMM/YMM OS state separately.
 * No AVX2 or AVX512FP16 instructions are required.
 *
 * Packed operands and results use simd<float,N,Arch> and simd<fp16,N,Arch>.
 * Half vectors preserve representation bits; no scalar numerical conversion
 * is used at the vector boundary. Scalar forms use float and uint16_t half bits.
 *
 * Imm8 accepts every byte, 0..255. Bit 2 selects MXCSR.RC; otherwise bits 1:0
 * select nearest-even (0), down (1), up (2), or toward zero (3). Bits 7:3 are
 * ignored by the instruction: in particular, bit 3 does NOT suppress exceptions.
 * Narrowing ignores FTZ and honors DAZ for binary32 subnormal inputs. Widening
 * ignores DAZ and does not raise a denormal exception for binary16 subnormals.
 * Signs of zero and infinity are preserved. NaNs retain their sign and high
 * payload bits and are quieted; signaling NaNs raise invalid.
 *
 * These operations retain architectural MXCSR status updates and unmasked
 * exceptions even if their result is discarded. They do not modify control
 * bits or masks. They are neither const nor pure, and are not constexpr.
 * \{ */

  /// Convert 4 binary32 lanes to 4 binary16 lanes.
  template<isa Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::f16c) && Imm8 <= 255)
  native_nodiscard native_inline native_target("f16c")
  simd<fp16, 4, Arch> cvtps_ph(simd<float, 4, Arch> a) noexcept {
    // VCVTPS2PH zeroes the unused high half of its destination.
    return simd<fp16, 4, Arch>::from_native(
      detail::x86_f16c::cvtps_ph<Arch, Imm8>(a.to_native()));
  }

  /// Convert 8 binary32 lanes to 8 binary16 lanes.
  template<isa Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::f16c) && Imm8 <= 255)
  native_nodiscard native_inline native_target("f16c")
  simd<fp16, 8, Arch> cvtps_ph(simd<float, 8, Arch> a) noexcept {
    return simd<fp16, 8, Arch>::from_native(
      detail::x86_f16c::cvtps_ph<Arch, Imm8>(a.to_native()));
  }

  /// Widen 4 binary16 lanes to 4 binary32 lanes.
  template<isa Arch, unsigned Lanes>
    requires(Arch.has(x86_feature::f16c) && Lanes == 4)
  native_nodiscard native_inline native_target("f16c")
  simd<float, 4, Arch> cvtph_ps(simd<fp16, 4, Arch> a) noexcept {
    return simd<float, 4, Arch>::from_native(
      detail::x86_f16c::cvtph_ps<Arch, 4>(a.to_native()));
  }

  /// Widen 8 binary16 lanes to 8 binary32 lanes.
  template<isa Arch, unsigned Lanes>
    requires(Arch.has(x86_feature::f16c) && Lanes == 8)
  native_nodiscard native_inline native_target("f16c")
  simd<float, 8, Arch> cvtph_ps(simd<fp16, 8, Arch> a) noexcept {
    return simd<float, 8, Arch>::from_native(
      detail::x86_f16c::cvtph_ps<Arch, 8>(a.to_native()));
  }

  /// Convert one binary32 value to binary16 representation bits.
  template<isa Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::f16c) && Imm8 <= 255)
  native_nodiscard native_inline native_target("f16c")
  std::uint16_t cvtss_sh(float a) noexcept {
    return detail::x86_f16c::cvtss_sh<Arch, Imm8>(a);
  }

  /// Widen one binary16 representation to binary32.
  template<isa Arch> requires(Arch.has(x86_feature::f16c))
  native_nodiscard native_inline native_target("f16c")
  float cvtsh_ss(std::uint16_t a) noexcept {
    return detail::x86_f16c::cvtsh_ss<Arch>(a);
  }

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa Arch, unsigned Imm8, class V> void cvtps_ph(V) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa Arch, unsigned Lanes, class V> void cvtph_ps(V) = delete;
  /// \}
}
#endif
