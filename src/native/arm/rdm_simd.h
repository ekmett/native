// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included by native.arm.rdm after native.simd.
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup arm_rdm ARM RDM
 * Explicit ISA-gated AArch64 instructions. Admit the feature before entering
 * a matching target scope; runtime calls provide no software fallback.
 * Constant operands use exact integer semantics; feature-absent overloads are
 * immediate-only and preserve the available SIMD storage shape.
 * SQRDMLAH and SQRDMLSH round and saturate once after accumulation.
 * Exact inline instructions avoid Clang's broader v8.1a builtin requirement.
 * Scalar calls without an ISA argument use the owning module's baseline.
 * Runtime saturation can set sticky FPSR.QC; no const/pure promise is made.
 * Constant evaluation computes values only and cannot observe or modify FPSR.
 * \{ */
  // All vector operands share Arch; native registers remain implementation details.
  /// SQRDMLAH: signed 16-bit rounding saturating add.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int16_t sqrdmlah(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah<Arch>(
        accumulator,
        lhs,
        rhs);
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::rdm))
  native_nodiscard consteval int16_t sqrdmlah(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        accumulator,
        lhs,
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        accumulator,
        lhs,
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8 && requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH: signed 16-bit rounding saturating add.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 4, Arch> sqrdmlah(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah<Arch>(
        __builtin_bit_cast(int16x4_t, accumulator.to_native()),
        __builtin_bit_cast(int16x4_t, lhs.to_native()),
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 4, Arch> sqrdmlah(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 4, Arch> sqrdmlah_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        __builtin_bit_cast(int16x4_t, accumulator.to_native()),
        __builtin_bit_cast(int16x4_t, lhs.to_native()),
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 4, Arch> sqrdmlah_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 4, Arch> sqrdmlah_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        __builtin_bit_cast(int16x4_t, accumulator.to_native()),
        __builtin_bit_cast(int16x4_t, lhs.to_native()),
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8 && requires { sizeof(simd<std::int16_t, 4, Arch>); sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 4, Arch> sqrdmlah_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH: signed 16-bit rounding saturating add.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 8, Arch> sqrdmlah(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah<Arch>(
        __builtin_bit_cast(int16x8_t, accumulator.to_native()),
        __builtin_bit_cast(int16x8_t, lhs.to_native()),
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> sqrdmlah(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 8, Arch> sqrdmlah_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        __builtin_bit_cast(int16x8_t, accumulator.to_native()),
        __builtin_bit_cast(int16x8_t, lhs.to_native()),
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int16_t, 8, Arch>); sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> sqrdmlah_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 8, Arch> sqrdmlah_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        __builtin_bit_cast(int16x8_t, accumulator.to_native()),
        __builtin_bit_cast(int16x8_t, lhs.to_native()),
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8 && requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> sqrdmlah_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH: signed 32-bit rounding saturating add.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int32_t sqrdmlah(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah<Arch>(
        accumulator,
        lhs,
        rhs);
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::rdm))
  native_nodiscard consteval int32_t sqrdmlah(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        accumulator,
        lhs,
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        accumulator,
        lhs,
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH: signed 32-bit rounding saturating add.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 2, Arch> sqrdmlah(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah<Arch>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sqrdmlah(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 2, Arch> sqrdmlah_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sqrdmlah_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 2, Arch> sqrdmlah_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 2, Arch>); sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sqrdmlah_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH: signed 32-bit rounding saturating add.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 4, Arch> sqrdmlah(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah<Arch>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int32x4_t, lhs.to_native()),
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sqrdmlah(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 4, Arch> sqrdmlah_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int32x4_t, lhs.to_native()),
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 4, Arch>); sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sqrdmlah_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLAH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 4, Arch> sqrdmlah_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlah_lane<Arch, Lane>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int32x4_t, lhs.to_native()),
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sqrdmlah_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<false, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH: signed 16-bit rounding saturating subtract.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int16_t sqrdmlsh(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh<Arch>(
        accumulator,
        lhs,
        rhs);
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::rdm))
  native_nodiscard consteval int16_t sqrdmlsh(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        accumulator,
        lhs,
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        accumulator,
        lhs,
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8 && requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH: signed 16-bit rounding saturating subtract.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 4, Arch> sqrdmlsh(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh<Arch>(
        __builtin_bit_cast(int16x4_t, accumulator.to_native()),
        __builtin_bit_cast(int16x4_t, lhs.to_native()),
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 4, Arch> sqrdmlsh(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        __builtin_bit_cast(int16x4_t, accumulator.to_native()),
        __builtin_bit_cast(int16x4_t, lhs.to_native()),
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        __builtin_bit_cast(int16x4_t, accumulator.to_native()),
        __builtin_bit_cast(int16x4_t, lhs.to_native()),
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return simd<std::int16_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8 && requires { sizeof(simd<std::int16_t, 4, Arch>); sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 4, Arch> accumulator,
      simd<std::int16_t, 4, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH: signed 16-bit rounding saturating subtract.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 8, Arch> sqrdmlsh(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh<Arch>(
        __builtin_bit_cast(int16x8_t, accumulator.to_native()),
        __builtin_bit_cast(int16x8_t, lhs.to_native()),
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> sqrdmlsh(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 8, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        __builtin_bit_cast(int16x8_t, accumulator.to_native()),
        __builtin_bit_cast(int16x8_t, lhs.to_native()),
        __builtin_bit_cast(int16x4_t, rhs.to_native()));
      return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int16_t, 8, Arch>); sizeof(simd<std::int16_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int16_t, 8, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        __builtin_bit_cast(int16x8_t, accumulator.to_native()),
        __builtin_bit_cast(int16x8_t, lhs.to_native()),
        __builtin_bit_cast(int16x8_t, rhs.to_native()));
      return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8 && requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> sqrdmlsh_lane(
      simd<std::int16_t, 8, Arch> accumulator,
      simd<std::int16_t, 8, Arch> lhs,
      simd<std::int16_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH: signed 32-bit rounding saturating subtract.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int32_t sqrdmlsh(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh<Arch>(
        accumulator,
        lhs,
        rhs);
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::rdm))
  native_nodiscard consteval int32_t sqrdmlsh(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        accumulator,
        lhs,
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        accumulator,
        lhs,
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return result;
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH: signed 32-bit rounding saturating subtract.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 2, Arch> sqrdmlsh(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh<Arch>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sqrdmlsh(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 2, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 2, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        vget_low_s32(__builtin_bit_cast(int32x4_t, lhs.to_native())),
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 2, Arch>); sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int32_t, 2, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH: signed 32-bit rounding saturating subtract.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 4, Arch> sqrdmlsh(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh<Arch>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int32x4_t, lhs.to_native()),
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::rdm) && requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sqrdmlsh(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, -1>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int32x4_t, lhs.to_native()),
        vget_low_s32(__builtin_bit_cast(int32x4_t, rhs.to_native())));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 4, Arch>); sizeof(simd<std::int32_t, 2, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 2, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// SQRDMLSH by element: broadcast rhs[Lane] before rounding and saturation.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  constexpr simd<std::int32_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sqrdmlsh_lane<Arch, Lane>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int32x4_t, lhs.to_native()),
        __builtin_bit_cast(int32x4_t, rhs.to_native()));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sqrdmlsh_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int32_t, 4, Arch> lhs,
      simd<std::int32_t, 4, Arch> rhs) noexcept {
    return detail::arm_constant::rdm<true, Lane>(accumulator, lhs, rhs);
  }

  /// \cond
  template<isa<arm> Arch=NATIVE_BASELINE, class A, class B, class C>
  void sqrdmlah(A, B, C) = delete;
  template<isa<arm> Arch, int Lane, class A, class B, class C>
  void sqrdmlah_lane(A, B, C) = delete;
  template<isa<arm> Arch=NATIVE_BASELINE, class A, class B, class C>
  void sqrdmlsh(A, B, C) = delete;
  template<isa<arm> Arch, int Lane, class A, class B, class C>
  void sqrdmlsh_lane(A, B, C) = delete;
  /// \endcond
  /// \}
}
#endif
