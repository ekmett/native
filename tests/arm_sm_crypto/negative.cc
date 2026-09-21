// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.sm3;
import native.arm.sm4;
#if NATIVE_SM_REJECT_KIND == 0
constexpr auto architecture = native::neon;
#else
constexpr auto architecture = native::target_features<native::arm>("sm4");
#endif
#if NATIVE_SM_REJECT_KIND == 2
using V = native::simd<std::uint32_t, 2, architecture>;
#else
using V = native::simd<std::uint32_t, 4, architecture>;
#endif
#if NATIVE_SM_REJECT_KIND == 1
__attribute__((target("cpu=generic,arch=armv8-a")))
#else
__attribute__((target("sm4")))
#endif
V rejected(V a, V b, V c) {
#if NATIVE_SM_REJECT_OP == 0
  return native::sm3ss1(a, b, c);
#endif
#if NATIVE_SM_REJECT_OP == 1
#if NATIVE_SM_REJECT_KIND == 3
  constexpr int lane = -1;
#elif NATIVE_SM_REJECT_KIND == 4
  constexpr int lane = 4;
#else
  constexpr int lane = 0;
#endif
  return native::sm3tt1a<architecture, lane>(a, b, c);
#endif
#if NATIVE_SM_REJECT_OP == 2
#if NATIVE_SM_REJECT_KIND == 3
  constexpr int lane = -1;
#elif NATIVE_SM_REJECT_KIND == 4
  constexpr int lane = 4;
#else
  constexpr int lane = 0;
#endif
  return native::sm3tt1b<architecture, lane>(a, b, c);
#endif
#if NATIVE_SM_REJECT_OP == 3
#if NATIVE_SM_REJECT_KIND == 3
  constexpr int lane = -1;
#elif NATIVE_SM_REJECT_KIND == 4
  constexpr int lane = 4;
#else
  constexpr int lane = 0;
#endif
  return native::sm3tt2a<architecture, lane>(a, b, c);
#endif
#if NATIVE_SM_REJECT_OP == 4
#if NATIVE_SM_REJECT_KIND == 3
  constexpr int lane = -1;
#elif NATIVE_SM_REJECT_KIND == 4
  constexpr int lane = 4;
#else
  constexpr int lane = 0;
#endif
  return native::sm3tt2b<architecture, lane>(a, b, c);
#endif
#if NATIVE_SM_REJECT_OP == 5
  return native::sm3partw1(a, b, c);
#endif
#if NATIVE_SM_REJECT_OP == 6
  return native::sm3partw2(a, b, c);
#endif
#if NATIVE_SM_REJECT_OP == 7
  return native::sm4e(a, b);
#endif
#if NATIVE_SM_REJECT_OP == 8
  return native::sm4ekey(a, b);
#endif
}
