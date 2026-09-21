// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included by native.arm.fcma after native.simd.
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
  /// \defgroup arm_fcma Complex arithmetic
  /// Interleaved real/imaginary pairs; requires arm_feature::complxnum (FEAT_FCMA).
  /// Half arithmetic additionally requires arm_feature::neon_fp16.
  /// FCMLA is one partial complex product per call, not a full complex multiply.
  /// FPCR/FPSR are observed/affected as specified by the instruction, never reset.
  /// Volatile assembly preserves status effects even when the result is unused;
  /// the compiler barrier keeps surrounding floating-environment accesses ordered.
  /// \{

  // All vector operands share Arch; native registers remain implementation details.
  /// FCADD on 1 binary32 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 2, Arch> fcadd(simd<float, 2, Arch> a, simd<float, 2, Arch> b) noexcept {
    auto result = detail::fcadd<Arch, Rotation>(
      vget_low_f32(a.to_native()),
      vget_low_f32(b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
  }

  /// FCMLA on 1 binary32 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 2, Arch> fcmla(
      simd<float, 2, Arch> acc,
      simd<float, 2, Arch> a,
      simd<float, 2, Arch> b) noexcept {
    auto result = detail::fcmla<Arch, Rotation>(
      vget_low_f32(acc.to_native()),
      vget_low_f32(a.to_native()),
      vget_low_f32(b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 1)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 2, Arch> fcmla_lane(
      simd<float, 2, Arch> acc,
      simd<float, 2, Arch> a,
      simd<float, 2, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      vget_low_f32(acc.to_native()),
      vget_low_f32(a.to_native()),
      vget_low_f32(b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 2, Arch> fcmla_lane(
      simd<float, 2, Arch> acc,
      simd<float, 2, Arch> a,
      simd<float, 4, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      vget_low_f32(acc.to_native()),
      vget_low_f32(a.to_native()),
      __builtin_bit_cast(float32x4_t, b.to_native()));
    return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
  }

  /// FCADD on 2 binary32 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 4, Arch> fcadd(simd<float, 4, Arch> a, simd<float, 4, Arch> b) noexcept {
    auto result = detail::fcadd<Arch, Rotation>(
      __builtin_bit_cast(float32x4_t, a.to_native()),
      __builtin_bit_cast(float32x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FCMLA on 2 binary32 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 4, Arch> fcmla(
      simd<float, 4, Arch> acc,
      simd<float, 4, Arch> a,
      simd<float, 4, Arch> b) noexcept {
    auto result = detail::fcmla<Arch, Rotation>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float32x4_t, a.to_native()),
      __builtin_bit_cast(float32x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 1)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 4, Arch> fcmla_lane(
      simd<float, 4, Arch> acc,
      simd<float, 4, Arch> a,
      simd<float, 2, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float32x4_t, a.to_native()),
      vget_low_f32(b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<float, 4, Arch> fcmla_lane(
      simd<float, 4, Arch> acc,
      simd<float, 4, Arch> a,
      simd<float, 4, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      __builtin_bit_cast(float32x4_t, acc.to_native()),
      __builtin_bit_cast(float32x4_t, a.to_native()),
      __builtin_bit_cast(float32x4_t, b.to_native()));
    return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
  }

  /// FCADD on 1 binary64 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<double, 2, Arch> fcadd(simd<double, 2, Arch> a, simd<double, 2, Arch> b) noexcept {
    auto result = detail::fcadd<Arch, Rotation>(
      __builtin_bit_cast(float64x2_t, a.to_native()),
      __builtin_bit_cast(float64x2_t, b.to_native()));
    return simd<double, 2, Arch>::from_native(__builtin_bit_cast(typename simd<double, 2, Arch>::native_type, result));
  }

  /// FCMLA on 1 binary64 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  simd<double, 2, Arch> fcmla(
      simd<double, 2, Arch> acc,
      simd<double, 2, Arch> a,
      simd<double, 2, Arch> b) noexcept {
    auto result = detail::fcmla<Arch, Rotation>(
      __builtin_bit_cast(float64x2_t, acc.to_native()),
      __builtin_bit_cast(float64x2_t, a.to_native()),
      __builtin_bit_cast(float64x2_t, b.to_native()));
    return simd<double, 2, Arch>::from_native(__builtin_bit_cast(typename simd<double, 2, Arch>::native_type, result));
  }

  /// FCADD on 2 binary16 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 4, Arch> fcadd(simd<fp16, 4, Arch> a, simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fcadd<Arch, Rotation>(
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<fp16, 4, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 4, Arch>::native_type, result));
  }

  /// FCMLA on 2 binary16 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 4, Arch> fcmla(
      simd<fp16, 4, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fcmla<Arch, Rotation>(
      __builtin_bit_cast(float16x4_t, acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<fp16, 4, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 4, Arch>::native_type, result));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 4, Arch> fcmla_lane(
      simd<fp16, 4, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      __builtin_bit_cast(float16x4_t, acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<fp16, 4, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 4, Arch>::native_type, result));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 4)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 4, Arch> fcmla_lane(
      simd<fp16, 4, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      __builtin_bit_cast(float16x4_t, acc.to_native()),
      __builtin_bit_cast(float16x4_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<fp16, 4, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 4, Arch>::native_type, result));
  }

  /// FCADD on 4 binary16 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 8, Arch> fcadd(simd<fp16, 8, Arch> a, simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fcadd<Arch, Rotation>(
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<fp16, 8, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 8, Arch>::native_type, result));
  }

  /// FCMLA on 4 binary16 complex pair(s), rotation in degrees.
  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 8, Arch> fcmla(
      simd<fp16, 8, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fcmla<Arch, Rotation>(
      __builtin_bit_cast(float16x8_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<fp16, 8, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 8, Arch>::native_type, result));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 8, Arch> fcmla_lane(
      simd<fp16, 8, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      __builtin_bit_cast(float16x8_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x4_t, b.to_native()));
    return simd<fp16, 8, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 8, Arch>::native_type, result));
  }

  /// FCMLA using complex pair Lane of b (the lane indexes pairs, not scalars).
  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 4)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  simd<fp16, 8, Arch> fcmla_lane(
      simd<fp16, 8, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    auto result = detail::fcmla_lane<Arch, Rotation, Lane>(
      __builtin_bit_cast(float16x8_t, acc.to_native()),
      __builtin_bit_cast(float16x8_t, a.to_native()),
      __builtin_bit_cast(float16x8_t, b.to_native()));
    return simd<fp16, 8, Arch>::from_native(__builtin_bit_cast(typename simd<fp16, 8, Arch>::native_type, result));
  }

  /// \cond
  template<isa Arch, unsigned Rotation, class A, class B>
  void fcadd(A, B) = delete;
  template<isa Arch, unsigned Rotation, class A, class B, class C>
  void fcmla(A, B, C) = delete;
  template<isa Arch, unsigned Rotation, unsigned Lane, class A, class B, class C>
  void fcmla_lane(A, B, C) = delete;
  /// \endcond
  /// \}
}
#endif
