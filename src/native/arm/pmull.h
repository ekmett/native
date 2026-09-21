// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
  /// \defgroup arm_pmull Polynomial multiplication
  /// Bit i is the coefficient of x^i. Products use XOR, without carry or
  /// reduction by a modulus. Byte products need NEON; 64-bit products need
  /// FEAT_PMULL. The latter use the compiler "aes" target, whose full
  /// target_features("aes") set must be admitted before entering the leaf.
  /// These integer operations do not read or modify FPCR, FPSR or NZCV.
  /// \{

  /// Multiply two degree-at-most-63 polynomials over GF(2), returning all 128 coefficients.
  template<isa Arch> requires(Arch.has(arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  poly128_t pmull(poly64_t a, poly64_t b) noexcept {
    return vmull_p64(a, b);
  }

  /// Multiply the high 64-bit polynomial lane of each operand; lower lanes are ignored.
  template<isa Arch> requires(Arch.has(arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  poly128_t pmull2(poly64x2_t a, poly64x2_t b) noexcept {
    return vmull_high_p64(a, b);
  }

  /// Multiply eight pairs of byte polynomials into 16-bit products; requires only NEON.
  template<isa Arch> requires(Arch.has(arm_feature::neon))
  native_nodiscard native_inline native_const
  poly16x8_t pmull(poly8x8_t a, poly8x8_t b) noexcept {
    return vmull_p8(a, b);
  }

  /// Multiply the high eight byte polynomials into 16-bit products; requires only NEON.
  template<isa Arch> requires(Arch.has(arm_feature::neon))
  native_nodiscard native_inline native_const
  poly16x8_t pmull2(poly8x16_t a, poly8x16_t b) noexcept {
    return vmull_high_p8(a, b);
  }

  // Reject Clang's implicit same-size vector conversions and scalar narrowing.
  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void pmull(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void pmull2(T...) = delete;

  /// \}
}
#endif
