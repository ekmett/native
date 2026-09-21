// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)

namespace native {
/** \defgroup arm_dotprod ARM DOTPROD
 * Explicit ISA-gated AArch64 instructions. Admit the feature before entering
 * a matching target scope; these operations provide no software fallback.
 * SDOT and UDOT accumulate modulo 2^32, including signed overflow.
 * \{ */
  /// SDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  int32x2_t sdot(int32x2_t accumulator, int8x8_t lhs, int8x8_t rhs) noexcept {
    return vdot_s32(accumulator, lhs, rhs);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  int32x2_t sdot_lane(int32x2_t accumulator, int8x8_t lhs, int8x8_t rhs) noexcept {
    return vdot_lane_s32(accumulator, lhs, rhs, Lane);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  int32x2_t sdot_lane(int32x2_t accumulator, int8x8_t lhs, int8x16_t rhs) noexcept {
    return vdot_laneq_s32(accumulator, lhs, rhs, Lane);
  }

  /// SDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  int32x4_t sdot(int32x4_t accumulator, int8x16_t lhs, int8x16_t rhs) noexcept {
    return vdotq_s32(accumulator, lhs, rhs);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  int32x4_t sdot_lane(int32x4_t accumulator, int8x16_t lhs, int8x8_t rhs) noexcept {
    return vdotq_lane_s32(accumulator, lhs, rhs, Lane);
  }

  /// SDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  int32x4_t sdot_lane(int32x4_t accumulator, int8x16_t lhs, int8x16_t rhs) noexcept {
    return vdotq_laneq_s32(accumulator, lhs, rhs, Lane);
  }

  /// UDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  uint32x2_t udot(uint32x2_t accumulator, uint8x8_t lhs, uint8x8_t rhs) noexcept {
    return vdot_u32(accumulator, lhs, rhs);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  uint32x2_t udot_lane(uint32x2_t accumulator, uint8x8_t lhs, uint8x8_t rhs) noexcept {
    return vdot_lane_u32(accumulator, lhs, rhs, Lane);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  uint32x2_t udot_lane(uint32x2_t accumulator, uint8x8_t lhs, uint8x16_t rhs) noexcept {
    return vdot_laneq_u32(accumulator, lhs, rhs, Lane);
  }

  /// UDOT: four byte products per accumulator lane, modulo 2^32.
  template<isa Arch> requires(Arch.has(arm_feature::dotprod))
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  uint32x4_t udot(uint32x4_t accumulator, uint8x16_t lhs, uint8x16_t rhs) noexcept {
    return vdotq_u32(accumulator, lhs, rhs);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  uint32x4_t udot_lane(uint32x4_t accumulator, uint8x16_t lhs, uint8x8_t rhs) noexcept {
    return vdotq_lane_u32(accumulator, lhs, rhs, Lane);
  }

  /// UDOT by element: broadcast rhs byte group Lane to every accumulator lane.
  template<isa Arch, int Lane> requires(Arch.has(arm_feature::dotprod) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("dotprod")))
  uint32x4_t udot_lane(uint32x4_t accumulator, uint8x16_t lhs, uint8x16_t rhs) noexcept {
    return vdotq_laneq_u32(accumulator, lhs, rhs, Lane);
  }

  // Reject Clang's lax vector conversions and scalar narrowing when an exact
  // instruction shape or immediate lane is unavailable.
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
