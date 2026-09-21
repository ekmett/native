// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
/// \cond NATIVE_DETAIL
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native::detail::arm_aes {

  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aese(uint8x16_t state, uint8x16_t round_key) noexcept {
    return vaeseq_u8(state, round_key);
  }

  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesd(uint8x16_t state, uint8x16_t round_key) noexcept {
    return vaesdq_u8(state, round_key);
  }

  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesmc(uint8x16_t state) noexcept {
    return vaesmcq_u8(state);
  }

  template<isa Arch> requires(Arch.has(arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesimc(uint8x16_t state) noexcept {
    return vaesimcq_u8(state);
  }

  // Reject Clang's implicit same-size vector conversions and scalar narrowing.
  template<isa Arch, class... T> void aese(T...) = delete;

  template<isa Arch, class... T> void aesd(T...) = delete;

  template<isa Arch, class... T> void aesmc(T...) = delete;

  template<isa Arch, class... T> void aesimc(T...) = delete;

}
#endif
/// \endcond
