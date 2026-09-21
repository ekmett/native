// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/crc.h>
#if TEST_KIND == 0
constexpr auto requirements=native::neon;
#else
constexpr auto requirements=native::target_features("crc");
#endif
#if TEST_KIND != 1
native_target("crc")
#endif
std::uint32_t rejected(std::uint32_t a,std::uint64_t b) {
#if TEST_KIND == 2
  return native::crc32c<requirements>(a,double(b));
#elif TEST_KIND == 3
  return native::crc32<requirements>(std::uint64_t(a),b);
#else
  return native::crc32c<requirements>(a,b);
#endif
}
