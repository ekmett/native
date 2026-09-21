// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.rdm;
#if NATIVE_INTEGER_REJECT == 0
auto rejected(int16_t accumulator, int16_t lhs, int16_t rhs) { return native::sqrdmlah<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 1
auto rejected(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 2
auto rejected(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 3
auto rejected(native::simd<std::int16_t, 4, native::neon> accumulator, native::simd<std::int16_t, 4, native::neon> lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlah<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 4
auto rejected(native::simd<std::int16_t, 4, native::neon> accumulator, native::simd<std::int16_t, 4, native::neon> lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 5
auto rejected(native::simd<std::int16_t, 4, native::neon> accumulator, native::simd<std::int16_t, 4, native::neon> lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 6
auto rejected(native::simd<std::int16_t, 8, native::neon> accumulator, native::simd<std::int16_t, 8, native::neon> lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlah<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 7
auto rejected(native::simd<std::int16_t, 8, native::neon> accumulator, native::simd<std::int16_t, 8, native::neon> lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 8
auto rejected(native::simd<std::int16_t, 8, native::neon> accumulator, native::simd<std::int16_t, 8, native::neon> lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 9
auto rejected(int32_t accumulator, int32_t lhs, int32_t rhs) { return native::sqrdmlah<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 10
auto rejected(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 11
auto rejected(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 12
auto rejected(native::simd<std::int32_t, 2, native::neon> accumulator, native::simd<std::int32_t, 2, native::neon> lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlah<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 13
auto rejected(native::simd<std::int32_t, 2, native::neon> accumulator, native::simd<std::int32_t, 2, native::neon> lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 14
auto rejected(native::simd<std::int32_t, 2, native::neon> accumulator, native::simd<std::int32_t, 2, native::neon> lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 15
auto rejected(native::simd<std::int32_t, 4, native::neon> accumulator, native::simd<std::int32_t, 4, native::neon> lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlah<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 16
auto rejected(native::simd<std::int32_t, 4, native::neon> accumulator, native::simd<std::int32_t, 4, native::neon> lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 17
auto rejected(native::simd<std::int32_t, 4, native::neon> accumulator, native::simd<std::int32_t, 4, native::neon> lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlah_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 18
auto rejected(int16_t accumulator, int16_t lhs, int16_t rhs) { return native::sqrdmlsh<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 19
auto rejected(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 20
auto rejected(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 21
auto rejected(native::simd<std::int16_t, 4, native::neon> accumulator, native::simd<std::int16_t, 4, native::neon> lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlsh<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 22
auto rejected(native::simd<std::int16_t, 4, native::neon> accumulator, native::simd<std::int16_t, 4, native::neon> lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 23
auto rejected(native::simd<std::int16_t, 4, native::neon> accumulator, native::simd<std::int16_t, 4, native::neon> lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 24
auto rejected(native::simd<std::int16_t, 8, native::neon> accumulator, native::simd<std::int16_t, 8, native::neon> lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlsh<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 25
auto rejected(native::simd<std::int16_t, 8, native::neon> accumulator, native::simd<std::int16_t, 8, native::neon> lhs, native::simd<std::int16_t, 4, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 26
auto rejected(native::simd<std::int16_t, 8, native::neon> accumulator, native::simd<std::int16_t, 8, native::neon> lhs, native::simd<std::int16_t, 8, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 27
auto rejected(int32_t accumulator, int32_t lhs, int32_t rhs) { return native::sqrdmlsh<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 28
auto rejected(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 29
auto rejected(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 30
auto rejected(native::simd<std::int32_t, 2, native::neon> accumulator, native::simd<std::int32_t, 2, native::neon> lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlsh<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 31
auto rejected(native::simd<std::int32_t, 2, native::neon> accumulator, native::simd<std::int32_t, 2, native::neon> lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 32
auto rejected(native::simd<std::int32_t, 2, native::neon> accumulator, native::simd<std::int32_t, 2, native::neon> lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 33
auto rejected(native::simd<std::int32_t, 4, native::neon> accumulator, native::simd<std::int32_t, 4, native::neon> lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlsh<native::neon>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 34
auto rejected(native::simd<std::int32_t, 4, native::neon> accumulator, native::simd<std::int32_t, 4, native::neon> lhs, native::simd<std::int32_t, 2, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
#if NATIVE_INTEGER_REJECT == 35
auto rejected(native::simd<std::int32_t, 4, native::neon> accumulator, native::simd<std::int32_t, 4, native::neon> lhs, native::simd<std::int32_t, 4, native::neon> rhs) { return native::sqrdmlsh_lane<native::neon, 0>(accumulator, lhs, rhs); }
#endif
