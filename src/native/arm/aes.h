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
  /// \defgroup arm_aes AES rounds
  /// Single-round AES transformations on a 16-byte state; no key expansion,
  /// padding or cipher mode is performed. Bytes use the ACLE state ordering.
  /// Arch requires FEAT_AES. The compiler target "aes" also enables PMULL;
  /// admit the complete target_features("aes") set before entering that leaf.
  /// These integer operations do not read or modify FPCR, FPSR or NZCV.
  /// \{

  /// Apply AddRoundKey, SubBytes and ShiftRows to one AES state.
  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aese(uint8x16_t state, uint8x16_t round_key) noexcept {
    return vaeseq_u8(state, round_key);
  }

  /// Apply AddRoundKey, inverse ShiftRows and inverse SubBytes to one AES state.
  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesd(uint8x16_t state, uint8x16_t round_key) noexcept {
    return vaesdq_u8(state, round_key);
  }

  /// Apply AES MixColumns independently to the four state columns.
  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesmc(uint8x16_t state) noexcept {
    return vaesmcq_u8(state);
  }

  /// Apply inverse AES MixColumns independently to the four state columns.
  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesimc(uint8x16_t state) noexcept {
    return vaesimcq_u8(state);
  }

  // Reject Clang's implicit same-size vector conversions and scalar narrowing.
  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void aese(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void aesd(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void aesmc(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void aesimc(T...) = delete;

  /// \}
}
#endif
