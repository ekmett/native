// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup arm_bf16 BF16 dot products, matrices and widening multiply-add
  /// Requires arm_feature::neon_bf16 and a compatible bf16 compiler target.
  /// BFDOT/BFMMLA use round-to-odd steps by default, or fused pairs followed by
  /// separate accumulation when FEAT_EBF16 and FPCR.EBF enable enhanced behavior.
  /// They return default NaNs and leave FPSR unchanged in either mode.
  /// BFMLALB/T instead perform single fused binary32 multiply-adds. With AH=0
  /// they use ordinary FP32 controls and accumulate exception flags. With AFP
  /// and AH=1 they force RNE and input/output flushing, suppressing exceptions.
  /// All operations read the caller's FPCR without changing it.
  /// \{

  /// Accumulate each adjacent pair of BF16 products into the corresponding FP32 lane.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 2, Arch> bfdot(simd<float, 2, Arch> acc, simd<bf16, 4, Arch> a, simd<bf16, 4, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfdot<Arch>(vget_low_f32(acc.to_storage().to_native()), __builtin_bit_cast(bfloat16x4_t, a.to_native()), __builtin_bit_cast(bfloat16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_storage(
      simd<float, 4, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f))));
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 2)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 2, Arch> bfdot_lane(simd<float, 2, Arch> acc, simd<bf16, 4, Arch> a, simd<bf16, 4, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfdot_lane<Arch, Lane>(vget_low_f32(acc.to_storage().to_native()), __builtin_bit_cast(bfloat16x4_t, a.to_native()), __builtin_bit_cast(bfloat16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_storage(
      simd<float, 4, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f))));
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 2, Arch> bfdot_lane(simd<float, 2, Arch> acc, simd<bf16, 4, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfdot_lane<Arch, Lane>(vget_low_f32(acc.to_storage().to_native()), __builtin_bit_cast(bfloat16x4_t, a.to_native()), b.to_native());
    return simd<float, 2, Arch>::from_storage(
      simd<float, 4, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f))));
  }

  /// Accumulate each adjacent pair of BF16 products into the corresponding FP32 lane.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfdot(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfdot<Arch>(acc.to_native(), a.to_native(), b.to_native());
    return simd<float, 4, Arch>::from_native(result);
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 2)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfdot_lane(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 4, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfdot_lane<Arch, Lane>(acc.to_native(), a.to_native(), __builtin_bit_cast(bfloat16x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(result);
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfdot_lane(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfdot_lane<Arch, Lane>(acc.to_native(), a.to_native(), b.to_native());
    return simd<float, 4, Arch>::from_native(result);
  }

  /// Accumulate a row-major 2x4 matrix times a column-major 4x2 matrix, two BFDOT steps per result.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfmmla(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfmmla<Arch>(acc.to_native(), a.to_native(), b.to_native());
    return simd<float, 4, Arch>::from_native(result);
  }

  /// Fused multiply-add of the even BF16 lanes into the corresponding FP32 lanes.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfmlalb(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfmlalb<Arch>(acc.to_native(), a.to_native(), b.to_native());
    return simd<float, 4, Arch>::from_native(result);
  }

  /// Fused multiply-add of the even lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfmlalb_lane(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 4, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfmlalb_lane<Arch, Lane>(acc.to_native(), a.to_native(), __builtin_bit_cast(bfloat16x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(result);
  }

  /// Fused multiply-add of the even lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfmlalb_lane(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfmlalb_lane<Arch, Lane>(acc.to_native(), a.to_native(), b.to_native());
    return simd<float, 4, Arch>::from_native(result);
  }

  /// Fused multiply-add of the odd BF16 lanes into the corresponding FP32 lanes.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfmlalt(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfmlalt<Arch>(acc.to_native(), a.to_native(), b.to_native());
    return simd<float, 4, Arch>::from_native(result);
  }

  /// Fused multiply-add of the odd lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfmlalt_lane(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 4, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfmlalt_lane<Arch, Lane>(acc.to_native(), a.to_native(), __builtin_bit_cast(bfloat16x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(result);
  }

  /// Fused multiply-add of the odd lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("bf16")))
  simd<float, 4, Arch> bfmlalt_lane(simd<float, 4, Arch> acc, simd<bf16, 8, Arch> a, simd<bf16, 8, Arch> b) noexcept {
    auto result = detail::arm_bf16::bfmlalt_lane<Arch, Lane>(acc.to_native(), a.to_native(), b.to_native());
    return simd<float, 4, Arch>::from_native(result);
  }

  // Exact deduction rejects unrelated vectors and invalid immediates before
  // Clang's lax vector conversions can select an overload for another shape.
  /// \cond
  template<isa Arch, class R, class A, class B> void bfdot(R, A, B) = delete;
  template<isa Arch, class R, class A, class B> void bfmmla(R, A, B) = delete;
  template<isa Arch, class R, class A, class B> void bfmlalb(R, A, B) = delete;
  template<isa Arch, class R, class A, class B> void bfmlalt(R, A, B) = delete;
  template<isa Arch, unsigned Lane, class R, class A, class B> void bfdot_lane(R, A, B) = delete;
  template<isa Arch, unsigned Lane, class R, class A, class B> void bfmlalb_lane(R, A, B) = delete;
  template<isa Arch, unsigned Lane, class R, class A, class B> void bfmlalt_lane(R, A, B) = delete;

  /// \endcond
  /// \}
}
#endif
