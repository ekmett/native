#pragma once
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Read after profile.h; this is the accepted exp range policy, not its algorithm.
namespace native::test {
  template<class V> inline constexpr bool exp_uses_single_factor = [] {
#if defined(__aarch64__) || defined(_M_ARM64)
    return true;
#elif defined(__x86_64__) || defined(_M_X64)
    // Preserve every shape with an admitted native VSCALEF implementation.
    constexpr auto arch = V::architecture;
    return !(arch.has(native::x86_feature::avx512f) &&
      (V::lanes == 1 || V::lanes == 16 ||
       (arch.has(native::x86_feature::avx512vl) && V::lanes > 1)));
#else
    return false;
#endif
  }();
}
