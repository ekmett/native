// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included by native.arm.fp16fml after native.simd.
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
  /// \defgroup arm_fp16fml FP16 widening fused multiply-add
  /// Requires arm_feature::fp16fml (FEAT_FHM), independently of FCMA/BF16.
  /// Binary16 inputs accumulate directly into binary32, with one rounding.
  /// FPCR controls and FPSR effects are those of the instruction; neither is reset.
  /// Volatile assembly retains status effects even when the result is unused.
  /// \{

  // All vector operands share Arch; native registers remain implementation details.
  /// Add products from the low 2 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlal(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlal<Arch>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLAL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlal_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlal_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLAL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlal_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlal_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// Add products from the low 4 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlal(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlal<Arch>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLAL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlal_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlal_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLAL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlal_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlal_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// Add products from the high 2 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlal2(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlal2<Arch>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlal2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlal2_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlal2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlal2_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// Add products from the high 4 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlal2(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlal2<Arch>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlal2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlal2_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlal2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlal2_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// Subtract products from the low 2 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlsl(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlsl<Arch>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLSL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlsl_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlsl_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLSL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlsl_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlsl_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// Subtract products from the low 4 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlsl(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlsl<Arch>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLSL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlsl_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlsl_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLSL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlsl_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlsl_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// Subtract products from the high 2 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlsl2(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlsl2<Arch>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlsl2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlsl2_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 2, Arch> fmlsl2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlsl2_lane<Arch, Lane>(
      vget_low_f32(acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0)));
  }

  /// Subtract products from the high 4 half lanes of a and b.
  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlsl2(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlsl2<Arch>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlsl2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fmlsl2_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  simd<float, 4, Arch> fmlsl2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fmlsl2_lane<Arch, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// \cond
  template<isa Arch, class A, class B, class C>
  void fmlal(A, B, C) = delete;
  template<isa Arch, unsigned Lane, class A, class B, class C>
  void fmlal_lane(A, B, C) = delete;
  template<isa Arch, class A, class B, class C>
  void fmlal2(A, B, C) = delete;
  template<isa Arch, unsigned Lane, class A, class B, class C>
  void fmlal2_lane(A, B, C) = delete;
  template<isa Arch, class A, class B, class C>
  void fmlsl(A, B, C) = delete;
  template<isa Arch, unsigned Lane, class A, class B, class C>
  void fmlsl_lane(A, B, C) = delete;
  template<isa Arch, class A, class B, class C>
  void fmlsl2(A, B, C) = delete;
  template<isa Arch, unsigned Lane, class A, class B, class C>
  void fmlsl2_lane(A, B, C) = delete;
  /// \endcond
  /// \}
}
#endif
