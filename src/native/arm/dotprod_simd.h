// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included by native.arm.dotprod after native.simd.
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup arm_dotprod ARM DOTPROD
 * Explicit ISA-gated AArch64 instructions. Admit the feature before entering
 * a matching target scope; these operations provide no software fallback.
 * SDOT and UDOT accumulate modulo 2^32, including signed overflow.
 * \{ */
  // All vector operands share Arch; native registers remain implementation details.
  /// SDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::int32_t, 2, Arch> sdot(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    auto result = detail::sdot<Arch>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      __builtin_bit_cast(int8x8_t, lhs.to_native()),
      __builtin_bit_cast(int8x8_t, rhs.to_native()));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::int32_t, 2, Arch> sdot_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    auto result = detail::sdot_lane<Arch, Lane>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      __builtin_bit_cast(int8x8_t, lhs.to_native()),
      __builtin_bit_cast(int8x8_t, rhs.to_native()));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::int32_t, 2, Arch> sdot_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    auto result = detail::sdot_lane<Arch, Lane>(
      vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
      __builtin_bit_cast(int8x8_t, lhs.to_native()),
      __builtin_bit_cast(int8x16_t, rhs.to_native()));
    return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
  }

  /// SDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::int32_t, 4, Arch> sdot(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    auto result = detail::sdot<Arch>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int8x16_t, lhs.to_native()),
      __builtin_bit_cast(int8x16_t, rhs.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::int32_t, 4, Arch> sdot_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    auto result = detail::sdot_lane<Arch, Lane>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int8x16_t, lhs.to_native()),
      __builtin_bit_cast(int8x8_t, rhs.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::int32_t, 4, Arch> sdot_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    auto result = detail::sdot_lane<Arch, Lane>(
      __builtin_bit_cast(int32x4_t, accumulator.to_native()),
      __builtin_bit_cast(int8x16_t, lhs.to_native()),
      __builtin_bit_cast(int8x16_t, rhs.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// UDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::uint32_t, 2, Arch> udot(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    auto result = detail::udot<Arch>(
      vget_low_u32(__builtin_bit_cast(uint32x4_t, accumulator.to_native())),
      __builtin_bit_cast(uint8x8_t, lhs.to_native()),
      __builtin_bit_cast(uint8x8_t, rhs.to_native()));
    return simd<std::uint32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 2, Arch>::native_type, vcombine_u32(result, vdup_n_u32(0))));
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::uint32_t, 2, Arch> udot_lane(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    auto result = detail::udot_lane<Arch, Lane>(
      vget_low_u32(__builtin_bit_cast(uint32x4_t, accumulator.to_native())),
      __builtin_bit_cast(uint8x8_t, lhs.to_native()),
      __builtin_bit_cast(uint8x8_t, rhs.to_native()));
    return simd<std::uint32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 2, Arch>::native_type, vcombine_u32(result, vdup_n_u32(0))));
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::uint32_t, 2, Arch> udot_lane(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    auto result = detail::udot_lane<Arch, Lane>(
      vget_low_u32(__builtin_bit_cast(uint32x4_t, accumulator.to_native())),
      __builtin_bit_cast(uint8x8_t, lhs.to_native()),
      __builtin_bit_cast(uint8x16_t, rhs.to_native()));
    return simd<std::uint32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 2, Arch>::native_type, vcombine_u32(result, vdup_n_u32(0))));
  }

  /// UDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::uint32_t, 4, Arch> udot(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    auto result = detail::udot<Arch>(
      __builtin_bit_cast(uint32x4_t, accumulator.to_native()),
      __builtin_bit_cast(uint8x16_t, lhs.to_native()),
      __builtin_bit_cast(uint8x16_t, rhs.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::uint32_t, 4, Arch> udot_lane(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    auto result = detail::udot_lane<Arch, Lane>(
      __builtin_bit_cast(uint32x4_t, accumulator.to_native()),
      __builtin_bit_cast(uint8x16_t, lhs.to_native()),
      __builtin_bit_cast(uint8x8_t, rhs.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  simd<std::uint32_t, 4, Arch> udot_lane(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    auto result = detail::udot_lane<Arch, Lane>(
      __builtin_bit_cast(uint32x4_t, accumulator.to_native()),
      __builtin_bit_cast(uint8x16_t, lhs.to_native()),
      __builtin_bit_cast(uint8x16_t, rhs.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// \cond
  template<isa Arch, class A, class B, class C>
  void sdot(A, B, C) = delete;
  template<isa Arch, int Lane, class A, class B, class C>
  void sdot_lane(A, B, C) = delete;
  template<isa Arch, class A, class B, class C>
  void udot(A, B, C) = delete;
  template<isa Arch, int Lane, class A, class B, class C>
  void udot_lane(A, B, C) = delete;
  /// \endcond
  /// \}
}
#endif
