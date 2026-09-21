// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.i8mm;
#if defined(TEST_NO_FEATURE)
constexpr auto arch = native::neon;
#else
constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
#endif
using accumulator = native::simd<std::int32_t, 4, arch>;
using signed_bytes = native::simd<std::int8_t, 16, arch>;
#if defined(TEST_NO_FEATURE)
__attribute__((target("i8mm")))
auto rejected(accumulator c, signed_bytes a) { return native::smmla<arch>(c,a,a); }
#elif defined(TEST_BAD_LANE)
__attribute__((target("i8mm")))
auto rejected(accumulator c, native::simd<std::uint8_t, 16, arch> a,
    native::simd<std::int8_t, 8, arch> b) {
  return native::usdot_lane<arch,2>(c,a,b);
}
#else
auto rejected(accumulator c, signed_bytes a) { return native::smmla<arch>(c,a,a); }
#endif
