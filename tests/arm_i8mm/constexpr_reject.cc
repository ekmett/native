// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.i8mm;
#if NATIVE_INTEGER_REJECT == 0
auto rejected(native::simd<std::int32_t, 4, native::neon> acc, native::simd<std::int8_t, 16, native::neon> a, native::simd<std::int8_t, 16, native::neon> b) { return native::smmla<native::neon>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 1
auto rejected(native::simd<std::uint32_t, 4, native::neon> acc, native::simd<std::uint8_t, 16, native::neon> a, native::simd<std::uint8_t, 16, native::neon> b) { return native::ummla<native::neon>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 2
auto rejected(native::simd<std::int32_t, 4, native::neon> acc, native::simd<std::uint8_t, 16, native::neon> a, native::simd<std::int8_t, 16, native::neon> b) { return native::usmmla<native::neon>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 3
auto rejected(native::simd<std::int32_t, 2, native::neon> acc, native::simd<std::uint8_t, 8, native::neon> a, native::simd<std::int8_t, 8, native::neon> b) { return native::usdot<native::neon>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 4
auto rejected(native::simd<std::int32_t, 2, native::neon> acc, native::simd<std::uint8_t, 8, native::neon> a, native::simd<std::int8_t, 8, native::neon> b) { return native::usdot_lane<native::neon, 0>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 5
auto rejected(native::simd<std::int32_t, 2, native::neon> acc, native::simd<std::int8_t, 8, native::neon> a, native::simd<std::uint8_t, 8, native::neon> b) { return native::sudot_lane<native::neon, 0>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 6
auto rejected(native::simd<std::int32_t, 2, native::neon> acc, native::simd<std::uint8_t, 8, native::neon> a, native::simd<std::int8_t, 16, native::neon> b) { return native::usdot_lane<native::neon, 0>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 7
auto rejected(native::simd<std::int32_t, 2, native::neon> acc, native::simd<std::int8_t, 8, native::neon> a, native::simd<std::uint8_t, 16, native::neon> b) { return native::sudot_lane<native::neon, 0>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 8
auto rejected(native::simd<std::int32_t, 4, native::neon> acc, native::simd<std::uint8_t, 16, native::neon> a, native::simd<std::int8_t, 16, native::neon> b) { return native::usdot<native::neon>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 9
auto rejected(native::simd<std::int32_t, 4, native::neon> acc, native::simd<std::uint8_t, 16, native::neon> a, native::simd<std::int8_t, 8, native::neon> b) { return native::usdot_lane<native::neon, 0>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 10
auto rejected(native::simd<std::int32_t, 4, native::neon> acc, native::simd<std::int8_t, 16, native::neon> a, native::simd<std::uint8_t, 8, native::neon> b) { return native::sudot_lane<native::neon, 0>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 11
auto rejected(native::simd<std::int32_t, 4, native::neon> acc, native::simd<std::uint8_t, 16, native::neon> a, native::simd<std::int8_t, 16, native::neon> b) { return native::usdot_lane<native::neon, 0>(acc, a, b); }
#endif
#if NATIVE_INTEGER_REJECT == 12
auto rejected(native::simd<std::int32_t, 4, native::neon> acc, native::simd<std::int8_t, 16, native::neon> a, native::simd<std::uint8_t, 16, native::neon> b) { return native::sudot_lane<native::neon, 0>(acc, a, b); }
#endif
