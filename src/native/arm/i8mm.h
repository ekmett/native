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
  /// \defgroup arm_i8mm I8MM
  /// Advanced SIMD integer matrix and mixed-sign dot products. Requires I8MM,
  /// independently of DotProd, FP16 and BF16. All sums wrap modulo 2^32;
  /// signed results interpret the resulting bits as two's complement.
  /// Matrix operands hold two rows of eight bytes in a and two columns of
  /// eight bytes in b. Result lane 2*r+c accumulates sum(a[8*r+k]*b[8*c+k]).
  /// \{

  /// Signed 2x8 times 8x2 matrix multiply-accumulate.
  template<isa Arch> requires(Arch.has(arm_feature::i8mm))
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x4_t smmla(int32x4_t acc, int8x16_t a, int8x16_t b) noexcept {
    return vmmlaq_s32(acc,a,b);
  }

  /// Unsigned 2x8 times 8x2 matrix multiply-accumulate.
  template<isa Arch> requires(Arch.has(arm_feature::i8mm))
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  uint32x4_t ummla(uint32x4_t acc, uint8x16_t a, uint8x16_t b) noexcept {
    return vmmlaq_u32(acc,a,b);
  }

  /// Unsigned left matrix times signed right matrix, with signed accumulator.
  template<isa Arch> requires(Arch.has(arm_feature::i8mm))
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x4_t usmmla(int32x4_t acc, uint8x16_t a, int8x16_t b) noexcept {
    return vusmmlaq_s32(acc,a,b);
  }

  /// Accumulate each corresponding group of four unsigned a bytes times signed b bytes.
  template<isa Arch> requires(Arch.has(arm_feature::i8mm))
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x2_t usdot(int32x2_t acc, uint8x8_t a, int8x8_t b) noexcept {
    return vusdot_s32(acc,a,b);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x2_t usdot_lane(int32x2_t acc, uint8x8_t a, int8x8_t b) noexcept {
    return vusdot_lane_s32(acc,a,b,Lane);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x2_t sudot_lane(int32x2_t acc, int8x8_t a, uint8x8_t b) noexcept {
    return vsudot_lane_s32(acc,a,b,Lane);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x2_t usdot_lane(int32x2_t acc, uint8x8_t a, int8x16_t b) noexcept {
    return vusdot_laneq_s32(acc,a,b,Lane);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x2_t sudot_lane(int32x2_t acc, int8x8_t a, uint8x16_t b) noexcept {
    return vsudot_laneq_s32(acc,a,b,Lane);
  }

  /// Accumulate each corresponding group of four unsigned a bytes times signed b bytes.
  template<isa Arch> requires(Arch.has(arm_feature::i8mm))
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x4_t usdot(int32x4_t acc, uint8x16_t a, int8x16_t b) noexcept {
    return vusdotq_s32(acc,a,b);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x4_t usdot_lane(int32x4_t acc, uint8x16_t a, int8x8_t b) noexcept {
    return vusdotq_lane_s32(acc,a,b,Lane);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 2)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x4_t sudot_lane(int32x4_t acc, int8x16_t a, uint8x8_t b) noexcept {
    return vsudotq_lane_s32(acc,a,b,Lane);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x4_t usdot_lane(int32x4_t acc, uint8x16_t a, int8x16_t b) noexcept {
    return vusdotq_laneq_s32(acc,a,b,Lane);
  }

  /// Accumulate each four-byte group of a times b[4*Lane..4*Lane+3].
  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::i8mm) && Lane < 4)
  native_nodiscard native_inline native_const __attribute__((target("i8mm")))
  int32x4_t sudot_lane(int32x4_t acc, int8x16_t a, uint8x16_t b) noexcept {
    return vsudotq_laneq_s32(acc,a,b,Lane);
  }

  /// \}
}
#endif
