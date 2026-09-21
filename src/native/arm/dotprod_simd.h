// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included by native.arm.dotprod after native.simd.
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup arm_dotprod ARM DOTPROD
 * Explicit ISA-gated AArch64 instructions. Admit the feature before entering
 * a matching target scope; runtime calls provide no software fallback.
 * Constant operands use exact integer semantics; feature-absent overloads are
 * immediate-only and preserve the available SIMD storage shape.
 * SDOT and UDOT accumulate modulo 2^32, including signed overflow.
 * \{ */
  // All vector operands share Arch; native registers remain implementation details.
  /// SDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::int32_t, 2, Arch> sdot(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sdot<Arch>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        __builtin_bit_cast(int8x8_t, lhs.to_native()),
        __builtin_bit_cast(int8x8_t, rhs.to_native()));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::dotprod) && requires { sizeof(simd<std::int32_t, 2, Arch>); sizeof(simd<std::int8_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sdot(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::int32_t, 2, Arch> sdot_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sdot_lane<Arch, Lane>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        __builtin_bit_cast(int8x8_t, lhs.to_native()),
        __builtin_bit_cast(int8x8_t, rhs.to_native()));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 2, Arch>); sizeof(simd<std::int8_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sdot_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::int32_t, 2, Arch> sdot_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sdot_lane<Arch, Lane>(
        vget_low_s32(__builtin_bit_cast(int32x4_t, accumulator.to_native())),
        __builtin_bit_cast(int8x8_t, lhs.to_native()),
        __builtin_bit_cast(int8x16_t, rhs.to_native()));
      return simd<std::int32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 2, Arch>::native_type, vcombine_s32(result, vdup_n_s32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 2, Arch>); sizeof(simd<std::int8_t, 8, Arch>); sizeof(simd<std::int8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 2, Arch> sdot_lane(
      simd<std::int32_t, 2, Arch> accumulator,
      simd<std::int8_t, 8, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// SDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::int32_t, 4, Arch> sdot(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sdot<Arch>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int8x16_t, lhs.to_native()),
        __builtin_bit_cast(int8x16_t, rhs.to_native()));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::dotprod) && requires { sizeof(simd<std::int32_t, 4, Arch>); sizeof(simd<std::int8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sdot(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::int32_t, 4, Arch> sdot_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sdot_lane<Arch, Lane>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int8x16_t, lhs.to_native()),
        __builtin_bit_cast(int8x8_t, rhs.to_native()));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::int32_t, 4, Arch>); sizeof(simd<std::int8_t, 16, Arch>); sizeof(simd<std::int8_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sdot_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::int32_t, 4, Arch> sdot_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::sdot_lane<Arch, Lane>(
        __builtin_bit_cast(int32x4_t, accumulator.to_native()),
        __builtin_bit_cast(int8x16_t, lhs.to_native()),
        __builtin_bit_cast(int8x16_t, rhs.to_native()));
      return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::int32_t, 4, Arch>); sizeof(simd<std::int8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> sdot_lane(
      simd<std::int32_t, 4, Arch> accumulator,
      simd<std::int8_t, 16, Arch> lhs,
      simd<std::int8_t, 16, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// UDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::uint32_t, 2, Arch> udot(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::udot<Arch>(
        vget_low_u32(__builtin_bit_cast(uint32x4_t, accumulator.to_native())),
        __builtin_bit_cast(uint8x8_t, lhs.to_native()),
        __builtin_bit_cast(uint8x8_t, rhs.to_native()));
      return simd<std::uint32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 2, Arch>::native_type, vcombine_u32(result, vdup_n_u32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::dotprod) && requires { sizeof(simd<std::uint32_t, 2, Arch>); sizeof(simd<std::uint8_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 2, Arch> udot(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::uint32_t, 2, Arch> udot_lane(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::udot_lane<Arch, Lane>(
        vget_low_u32(__builtin_bit_cast(uint32x4_t, accumulator.to_native())),
        __builtin_bit_cast(uint8x8_t, lhs.to_native()),
        __builtin_bit_cast(uint8x8_t, rhs.to_native()));
      return simd<std::uint32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 2, Arch>::native_type, vcombine_u32(result, vdup_n_u32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::uint32_t, 2, Arch>); sizeof(simd<std::uint8_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 2, Arch> udot_lane(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::uint32_t, 2, Arch> udot_lane(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::udot_lane<Arch, Lane>(
        vget_low_u32(__builtin_bit_cast(uint32x4_t, accumulator.to_native())),
        __builtin_bit_cast(uint8x8_t, lhs.to_native()),
        __builtin_bit_cast(uint8x16_t, rhs.to_native()));
      return simd<std::uint32_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 2, Arch>::native_type, vcombine_u32(result, vdup_n_u32(0))));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::uint32_t, 2, Arch>); sizeof(simd<std::uint8_t, 8, Arch>); sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 2, Arch> udot_lane(
      simd<std::uint32_t, 2, Arch> accumulator,
      simd<std::uint8_t, 8, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// UDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::uint32_t, 4, Arch> udot(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs); }
    else {
      auto result = detail::udot<Arch>(
        __builtin_bit_cast(uint32x4_t, accumulator.to_native()),
        __builtin_bit_cast(uint8x16_t, lhs.to_native()),
        __builtin_bit_cast(uint8x16_t, rhs.to_native()));
      return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch> requires(!Arch.has(arm_feature::dotprod) && requires { sizeof(simd<std::uint32_t, 4, Arch>); sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> udot(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, -1>(accumulator, lhs, rhs);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::uint32_t, 4, Arch> udot_lane(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::udot_lane<Arch, Lane>(
        __builtin_bit_cast(uint32x4_t, accumulator.to_native()),
        __builtin_bit_cast(uint8x16_t, lhs.to_native()),
        __builtin_bit_cast(uint8x8_t, rhs.to_native()));
      return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2 && requires { sizeof(simd<std::uint32_t, 4, Arch>); sizeof(simd<std::uint8_t, 16, Arch>); sizeof(simd<std::uint8_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> udot_lane(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 8, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  constexpr simd<std::uint32_t, 4, Arch> udot_lane(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    if consteval { return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs); }
    else {
      auto result = detail::udot_lane<Arch, Lane>(
        __builtin_bit_cast(uint32x4_t, accumulator.to_native()),
        __builtin_bit_cast(uint8x16_t, lhs.to_native()),
        __builtin_bit_cast(uint8x16_t, rhs.to_native()));
      return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
    }
  }

  /// Evaluate constant operands without the instruction feature.
  template<isa<arm> Arch, int Lane> requires(!Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4 && requires { sizeof(simd<std::uint32_t, 4, Arch>); sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> udot_lane(
      simd<std::uint32_t, 4, Arch> accumulator,
      simd<std::uint8_t, 16, Arch> lhs,
      simd<std::uint8_t, 16, Arch> rhs) noexcept {
    return detail::arm_constant::dot<false, Lane>(accumulator, lhs, rhs);
  }

  /// \cond
  template<isa<arm> Arch, class A, class B, class C>
  void sdot(A, B, C) = delete;
  template<isa<arm> Arch, int Lane, class A, class B, class C>
  void sdot_lane(A, B, C) = delete;
  template<isa<arm> Arch, class A, class B, class C>
  void udot(A, B, C) = delete;
  template<isa<arm> Arch, int Lane, class A, class B, class C>
  void udot_lane(A, B, C) = delete;
  /// \endcond
  /// \}
}
#endif
