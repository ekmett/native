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
namespace native::detail::arm_pmull {

  template<isa Arch> requires(Arch.has(arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  poly128_t pmull(poly64_t a, poly64_t b) noexcept {
    return vmull_p64(a, b);
  }

  template<isa Arch> requires(Arch.has(arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  poly128_t pmull2(poly64x2_t a, poly64x2_t b) noexcept {
    return vmull_high_p64(a, b);
  }

  template<isa Arch> requires(Arch.has(arm_feature::neon))
  native_nodiscard native_inline native_const
  poly16x8_t pmull(poly8x8_t a, poly8x8_t b) noexcept {
    return vmull_p8(a, b);
  }

  template<isa Arch> requires(Arch.has(arm_feature::neon))
  native_nodiscard native_inline native_const
  poly16x8_t pmull2(poly8x16_t a, poly8x16_t b) noexcept {
    return vmull_high_p8(a, b);
  }

  // Reject Clang's implicit same-size vector conversions and scalar narrowing.
  template<isa Arch, class... T> void pmull(T...) = delete;

  template<isa Arch, class... T> void pmull2(T...) = delete;

}
#endif
/// \endcond
