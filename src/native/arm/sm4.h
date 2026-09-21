// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
/// \cond NATIVE_DETAIL
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_NEON
#include <arm_neon.h>

namespace native::detail::arm_sm4 {
  template<isa<arm> Arch>
    requires(Arch.has(arm_feature::sm4))
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm4e(
    uint32x4_t a, uint32x4_t b) noexcept {
    return vsm4eq_u32(a, b);
  }

  template<isa<arm> Arch>
    requires(Arch.has(arm_feature::sm4))
  native_nodiscard native_inline native_const native_target("sm4") uint32x4_t sm4ekey(
    uint32x4_t a, uint32x4_t b) noexcept {
    return vsm4ekeyq_u32(a, b);
  }

} // namespace native::detail::arm_sm4
#endif
/// \endcond
