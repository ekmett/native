// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
import native.x86.sha;

#if NATIVE_TEST_WEAK
constexpr auto arch = native::target_features<native::x86>("sse2");
#else
constexpr auto arch = native::target_features<native::x86>("sha");
#endif
using vector = native::simd<std::uint32_t, 4, arch>;

native_noinline native_target("sha")
auto reject_selector(vector value, unsigned selector) {
#if NATIVE_TEST_IMMEDIATE == 1
  return native::sha1rnds4<arch, 4>(value, value);
#elif NATIVE_TEST_IMMEDIATE == 2
  return native::sha1rnds4<arch, 255>(value, value);
#elif NATIVE_TEST_IMMEDIATE == 3
  return native::sha1rnds4<arch, static_cast<unsigned>(-1)>(value, value);
#elif NATIVE_TEST_IMMEDIATE == 4
  return native::sha1rnds4<arch, selector>(value, value);
#else
  constexpr std::uint32_t zeros[4]{};
  constexpr auto zero = vector::load(zeros);
  return native::sha1rnds4<arch, 0>(zero, zero);
#endif
}
