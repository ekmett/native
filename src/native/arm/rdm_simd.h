// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included by native.arm.rdm after native.simd.
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup arm_rdm ARM RDM
 * Explicit ISA-gated AArch64 instructions. Admit the feature before entering
 * a matching target scope; these operations provide no software fallback.
 * SQRDMLAH and SQRDMLSH round and saturate once after accumulation.
 * Exact inline instructions avoid Clang's broader v8.1a builtin requirement.
 * Scalar calls without an ISA argument use the owning module's baseline.
 * Saturation can set the sticky FPSR.QC flag; no const/pure promise is made.
 * \{ */
  // All vector operands share Arch; native registers remain implementation details.
  /// SQRDMLAH: signed 16-bit rounding saturating add.
  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlah(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    auto result = detail::sqrdmlah<Arch>(
      accumulator,
      lhs,
      rhs);
    return result;
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      accumulator,
      lhs,
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return result;
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      accumulator,
      lhs,
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return result;
  }

  /// SQRDMLAH: signed 16-bit rounding saturating add.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 4, Arch> sqrdmlah(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah<Arch>(
      __builtin_bit_cast(int16x4_t, accumulator.to_native()),
      __builtin_bit_cast(int16x4_t, lhs.to_native()),
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 4, Arch> sqrdmlah_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      __builtin_bit_cast(int16x4_t, accumulator.to_native()),
      __builtin_bit_cast(int16x4_t, lhs.to_native()),
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 4, Arch> sqrdmlah_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      __builtin_bit_cast(int16x4_t, accumulator.to_native()),
      __builtin_bit_cast(int16x4_t, lhs.to_native()),
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLAH: signed 16-bit rounding saturating add.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 8, Arch> sqrdmlah(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah<Arch>(
      __builtin_bit_cast(int16x8_t, accumulator.to_native()),
      __builtin_bit_cast(int16x8_t, lhs.to_native()),
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 8, Arch> sqrdmlah_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      __builtin_bit_cast(int16x8_t, accumulator.to_native()),
      __builtin_bit_cast(int16x8_t, lhs.to_native()),
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 8, Arch> sqrdmlah_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      __builtin_bit_cast(int16x8_t, accumulator.to_native()),
      __builtin_bit_cast(int16x8_t, lhs.to_native()),
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// SQRDMLAH: signed 32-bit rounding saturating add.
  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlah(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    auto result = detail::sqrdmlah<Arch>(
      accumulator,
      lhs,
      rhs);
    return result;
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      accumulator,
      lhs,
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return result;
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      accumulator,
      lhs,
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return result;
  }

  /// SQRDMLAH: signed 32-bit rounding saturating add.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 2, Arch> sqrdmlah(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah<Arch>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 2, Arch> sqrdmlah_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 2, Arch> sqrdmlah_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SQRDMLAH: signed 32-bit rounding saturating add.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 4, Arch> sqrdmlah(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah<Arch>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int32x4_t, lhs.to_native()),
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 4, Arch> sqrdmlah_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int32x4_t, lhs.to_native()),
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 4, Arch> sqrdmlah_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlah_lane<Arch, Lane>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int32x4_t, lhs.to_native()),
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLSH: signed 16-bit rounding saturating subtract.
  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlsh(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    auto result = detail::sqrdmlsh<Arch>(
      accumulator,
      lhs,
      rhs);
    return result;
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      accumulator,
      lhs,
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return result;
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      accumulator,
      lhs,
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return result;
  }

  /// SQRDMLSH: signed 16-bit rounding saturating subtract.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 4, Arch> sqrdmlsh(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh<Arch>(
      __builtin_bit_cast(int16x4_t, accumulator.to_native()),
      __builtin_bit_cast(int16x4_t, lhs.to_native()),
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      __builtin_bit_cast(int16x4_t, accumulator.to_native()),
      __builtin_bit_cast(int16x4_t, lhs.to_native()),
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      __builtin_bit_cast(int16x4_t, accumulator.to_native()),
      __builtin_bit_cast(int16x4_t, lhs.to_native()),
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLSH: signed 16-bit rounding saturating subtract.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 8, Arch> sqrdmlsh(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh<Arch>(
      __builtin_bit_cast(int16x8_t, accumulator.to_native()),
      __builtin_bit_cast(int16x8_t, lhs.to_native()),
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 8, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      __builtin_bit_cast(int16x8_t, accumulator.to_native()),
      __builtin_bit_cast(int16x8_t, lhs.to_native()),
      __builtin_bit_cast(int16x4_t, rhs.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int16_t, 8, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      __builtin_bit_cast(int16x8_t, accumulator.to_native()),
      __builtin_bit_cast(int16x8_t, lhs.to_native()),
      __builtin_bit_cast(int16x8_t, rhs.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// SQRDMLSH: signed 32-bit rounding saturating subtract.
  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlsh(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    auto result = detail::sqrdmlsh<Arch>(
      accumulator,
      lhs,
      rhs);
    return result;
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      accumulator,
      lhs,
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return result;
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      accumulator,
      lhs,
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return result;
  }

  /// SQRDMLSH: signed 32-bit rounding saturating subtract.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 2, Arch> sqrdmlsh(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh<Arch>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 2, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 2, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SQRDMLSH: signed 32-bit rounding saturating subtract.
  template<isa Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 4, Arch> sqrdmlsh(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh<Arch>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int32x4_t, lhs.to_native()),
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int32x4_t, lhs.to_native()),
      vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  simd<std::int32_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    auto result = detail::sqrdmlsh_lane<Arch, Lane>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int32x4_t, lhs.to_native()),
      __builtin_bit_cast(int32x4_t, rhs.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// \cond
  template<isa Arch=NATIVE_BASELINE, class A, class B, class C>
  void sqrdmlah(A, B, C) = delete;
  template<isa Arch, int Lane, class A, class B, class C>
  void sqrdmlah_lane(A, B, C) = delete;
  template<isa Arch=NATIVE_BASELINE, class A, class B, class C>
  void sqrdmlsh(A, B, C) = delete;
  template<isa Arch, int Lane, class A, class B, class C>
  void sqrdmlsh_lane(A, B, C) = delete;
  /// \endcond
  /// \}
}
#endif
