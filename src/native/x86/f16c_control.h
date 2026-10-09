// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/detail/constexpr_float.h"
#include <hint.h>
#if NATIVE_HOST_X86
#include <immintrin.h>
namespace native::detail::x86_f16c {
  struct control {
    constexpr_float::rounding mode;
    constexpr_float::policy environment;
  };
  [[nodiscard]] hint_inline control current_control() noexcept {
    auto csr = _mm_getcsr();
    constexpr_float::policy environment{};
    environment.flush_inputs = (csr & 0x40u) != 0;
    using r = constexpr_float::rounding;
    constexpr r modes[]{r::nearest_even, r::downward, r::upward, r::toward_zero};
    return {modes[(csr >> 13) & 3u], environment};
  }
}
#endif
