// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
/// \cond NATIVE_DETAIL
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_NEON
#include <arm_neon.h>

namespace native::detail::arm_sm3 {
  template<isa<arm> Arch>
    requires(Arch.has(arm_feature::sm3))
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm3ss1(
    uint32x4_t a, uint32x4_t b, uint32x4_t c) noexcept {
    return vsm3ss1q_u32(a, b, c);
  }

  template<isa<arm> Arch, int Lane>
    requires(Arch.has(arm_feature::sm3) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm3tt1a(
    uint32x4_t state, uint32x4_t ss1, uint32x4_t words) noexcept {
    return vsm3tt1aq_u32(state, ss1, words, Lane);
  }

  template<isa<arm> Arch, int Lane>
    requires(Arch.has(arm_feature::sm3) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm3tt1b(
    uint32x4_t state, uint32x4_t ss1, uint32x4_t words) noexcept {
    return vsm3tt1bq_u32(state, ss1, words, Lane);
  }

  template<isa<arm> Arch, int Lane>
    requires(Arch.has(arm_feature::sm3) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm3tt2a(
    uint32x4_t state, uint32x4_t ss1, uint32x4_t words) noexcept {
    return vsm3tt2aq_u32(state, ss1, words, Lane);
  }

  template<isa<arm> Arch, int Lane>
    requires(Arch.has(arm_feature::sm3) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm3tt2b(
    uint32x4_t state, uint32x4_t ss1, uint32x4_t words) noexcept {
    return vsm3tt2bq_u32(state, ss1, words, Lane);
  }

  template<isa<arm> Arch>
    requires(Arch.has(arm_feature::sm3))
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm3partw1(
    uint32x4_t a, uint32x4_t b, uint32x4_t c) noexcept {
    return vsm3partw1q_u32(a, b, c);
  }

  template<isa<arm> Arch>
    requires(Arch.has(arm_feature::sm3))
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm3partw2(
    uint32x4_t a, uint32x4_t b, uint32x4_t c) noexcept {
    return vsm3partw2q_u32(a, b, c);
  }

} // namespace native::detail::arm_sm3
#endif
/// \endcond
