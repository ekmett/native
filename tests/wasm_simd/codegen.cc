// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <wasm_simd128.h>
import native;

constexpr auto architecture = native::feature_closure(native::wasm_feature::simd128);
template <class T> using V = native::simd<T, 16 / sizeof(T), architecture>;
using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

// Scalar pointer arguments carry natural alignment on both sides; the raw
// memory intrinsics take void pointers, so make that guarantee explicit.
// Both sides expose exactly the same register/scalar ABI. Public cases only use
// the implementation bridge to enter and leave the typed vector operation.
#define PAIR(name, result, parameters, raw_expression, public_expression)                          \
  extern "C" __attribute__((target("simd128"), noinline, used)) result raw_##name parameters {     \
    return raw_expression;                                                                         \
  }                                                                                                \
  extern "C" __attribute__((target("simd128"), noinline, used)) result public_##name parameters {  \
    return public_expression;                                                                      \
  }

PAIR(i8x16_add, v128_t, (v128_t x, v128_t y), (wasm_i8x16_add(x, y)),
     ((V<i8>::from_native(x) + V<i8>::from_native(y)).to_native()))

PAIR(i8x16_sub, v128_t, (v128_t x, v128_t y), (wasm_i8x16_sub(x, y)),
     ((V<i8>::from_native(x) - V<i8>::from_native(y)).to_native()))

PAIR(i8x16_eq, v128_t, (v128_t x, v128_t y), (wasm_i8x16_eq(x, y)),
     ((V<i8>::from_native(x) == V<i8>::from_native(y)).to_native()))

PAIR(i8x16_ne, v128_t, (v128_t x, v128_t y), (wasm_i8x16_ne(x, y)),
     ((V<i8>::from_native(x) != V<i8>::from_native(y)).to_native()))

PAIR(i8x16_lt, v128_t, (v128_t x, v128_t y), (wasm_i8x16_lt(x, y)),
     ((V<i8>::from_native(x) < V<i8>::from_native(y)).to_native()))

PAIR(i8x16_le, v128_t, (v128_t x, v128_t y), (wasm_i8x16_le(x, y)),
     ((V<i8>::from_native(x) <= V<i8>::from_native(y)).to_native()))

PAIR(i8x16_gt, v128_t, (v128_t x, v128_t y), (wasm_i8x16_gt(x, y)),
     ((V<i8>::from_native(x) > V<i8>::from_native(y)).to_native()))

PAIR(i8x16_ge, v128_t, (v128_t x, v128_t y), (wasm_i8x16_ge(x, y)),
     ((V<i8>::from_native(x) >= V<i8>::from_native(y)).to_native()))

PAIR(i8x16_min, v128_t, (v128_t x, v128_t y), (wasm_i8x16_min(x, y)),
     (native::min(V<i8>::from_native(x), V<i8>::from_native(y)).to_native()))

PAIR(i8x16_max, v128_t, (v128_t x, v128_t y), (wasm_i8x16_max(x, y)),
     (native::max(V<i8>::from_native(x), V<i8>::from_native(y)).to_native()))

PAIR(i8x16_abs, v128_t, (v128_t x), (wasm_i8x16_abs(x)),
     (native::abs(V<i8>::from_native(x)).to_native()))

PAIR(i8x16_neg, v128_t, (v128_t x), (wasm_i8x16_neg(x)), ((-V<i8>::from_native(x)).to_native()))

PAIR(i8x16_splat, v128_t, (i8 x), (wasm_i8x16_splat(x)), (V<i8>(x).to_native()))

PAIR(i8x16_extract, i8, (v128_t x), (wasm_i8x16_extract_lane(x, 1)),
     (V<i8>::from_native(x).get<1>()))

PAIR(i8x16_replace, v128_t, (v128_t x, i8 y), (wasm_i8x16_replace_lane(x, 1, y)),
     (V<i8>::from_native(x).replace<1>(y).to_native()))

PAIR(i8x16_shl, v128_t, (v128_t x, unsigned count), (wasm_i8x16_shl(x, count)),
     ((V<i8>::from_native(x) << count).to_native()))

PAIR(i8x16_shr, v128_t, (v128_t x, unsigned count), (wasm_i8x16_shr(x, count)),
     ((V<i8>::from_native(x) >> count).to_native()))

PAIR(i8x16_bitmask, unsigned, (v128_t x), (wasm_i8x16_bitmask(x)),
     (native::bitmask(V<i8>::from_native(x))))

PAIR(i8x16_all_true, bool, (v128_t x), (wasm_i8x16_all_true(x)),
     (native::all(V<i8>::from_native(x))))

PAIR(i8x16_add_sat, v128_t, (v128_t x, v128_t y), (wasm_i8x16_add_sat(x, y)),
     (native::add_sat(V<i8>::from_native(x), V<i8>::from_native(y)).to_native()))

PAIR(i8x16_sub_sat, v128_t, (v128_t x, v128_t y), (wasm_i8x16_sub_sat(x, y)),
     (native::sub_sat(V<i8>::from_native(x), V<i8>::from_native(y)).to_native()))

PAIR(i8x16_pairwise, v128_t, (v128_t x), (wasm_i16x8_extadd_pairwise_i8x16(x)),
     (native::pairwise_add_widened(V<i8>::from_native(x)).to_native()))

PAIR(i8x16_extend_low, v128_t, (v128_t x), (wasm_i16x8_extend_low_i8x16(x)),
     (native::extend_low(V<i8>::from_native(x)).to_native()))

PAIR(i8x16_extmul_low, v128_t, (v128_t x, v128_t y), (wasm_i16x8_extmul_low_i8x16(x, y)),
     (native::multiply_widened_low(V<i8>::from_native(x), V<i8>::from_native(y)).to_native()))

PAIR(i8x16_extend_high, v128_t, (v128_t x), (wasm_i16x8_extend_high_i8x16(x)),
     (native::extend_high(V<i8>::from_native(x)).to_native()))

PAIR(i8x16_extmul_high, v128_t, (v128_t x, v128_t y), (wasm_i16x8_extmul_high_i8x16(x, y)),
     (native::multiply_widened_high(V<i8>::from_native(x), V<i8>::from_native(y)).to_native()))

PAIR(i8x16_load_widened, v128_t, (i8 const *p), (wasm_i16x8_load8x8(p)),
     (native::load_widened<i16, i8, architecture>(p).to_native()))

PAIR(u8x16_add, v128_t, (v128_t x, v128_t y), (wasm_i8x16_add(x, y)),
     ((V<u8>::from_native(x) + V<u8>::from_native(y)).to_native()))

PAIR(u8x16_sub, v128_t, (v128_t x, v128_t y), (wasm_i8x16_sub(x, y)),
     ((V<u8>::from_native(x) - V<u8>::from_native(y)).to_native()))

PAIR(u8x16_eq, v128_t, (v128_t x, v128_t y), (wasm_i8x16_eq(x, y)),
     ((V<u8>::from_native(x) == V<u8>::from_native(y)).to_native()))

PAIR(u8x16_ne, v128_t, (v128_t x, v128_t y), (wasm_i8x16_ne(x, y)),
     ((V<u8>::from_native(x) != V<u8>::from_native(y)).to_native()))

PAIR(u8x16_lt, v128_t, (v128_t x, v128_t y), (wasm_u8x16_lt(x, y)),
     ((V<u8>::from_native(x) < V<u8>::from_native(y)).to_native()))

PAIR(u8x16_le, v128_t, (v128_t x, v128_t y), (wasm_u8x16_le(x, y)),
     ((V<u8>::from_native(x) <= V<u8>::from_native(y)).to_native()))

PAIR(u8x16_gt, v128_t, (v128_t x, v128_t y), (wasm_u8x16_gt(x, y)),
     ((V<u8>::from_native(x) > V<u8>::from_native(y)).to_native()))

PAIR(u8x16_ge, v128_t, (v128_t x, v128_t y), (wasm_u8x16_ge(x, y)),
     ((V<u8>::from_native(x) >= V<u8>::from_native(y)).to_native()))

PAIR(u8x16_min, v128_t, (v128_t x, v128_t y), (wasm_u8x16_min(x, y)),
     (native::min(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(u8x16_max, v128_t, (v128_t x, v128_t y), (wasm_u8x16_max(x, y)),
     (native::max(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(u8x16_splat, v128_t, (u8 x), (wasm_i8x16_splat(x)), (V<u8>(x).to_native()))

PAIR(u8x16_extract, u8, (v128_t x), (wasm_u8x16_extract_lane(x, 1)),
     (V<u8>::from_native(x).get<1>()))

PAIR(u8x16_replace, v128_t, (v128_t x, u8 y), (wasm_i8x16_replace_lane(x, 1, y)),
     (V<u8>::from_native(x).replace<1>(y).to_native()))

PAIR(u8x16_shl, v128_t, (v128_t x, unsigned count), (wasm_i8x16_shl(x, count)),
     ((V<u8>::from_native(x) << count).to_native()))

PAIR(u8x16_shr, v128_t, (v128_t x, unsigned count), (wasm_u8x16_shr(x, count)),
     ((V<u8>::from_native(x) >> count).to_native()))

PAIR(u8x16_bitmask, unsigned, (v128_t x), (wasm_i8x16_bitmask(x)),
     (native::bitmask(V<u8>::from_native(x))))

PAIR(u8x16_all_true, bool, (v128_t x), (wasm_i8x16_all_true(x)),
     (native::all(V<u8>::from_native(x))))

PAIR(u8x16_add_sat, v128_t, (v128_t x, v128_t y), (wasm_u8x16_add_sat(x, y)),
     (native::add_sat(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(u8x16_sub_sat, v128_t, (v128_t x, v128_t y), (wasm_u8x16_sub_sat(x, y)),
     (native::sub_sat(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(u8x16_avgr, v128_t, (v128_t x, v128_t y), (wasm_u8x16_avgr(x, y)),
     (native::average_round(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(u8x16_pairwise, v128_t, (v128_t x), (wasm_u16x8_extadd_pairwise_u8x16(x)),
     (native::pairwise_add_widened(V<u8>::from_native(x)).to_native()))

PAIR(u8x16_extend_low, v128_t, (v128_t x), (wasm_u16x8_extend_low_u8x16(x)),
     (native::extend_low(V<u8>::from_native(x)).to_native()))

PAIR(u8x16_extmul_low, v128_t, (v128_t x, v128_t y), (wasm_u16x8_extmul_low_u8x16(x, y)),
     (native::multiply_widened_low(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(u8x16_extend_high, v128_t, (v128_t x), (wasm_u16x8_extend_high_u8x16(x)),
     (native::extend_high(V<u8>::from_native(x)).to_native()))

PAIR(u8x16_extmul_high, v128_t, (v128_t x, v128_t y), (wasm_u16x8_extmul_high_u8x16(x, y)),
     (native::multiply_widened_high(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(u8x16_load_widened, v128_t, (u8 const *p), (wasm_u16x8_load8x8(p)),
     (native::load_widened<u16, u8, architecture>(p).to_native()))

PAIR(i16x8_add, v128_t, (v128_t x, v128_t y), (wasm_i16x8_add(x, y)),
     ((V<i16>::from_native(x) + V<i16>::from_native(y)).to_native()))

PAIR(i16x8_sub, v128_t, (v128_t x, v128_t y), (wasm_i16x8_sub(x, y)),
     ((V<i16>::from_native(x) - V<i16>::from_native(y)).to_native()))

PAIR(i16x8_mul, v128_t, (v128_t x, v128_t y), (wasm_i16x8_mul(x, y)),
     ((V<i16>::from_native(x) * V<i16>::from_native(y)).to_native()))

PAIR(i16x8_eq, v128_t, (v128_t x, v128_t y), (wasm_i16x8_eq(x, y)),
     ((V<i16>::from_native(x) == V<i16>::from_native(y)).to_native()))

PAIR(i16x8_ne, v128_t, (v128_t x, v128_t y), (wasm_i16x8_ne(x, y)),
     ((V<i16>::from_native(x) != V<i16>::from_native(y)).to_native()))

PAIR(i16x8_lt, v128_t, (v128_t x, v128_t y), (wasm_i16x8_lt(x, y)),
     ((V<i16>::from_native(x) < V<i16>::from_native(y)).to_native()))

PAIR(i16x8_le, v128_t, (v128_t x, v128_t y), (wasm_i16x8_le(x, y)),
     ((V<i16>::from_native(x) <= V<i16>::from_native(y)).to_native()))

PAIR(i16x8_gt, v128_t, (v128_t x, v128_t y), (wasm_i16x8_gt(x, y)),
     ((V<i16>::from_native(x) > V<i16>::from_native(y)).to_native()))

PAIR(i16x8_ge, v128_t, (v128_t x, v128_t y), (wasm_i16x8_ge(x, y)),
     ((V<i16>::from_native(x) >= V<i16>::from_native(y)).to_native()))

PAIR(i16x8_min, v128_t, (v128_t x, v128_t y), (wasm_i16x8_min(x, y)),
     (native::min(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i16x8_max, v128_t, (v128_t x, v128_t y), (wasm_i16x8_max(x, y)),
     (native::max(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i16x8_abs, v128_t, (v128_t x), (wasm_i16x8_abs(x)),
     (native::abs(V<i16>::from_native(x)).to_native()))

PAIR(i16x8_neg, v128_t, (v128_t x), (wasm_i16x8_neg(x)), ((-V<i16>::from_native(x)).to_native()))

PAIR(i16x8_splat, v128_t, (i16 x), (wasm_i16x8_splat(x)), (V<i16>(x).to_native()))

PAIR(i16x8_extract, i16, (v128_t x), (wasm_i16x8_extract_lane(x, 1)),
     (V<i16>::from_native(x).get<1>()))

PAIR(i16x8_replace, v128_t, (v128_t x, i16 y), (wasm_i16x8_replace_lane(x, 1, y)),
     (V<i16>::from_native(x).replace<1>(y).to_native()))

PAIR(i16x8_shl, v128_t, (v128_t x, unsigned count), (wasm_i16x8_shl(x, count)),
     ((V<i16>::from_native(x) << count).to_native()))

PAIR(i16x8_shr, v128_t, (v128_t x, unsigned count), (wasm_i16x8_shr(x, count)),
     ((V<i16>::from_native(x) >> count).to_native()))

PAIR(i16x8_bitmask, unsigned, (v128_t x), (wasm_i16x8_bitmask(x)),
     (native::bitmask(V<i16>::from_native(x))))

PAIR(i16x8_all_true, bool, (v128_t x), (wasm_i16x8_all_true(x)),
     (native::all(V<i16>::from_native(x))))

PAIR(i16x8_add_sat, v128_t, (v128_t x, v128_t y), (wasm_i16x8_add_sat(x, y)),
     (native::add_sat(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i16x8_sub_sat, v128_t, (v128_t x, v128_t y), (wasm_i16x8_sub_sat(x, y)),
     (native::sub_sat(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i16x8_pairwise, v128_t, (v128_t x), (wasm_i32x4_extadd_pairwise_i16x8(x)),
     (native::pairwise_add_widened(V<i16>::from_native(x)).to_native()))

PAIR(i16x8_extend_low, v128_t, (v128_t x), (wasm_i32x4_extend_low_i16x8(x)),
     (native::extend_low(V<i16>::from_native(x)).to_native()))

PAIR(i16x8_extmul_low, v128_t, (v128_t x, v128_t y), (wasm_i32x4_extmul_low_i16x8(x, y)),
     (native::multiply_widened_low(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i16x8_extend_high, v128_t, (v128_t x), (wasm_i32x4_extend_high_i16x8(x)),
     (native::extend_high(V<i16>::from_native(x)).to_native()))

PAIR(i16x8_extmul_high, v128_t, (v128_t x, v128_t y), (wasm_i32x4_extmul_high_i16x8(x, y)),
     (native::multiply_widened_high(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i16x8_load_widened, v128_t, (i16 const *p), (wasm_i32x4_load16x4(p)),
     (native::load_widened<i32, i16, architecture>(p).to_native()))

PAIR(u16x8_add, v128_t, (v128_t x, v128_t y), (wasm_i16x8_add(x, y)),
     ((V<u16>::from_native(x) + V<u16>::from_native(y)).to_native()))

PAIR(u16x8_sub, v128_t, (v128_t x, v128_t y), (wasm_i16x8_sub(x, y)),
     ((V<u16>::from_native(x) - V<u16>::from_native(y)).to_native()))

PAIR(u16x8_mul, v128_t, (v128_t x, v128_t y), (wasm_i16x8_mul(x, y)),
     ((V<u16>::from_native(x) * V<u16>::from_native(y)).to_native()))

PAIR(u16x8_eq, v128_t, (v128_t x, v128_t y), (wasm_i16x8_eq(x, y)),
     ((V<u16>::from_native(x) == V<u16>::from_native(y)).to_native()))

PAIR(u16x8_ne, v128_t, (v128_t x, v128_t y), (wasm_i16x8_ne(x, y)),
     ((V<u16>::from_native(x) != V<u16>::from_native(y)).to_native()))

PAIR(u16x8_lt, v128_t, (v128_t x, v128_t y), (wasm_u16x8_lt(x, y)),
     ((V<u16>::from_native(x) < V<u16>::from_native(y)).to_native()))

PAIR(u16x8_le, v128_t, (v128_t x, v128_t y), (wasm_u16x8_le(x, y)),
     ((V<u16>::from_native(x) <= V<u16>::from_native(y)).to_native()))

PAIR(u16x8_gt, v128_t, (v128_t x, v128_t y), (wasm_u16x8_gt(x, y)),
     ((V<u16>::from_native(x) > V<u16>::from_native(y)).to_native()))

PAIR(u16x8_ge, v128_t, (v128_t x, v128_t y), (wasm_u16x8_ge(x, y)),
     ((V<u16>::from_native(x) >= V<u16>::from_native(y)).to_native()))

PAIR(u16x8_min, v128_t, (v128_t x, v128_t y), (wasm_u16x8_min(x, y)),
     (native::min(V<u16>::from_native(x), V<u16>::from_native(y)).to_native()))

PAIR(u16x8_max, v128_t, (v128_t x, v128_t y), (wasm_u16x8_max(x, y)),
     (native::max(V<u16>::from_native(x), V<u16>::from_native(y)).to_native()))

PAIR(u16x8_splat, v128_t, (u16 x), (wasm_i16x8_splat(x)), (V<u16>(x).to_native()))

PAIR(u16x8_extract, u16, (v128_t x), (wasm_u16x8_extract_lane(x, 1)),
     (V<u16>::from_native(x).get<1>()))

PAIR(u16x8_replace, v128_t, (v128_t x, u16 y), (wasm_i16x8_replace_lane(x, 1, y)),
     (V<u16>::from_native(x).replace<1>(y).to_native()))

PAIR(u16x8_shl, v128_t, (v128_t x, unsigned count), (wasm_i16x8_shl(x, count)),
     ((V<u16>::from_native(x) << count).to_native()))

PAIR(u16x8_shr, v128_t, (v128_t x, unsigned count), (wasm_u16x8_shr(x, count)),
     ((V<u16>::from_native(x) >> count).to_native()))

PAIR(u16x8_bitmask, unsigned, (v128_t x), (wasm_i16x8_bitmask(x)),
     (native::bitmask(V<u16>::from_native(x))))

PAIR(u16x8_all_true, bool, (v128_t x), (wasm_i16x8_all_true(x)),
     (native::all(V<u16>::from_native(x))))

PAIR(u16x8_add_sat, v128_t, (v128_t x, v128_t y), (wasm_u16x8_add_sat(x, y)),
     (native::add_sat(V<u16>::from_native(x), V<u16>::from_native(y)).to_native()))

PAIR(u16x8_sub_sat, v128_t, (v128_t x, v128_t y), (wasm_u16x8_sub_sat(x, y)),
     (native::sub_sat(V<u16>::from_native(x), V<u16>::from_native(y)).to_native()))

PAIR(u16x8_avgr, v128_t, (v128_t x, v128_t y), (wasm_u16x8_avgr(x, y)),
     (native::average_round(V<u16>::from_native(x), V<u16>::from_native(y)).to_native()))

PAIR(u16x8_pairwise, v128_t, (v128_t x), (wasm_u32x4_extadd_pairwise_u16x8(x)),
     (native::pairwise_add_widened(V<u16>::from_native(x)).to_native()))

PAIR(u16x8_extend_low, v128_t, (v128_t x), (wasm_u32x4_extend_low_u16x8(x)),
     (native::extend_low(V<u16>::from_native(x)).to_native()))

PAIR(u16x8_extmul_low, v128_t, (v128_t x, v128_t y), (wasm_u32x4_extmul_low_u16x8(x, y)),
     (native::multiply_widened_low(V<u16>::from_native(x), V<u16>::from_native(y)).to_native()))

PAIR(u16x8_extend_high, v128_t, (v128_t x), (wasm_u32x4_extend_high_u16x8(x)),
     (native::extend_high(V<u16>::from_native(x)).to_native()))

PAIR(u16x8_extmul_high, v128_t, (v128_t x, v128_t y), (wasm_u32x4_extmul_high_u16x8(x, y)),
     (native::multiply_widened_high(V<u16>::from_native(x), V<u16>::from_native(y)).to_native()))

PAIR(u16x8_load_widened, v128_t, (u16 const *p), (wasm_u32x4_load16x4(p)),
     (native::load_widened<u32, u16, architecture>(p).to_native()))

PAIR(i32x4_add, v128_t, (v128_t x, v128_t y), (wasm_i32x4_add(x, y)),
     ((V<i32>::from_native(x) + V<i32>::from_native(y)).to_native()))

PAIR(i32x4_sub, v128_t, (v128_t x, v128_t y), (wasm_i32x4_sub(x, y)),
     ((V<i32>::from_native(x) - V<i32>::from_native(y)).to_native()))

PAIR(i32x4_mul, v128_t, (v128_t x, v128_t y), (wasm_i32x4_mul(x, y)),
     ((V<i32>::from_native(x) * V<i32>::from_native(y)).to_native()))

PAIR(i32x4_eq, v128_t, (v128_t x, v128_t y), (wasm_i32x4_eq(x, y)),
     ((V<i32>::from_native(x) == V<i32>::from_native(y)).to_native()))

PAIR(i32x4_ne, v128_t, (v128_t x, v128_t y), (wasm_i32x4_ne(x, y)),
     ((V<i32>::from_native(x) != V<i32>::from_native(y)).to_native()))

PAIR(i32x4_lt, v128_t, (v128_t x, v128_t y), (wasm_i32x4_lt(x, y)),
     ((V<i32>::from_native(x) < V<i32>::from_native(y)).to_native()))

PAIR(i32x4_le, v128_t, (v128_t x, v128_t y), (wasm_i32x4_le(x, y)),
     ((V<i32>::from_native(x) <= V<i32>::from_native(y)).to_native()))

PAIR(i32x4_gt, v128_t, (v128_t x, v128_t y), (wasm_i32x4_gt(x, y)),
     ((V<i32>::from_native(x) > V<i32>::from_native(y)).to_native()))

PAIR(i32x4_ge, v128_t, (v128_t x, v128_t y), (wasm_i32x4_ge(x, y)),
     ((V<i32>::from_native(x) >= V<i32>::from_native(y)).to_native()))

PAIR(i32x4_min, v128_t, (v128_t x, v128_t y), (wasm_i32x4_min(x, y)),
     (native::min(V<i32>::from_native(x), V<i32>::from_native(y)).to_native()))

PAIR(i32x4_max, v128_t, (v128_t x, v128_t y), (wasm_i32x4_max(x, y)),
     (native::max(V<i32>::from_native(x), V<i32>::from_native(y)).to_native()))

PAIR(i32x4_abs, v128_t, (v128_t x), (wasm_i32x4_abs(x)),
     (native::abs(V<i32>::from_native(x)).to_native()))

PAIR(i32x4_neg, v128_t, (v128_t x), (wasm_i32x4_neg(x)), ((-V<i32>::from_native(x)).to_native()))

PAIR(i32x4_splat, v128_t, (i32 x), (wasm_i32x4_splat(x)), (V<i32>(x).to_native()))

PAIR(i32x4_extract, i32, (v128_t x), (wasm_i32x4_extract_lane(x, 1)),
     (V<i32>::from_native(x).get<1>()))

PAIR(i32x4_replace, v128_t, (v128_t x, i32 y), (wasm_i32x4_replace_lane(x, 1, y)),
     (V<i32>::from_native(x).replace<1>(y).to_native()))

PAIR(i32x4_shl, v128_t, (v128_t x, unsigned count), (wasm_i32x4_shl(x, count)),
     ((V<i32>::from_native(x) << count).to_native()))

PAIR(i32x4_shr, v128_t, (v128_t x, unsigned count), (wasm_i32x4_shr(x, count)),
     ((V<i32>::from_native(x) >> count).to_native()))

PAIR(i32x4_bitmask, unsigned, (v128_t x), (wasm_i32x4_bitmask(x)),
     (native::bitmask(V<i32>::from_native(x))))

PAIR(i32x4_all_true, bool, (v128_t x), (wasm_i32x4_all_true(x)),
     (native::all(V<i32>::from_native(x))))

PAIR(i32x4_extend_low, v128_t, (v128_t x), (wasm_i64x2_extend_low_i32x4(x)),
     (native::extend_low(V<i32>::from_native(x)).to_native()))

PAIR(i32x4_extmul_low, v128_t, (v128_t x, v128_t y), (wasm_i64x2_extmul_low_i32x4(x, y)),
     (native::multiply_widened_low(V<i32>::from_native(x), V<i32>::from_native(y)).to_native()))

PAIR(i32x4_extend_high, v128_t, (v128_t x), (wasm_i64x2_extend_high_i32x4(x)),
     (native::extend_high(V<i32>::from_native(x)).to_native()))

PAIR(i32x4_extmul_high, v128_t, (v128_t x, v128_t y), (wasm_i64x2_extmul_high_i32x4(x, y)),
     (native::multiply_widened_high(V<i32>::from_native(x), V<i32>::from_native(y)).to_native()))

PAIR(i32x4_load_widened, v128_t, (i32 const *p), (wasm_i64x2_load32x2(p)),
     (native::load_widened<i64, i32, architecture>(p).to_native()))

PAIR(u32x4_add, v128_t, (v128_t x, v128_t y), (wasm_i32x4_add(x, y)),
     ((V<u32>::from_native(x) + V<u32>::from_native(y)).to_native()))

PAIR(u32x4_sub, v128_t, (v128_t x, v128_t y), (wasm_i32x4_sub(x, y)),
     ((V<u32>::from_native(x) - V<u32>::from_native(y)).to_native()))

PAIR(u32x4_mul, v128_t, (v128_t x, v128_t y), (wasm_i32x4_mul(x, y)),
     ((V<u32>::from_native(x) * V<u32>::from_native(y)).to_native()))

PAIR(u32x4_eq, v128_t, (v128_t x, v128_t y), (wasm_i32x4_eq(x, y)),
     ((V<u32>::from_native(x) == V<u32>::from_native(y)).to_native()))

PAIR(u32x4_ne, v128_t, (v128_t x, v128_t y), (wasm_i32x4_ne(x, y)),
     ((V<u32>::from_native(x) != V<u32>::from_native(y)).to_native()))

PAIR(u32x4_lt, v128_t, (v128_t x, v128_t y), (wasm_u32x4_lt(x, y)),
     ((V<u32>::from_native(x) < V<u32>::from_native(y)).to_native()))

PAIR(u32x4_le, v128_t, (v128_t x, v128_t y), (wasm_u32x4_le(x, y)),
     ((V<u32>::from_native(x) <= V<u32>::from_native(y)).to_native()))

PAIR(u32x4_gt, v128_t, (v128_t x, v128_t y), (wasm_u32x4_gt(x, y)),
     ((V<u32>::from_native(x) > V<u32>::from_native(y)).to_native()))

PAIR(u32x4_ge, v128_t, (v128_t x, v128_t y), (wasm_u32x4_ge(x, y)),
     ((V<u32>::from_native(x) >= V<u32>::from_native(y)).to_native()))

PAIR(u32x4_min, v128_t, (v128_t x, v128_t y), (wasm_u32x4_min(x, y)),
     (native::min(V<u32>::from_native(x), V<u32>::from_native(y)).to_native()))

PAIR(u32x4_max, v128_t, (v128_t x, v128_t y), (wasm_u32x4_max(x, y)),
     (native::max(V<u32>::from_native(x), V<u32>::from_native(y)).to_native()))

PAIR(u32x4_splat, v128_t, (u32 x), (wasm_i32x4_splat(x)), (V<u32>(x).to_native()))

PAIR(u32x4_extract, u32, (v128_t x), (wasm_u32x4_extract_lane(x, 1)),
     (V<u32>::from_native(x).get<1>()))

PAIR(u32x4_replace, v128_t, (v128_t x, u32 y), (wasm_i32x4_replace_lane(x, 1, y)),
     (V<u32>::from_native(x).replace<1>(y).to_native()))

PAIR(u32x4_shl, v128_t, (v128_t x, unsigned count), (wasm_i32x4_shl(x, count)),
     ((V<u32>::from_native(x) << count).to_native()))

PAIR(u32x4_shr, v128_t, (v128_t x, unsigned count), (wasm_u32x4_shr(x, count)),
     ((V<u32>::from_native(x) >> count).to_native()))

PAIR(u32x4_bitmask, unsigned, (v128_t x), (wasm_i32x4_bitmask(x)),
     (native::bitmask(V<u32>::from_native(x))))

PAIR(u32x4_all_true, bool, (v128_t x), (wasm_i32x4_all_true(x)),
     (native::all(V<u32>::from_native(x))))

PAIR(u32x4_extend_low, v128_t, (v128_t x), (wasm_u64x2_extend_low_u32x4(x)),
     (native::extend_low(V<u32>::from_native(x)).to_native()))

PAIR(u32x4_extmul_low, v128_t, (v128_t x, v128_t y), (wasm_u64x2_extmul_low_u32x4(x, y)),
     (native::multiply_widened_low(V<u32>::from_native(x), V<u32>::from_native(y)).to_native()))

PAIR(u32x4_extend_high, v128_t, (v128_t x), (wasm_u64x2_extend_high_u32x4(x)),
     (native::extend_high(V<u32>::from_native(x)).to_native()))

PAIR(u32x4_extmul_high, v128_t, (v128_t x, v128_t y), (wasm_u64x2_extmul_high_u32x4(x, y)),
     (native::multiply_widened_high(V<u32>::from_native(x), V<u32>::from_native(y)).to_native()))

PAIR(u32x4_load_widened, v128_t, (u32 const *p), (wasm_u64x2_load32x2(p)),
     (native::load_widened<u64, u32, architecture>(p).to_native()))

PAIR(i64x2_add, v128_t, (v128_t x, v128_t y), (wasm_i64x2_add(x, y)),
     ((V<i64>::from_native(x) + V<i64>::from_native(y)).to_native()))

PAIR(i64x2_sub, v128_t, (v128_t x, v128_t y), (wasm_i64x2_sub(x, y)),
     ((V<i64>::from_native(x) - V<i64>::from_native(y)).to_native()))

PAIR(i64x2_mul, v128_t, (v128_t x, v128_t y), (wasm_i64x2_mul(x, y)),
     ((V<i64>::from_native(x) * V<i64>::from_native(y)).to_native()))

PAIR(i64x2_eq, v128_t, (v128_t x, v128_t y), (wasm_i64x2_eq(x, y)),
     ((V<i64>::from_native(x) == V<i64>::from_native(y)).to_native()))

PAIR(i64x2_ne, v128_t, (v128_t x, v128_t y), (wasm_i64x2_ne(x, y)),
     ((V<i64>::from_native(x) != V<i64>::from_native(y)).to_native()))

PAIR(i64x2_lt, v128_t, (v128_t x, v128_t y), (wasm_i64x2_lt(x, y)),
     ((V<i64>::from_native(x) < V<i64>::from_native(y)).to_native()))

PAIR(i64x2_le, v128_t, (v128_t x, v128_t y), (wasm_i64x2_le(x, y)),
     ((V<i64>::from_native(x) <= V<i64>::from_native(y)).to_native()))

PAIR(i64x2_gt, v128_t, (v128_t x, v128_t y), (wasm_i64x2_gt(x, y)),
     ((V<i64>::from_native(x) > V<i64>::from_native(y)).to_native()))

PAIR(i64x2_ge, v128_t, (v128_t x, v128_t y), (wasm_i64x2_ge(x, y)),
     ((V<i64>::from_native(x) >= V<i64>::from_native(y)).to_native()))

PAIR(i64x2_abs, v128_t, (v128_t x), (wasm_i64x2_abs(x)),
     (native::abs(V<i64>::from_native(x)).to_native()))

PAIR(i64x2_neg, v128_t, (v128_t x), (wasm_i64x2_neg(x)), ((-V<i64>::from_native(x)).to_native()))

PAIR(i64x2_splat, v128_t, (i64 x), (wasm_i64x2_splat(x)), (V<i64>(x).to_native()))

PAIR(i64x2_extract, i64, (v128_t x), (wasm_i64x2_extract_lane(x, 1)),
     (V<i64>::from_native(x).get<1>()))

PAIR(i64x2_replace, v128_t, (v128_t x, i64 y), (wasm_i64x2_replace_lane(x, 1, y)),
     (V<i64>::from_native(x).replace<1>(y).to_native()))

PAIR(i64x2_shl, v128_t, (v128_t x, unsigned count), (wasm_i64x2_shl(x, count)),
     ((V<i64>::from_native(x) << count).to_native()))

PAIR(i64x2_shr, v128_t, (v128_t x, unsigned count), (wasm_i64x2_shr(x, count)),
     ((V<i64>::from_native(x) >> count).to_native()))

PAIR(i64x2_bitmask, unsigned, (v128_t x), (wasm_i64x2_bitmask(x)),
     (native::bitmask(V<i64>::from_native(x))))

PAIR(i64x2_all_true, bool, (v128_t x), (wasm_i64x2_all_true(x)),
     (native::all(V<i64>::from_native(x))))

PAIR(u64x2_add, v128_t, (v128_t x, v128_t y), (wasm_i64x2_add(x, y)),
     ((V<u64>::from_native(x) + V<u64>::from_native(y)).to_native()))

PAIR(u64x2_sub, v128_t, (v128_t x, v128_t y), (wasm_i64x2_sub(x, y)),
     ((V<u64>::from_native(x) - V<u64>::from_native(y)).to_native()))

PAIR(u64x2_mul, v128_t, (v128_t x, v128_t y), (wasm_i64x2_mul(x, y)),
     ((V<u64>::from_native(x) * V<u64>::from_native(y)).to_native()))

PAIR(u64x2_eq, v128_t, (v128_t x, v128_t y), (wasm_i64x2_eq(x, y)),
     ((V<u64>::from_native(x) == V<u64>::from_native(y)).to_native()))

PAIR(u64x2_ne, v128_t, (v128_t x, v128_t y), (wasm_i64x2_ne(x, y)),
     ((V<u64>::from_native(x) != V<u64>::from_native(y)).to_native()))

PAIR(u64x2_splat, v128_t, (u64 x), (wasm_i64x2_splat(x)), (V<u64>(x).to_native()))

PAIR(u64x2_extract, u64, (v128_t x), (wasm_u64x2_extract_lane(x, 1)),
     (V<u64>::from_native(x).get<1>()))

PAIR(u64x2_replace, v128_t, (v128_t x, u64 y), (wasm_i64x2_replace_lane(x, 1, y)),
     (V<u64>::from_native(x).replace<1>(y).to_native()))

PAIR(u64x2_shl, v128_t, (v128_t x, unsigned count), (wasm_i64x2_shl(x, count)),
     ((V<u64>::from_native(x) << count).to_native()))

PAIR(u64x2_shr, v128_t, (v128_t x, unsigned count), (wasm_u64x2_shr(x, count)),
     ((V<u64>::from_native(x) >> count).to_native()))

PAIR(u64x2_bitmask, unsigned, (v128_t x), (wasm_i64x2_bitmask(x)),
     (native::bitmask(V<u64>::from_native(x))))

PAIR(u64x2_all_true, bool, (v128_t x), (wasm_i64x2_all_true(x)),
     (native::all(V<u64>::from_native(x))))

PAIR(f32x4_add, v128_t, (v128_t x, v128_t y), (wasm_f32x4_add(x, y)),
     ((V<float>::from_native(x) + V<float>::from_native(y)).to_native()))

PAIR(f32x4_sub, v128_t, (v128_t x, v128_t y), (wasm_f32x4_sub(x, y)),
     ((V<float>::from_native(x) - V<float>::from_native(y)).to_native()))

PAIR(f32x4_mul, v128_t, (v128_t x, v128_t y), (wasm_f32x4_mul(x, y)),
     ((V<float>::from_native(x) * V<float>::from_native(y)).to_native()))

PAIR(f32x4_div, v128_t, (v128_t x, v128_t y), (wasm_f32x4_div(x, y)),
     ((V<float>::from_native(x) / V<float>::from_native(y)).to_native()))

PAIR(f32x4_eq, v128_t, (v128_t x, v128_t y), (wasm_f32x4_eq(x, y)),
     ((V<float>::from_native(x) == V<float>::from_native(y)).to_native()))

PAIR(f32x4_ne, v128_t, (v128_t x, v128_t y), (wasm_f32x4_ne(x, y)),
     ((V<float>::from_native(x) != V<float>::from_native(y)).to_native()))

PAIR(f32x4_lt, v128_t, (v128_t x, v128_t y), (wasm_f32x4_lt(x, y)),
     ((V<float>::from_native(x) < V<float>::from_native(y)).to_native()))

PAIR(f32x4_le, v128_t, (v128_t x, v128_t y), (wasm_f32x4_le(x, y)),
     ((V<float>::from_native(x) <= V<float>::from_native(y)).to_native()))

PAIR(f32x4_gt, v128_t, (v128_t x, v128_t y), (wasm_f32x4_gt(x, y)),
     ((V<float>::from_native(x) > V<float>::from_native(y)).to_native()))

PAIR(f32x4_ge, v128_t, (v128_t x, v128_t y), (wasm_f32x4_ge(x, y)),
     ((V<float>::from_native(x) >= V<float>::from_native(y)).to_native()))

PAIR(f32x4_min, v128_t, (v128_t x, v128_t y), (wasm_f32x4_min(x, y)),
     (native::min(V<float>::from_native(x), V<float>::from_native(y)).to_native()))

PAIR(f32x4_max, v128_t, (v128_t x, v128_t y), (wasm_f32x4_max(x, y)),
     (native::max(V<float>::from_native(x), V<float>::from_native(y)).to_native()))

PAIR(f32x4_abs, v128_t, (v128_t x), (wasm_f32x4_abs(x)),
     (native::abs(V<float>::from_native(x)).to_native()))

PAIR(f32x4_neg, v128_t, (v128_t x), (wasm_f32x4_neg(x)), ((-V<float>::from_native(x)).to_native()))

PAIR(f32x4_splat, v128_t, (float x), (wasm_f32x4_splat(x)), (V<float>(x).to_native()))

PAIR(f32x4_extract, float, (v128_t x), (wasm_f32x4_extract_lane(x, 1)),
     (V<float>::from_native(x).get<1>()))

PAIR(f32x4_replace, v128_t, (v128_t x, float y), (wasm_f32x4_replace_lane(x, 1, y)),
     (V<float>::from_native(x).replace<1>(y).to_native()))

PAIR(f32x4_sqrt, v128_t, (v128_t x), (wasm_f32x4_sqrt(x)),
     (native::sqrt(V<float>::from_native(x)).to_native()))

PAIR(f32x4_ceil, v128_t, (v128_t x), (wasm_f32x4_ceil(x)),
     (native::ceil(V<float>::from_native(x)).to_native()))

PAIR(f32x4_floor, v128_t, (v128_t x), (wasm_f32x4_floor(x)),
     (native::floor(V<float>::from_native(x)).to_native()))

PAIR(f32x4_trunc, v128_t, (v128_t x), (wasm_f32x4_trunc(x)),
     (native::trunc(V<float>::from_native(x)).to_native()))

PAIR(f32x4_nearest, v128_t, (v128_t x), (wasm_f32x4_nearest(x)),
     (native::round_even(V<float>::from_native(x)).to_native()))

PAIR(f32x4_pmin, v128_t, (v128_t x, v128_t y), (wasm_f32x4_pmin(x, y)),
     (native::pmin(V<float>::from_native(x), V<float>::from_native(y)).to_native()))

PAIR(f32x4_pmax, v128_t, (v128_t x, v128_t y), (wasm_f32x4_pmax(x, y)),
     (native::pmax(V<float>::from_native(x), V<float>::from_native(y)).to_native()))

PAIR(f64x2_add, v128_t, (v128_t x, v128_t y), (wasm_f64x2_add(x, y)),
     ((V<double>::from_native(x) + V<double>::from_native(y)).to_native()))

PAIR(f64x2_sub, v128_t, (v128_t x, v128_t y), (wasm_f64x2_sub(x, y)),
     ((V<double>::from_native(x) - V<double>::from_native(y)).to_native()))

PAIR(f64x2_mul, v128_t, (v128_t x, v128_t y), (wasm_f64x2_mul(x, y)),
     ((V<double>::from_native(x) * V<double>::from_native(y)).to_native()))

PAIR(f64x2_div, v128_t, (v128_t x, v128_t y), (wasm_f64x2_div(x, y)),
     ((V<double>::from_native(x) / V<double>::from_native(y)).to_native()))

PAIR(f64x2_eq, v128_t, (v128_t x, v128_t y), (wasm_f64x2_eq(x, y)),
     ((V<double>::from_native(x) == V<double>::from_native(y)).to_native()))

PAIR(f64x2_ne, v128_t, (v128_t x, v128_t y), (wasm_f64x2_ne(x, y)),
     ((V<double>::from_native(x) != V<double>::from_native(y)).to_native()))

PAIR(f64x2_lt, v128_t, (v128_t x, v128_t y), (wasm_f64x2_lt(x, y)),
     ((V<double>::from_native(x) < V<double>::from_native(y)).to_native()))

PAIR(f64x2_le, v128_t, (v128_t x, v128_t y), (wasm_f64x2_le(x, y)),
     ((V<double>::from_native(x) <= V<double>::from_native(y)).to_native()))

PAIR(f64x2_gt, v128_t, (v128_t x, v128_t y), (wasm_f64x2_gt(x, y)),
     ((V<double>::from_native(x) > V<double>::from_native(y)).to_native()))

PAIR(f64x2_ge, v128_t, (v128_t x, v128_t y), (wasm_f64x2_ge(x, y)),
     ((V<double>::from_native(x) >= V<double>::from_native(y)).to_native()))

PAIR(f64x2_min, v128_t, (v128_t x, v128_t y), (wasm_f64x2_min(x, y)),
     (native::min(V<double>::from_native(x), V<double>::from_native(y)).to_native()))

PAIR(f64x2_max, v128_t, (v128_t x, v128_t y), (wasm_f64x2_max(x, y)),
     (native::max(V<double>::from_native(x), V<double>::from_native(y)).to_native()))

PAIR(f64x2_abs, v128_t, (v128_t x), (wasm_f64x2_abs(x)),
     (native::abs(V<double>::from_native(x)).to_native()))

PAIR(f64x2_neg, v128_t, (v128_t x), (wasm_f64x2_neg(x)), ((-V<double>::from_native(x)).to_native()))

PAIR(f64x2_splat, v128_t, (double x), (wasm_f64x2_splat(x)), (V<double>(x).to_native()))

PAIR(f64x2_extract, double, (v128_t x), (wasm_f64x2_extract_lane(x, 1)),
     (V<double>::from_native(x).get<1>()))

PAIR(f64x2_replace, v128_t, (v128_t x, double y), (wasm_f64x2_replace_lane(x, 1, y)),
     (V<double>::from_native(x).replace<1>(y).to_native()))

PAIR(f64x2_sqrt, v128_t, (v128_t x), (wasm_f64x2_sqrt(x)),
     (native::sqrt(V<double>::from_native(x)).to_native()))

PAIR(f64x2_ceil, v128_t, (v128_t x), (wasm_f64x2_ceil(x)),
     (native::ceil(V<double>::from_native(x)).to_native()))

PAIR(f64x2_floor, v128_t, (v128_t x), (wasm_f64x2_floor(x)),
     (native::floor(V<double>::from_native(x)).to_native()))

PAIR(f64x2_trunc, v128_t, (v128_t x), (wasm_f64x2_trunc(x)),
     (native::trunc(V<double>::from_native(x)).to_native()))

PAIR(f64x2_nearest, v128_t, (v128_t x), (wasm_f64x2_nearest(x)),
     (native::round_even(V<double>::from_native(x)).to_native()))

PAIR(f64x2_pmin, v128_t, (v128_t x, v128_t y), (wasm_f64x2_pmin(x, y)),
     (native::pmin(V<double>::from_native(x), V<double>::from_native(y)).to_native()))

PAIR(f64x2_pmax, v128_t, (v128_t x, v128_t y), (wasm_f64x2_pmax(x, y)),
     (native::pmax(V<double>::from_native(x), V<double>::from_native(y)).to_native()))

PAIR(v128_and, v128_t, (v128_t x, v128_t y), (wasm_v128_and(x, y)),
     ((V<u32>::from_native(x) & V<u32>::from_native(y)).to_native()))

PAIR(v128_or, v128_t, (v128_t x, v128_t y), (wasm_v128_or(x, y)),
     ((V<u32>::from_native(x) | V<u32>::from_native(y)).to_native()))

PAIR(v128_xor, v128_t, (v128_t x, v128_t y), (wasm_v128_xor(x, y)),
     ((V<u32>::from_native(x) ^ V<u32>::from_native(y)).to_native()))

PAIR(v128_andnot, v128_t, (v128_t x, v128_t y), (wasm_v128_andnot(x, y)),
     ((V<u32>::from_native(x) & ~V<u32>::from_native(y)).to_native()))

PAIR(v128_not, v128_t, (v128_t x), (wasm_v128_not(x)), ((~V<u32>::from_native(x)).to_native()))

PAIR(v128_bitselect, v128_t, (v128_t x, v128_t y, v128_t m), (wasm_v128_bitselect(x, y, m)),
     (native::bit_select(V<u32>::from_native(m), V<u32>::from_native(x), V<u32>::from_native(y))
          .to_native()))

PAIR(v128_any_true, bool, (v128_t x), (wasm_v128_any_true(x)), (native::any(V<u8>::from_native(x))))

PAIR(v128_load, v128_t, (u8 const *p), (wasm_v128_load(p)), (V<u8>::load(p).to_native()))

PAIR(v128_store, void, (u8 * p, v128_t x), (wasm_v128_store(p, x)),
     (V<u8>::from_native(x).store(p)))

PAIR(load8_splat, v128_t, (u8 const *p), (wasm_v128_load8_splat(p)),
     (native::load_splat<V<u8>>(p).to_native()))

PAIR(load8_lane, v128_t, (u8 const *p, v128_t x), (wasm_v128_load8_lane(p, x, 1)),
     (native::load_lane<1>(p, V<u8>::from_native(x)).to_native()))

PAIR(store8_lane, void, (u8 * p, v128_t x), (wasm_v128_store8_lane(p, x, 1)),
     (native::store_lane<1>(p, V<u8>::from_native(x))))

PAIR(load16_splat, v128_t, (u16 const *p), (wasm_v128_load16_splat(__builtin_assume_aligned(p, 2))),
     (native::load_splat<V<u16>>(p).to_native()))

PAIR(load16_lane, v128_t, (u16 const *p, v128_t x),
     (wasm_v128_load16_lane(__builtin_assume_aligned(p, 2), x, 1)),
     (native::load_lane<1>(p, V<u16>::from_native(x)).to_native()))

PAIR(store16_lane, void, (u16 * p, v128_t x),
     (wasm_v128_store16_lane(__builtin_assume_aligned(p, 2), x, 1)),
     (native::store_lane<1>(p, V<u16>::from_native(x))))

PAIR(load32_splat, v128_t, (u32 const *p), (wasm_v128_load32_splat(__builtin_assume_aligned(p, 4))),
     (native::load_splat<V<u32>>(p).to_native()))

PAIR(load32_lane, v128_t, (u32 const *p, v128_t x),
     (wasm_v128_load32_lane(__builtin_assume_aligned(p, 4), x, 1)),
     (native::load_lane<1>(p, V<u32>::from_native(x)).to_native()))

PAIR(store32_lane, void, (u32 * p, v128_t x),
     (wasm_v128_store32_lane(__builtin_assume_aligned(p, 4), x, 1)),
     (native::store_lane<1>(p, V<u32>::from_native(x))))

PAIR(load32_zero, v128_t, (u32 const *p), (wasm_v128_load32_zero(p)),
     (native::load_zero<V<u32>>(p).to_native()))

PAIR(load64_splat, v128_t, (u64 const *p), (wasm_v128_load64_splat(__builtin_assume_aligned(p, 8))),
     (native::load_splat<V<u64>>(p).to_native()))

PAIR(load64_lane, v128_t, (u64 const *p, v128_t x),
     (wasm_v128_load64_lane(__builtin_assume_aligned(p, 8), x, 1)),
     (native::load_lane<1>(p, V<u64>::from_native(x)).to_native()))

PAIR(store64_lane, void, (u64 * p, v128_t x),
     (wasm_v128_store64_lane(__builtin_assume_aligned(p, 8), x, 1)),
     (native::store_lane<1>(p, V<u64>::from_native(x))))

PAIR(load64_zero, v128_t, (u64 const *p), (wasm_v128_load64_zero(p)),
     (native::load_zero<V<u64>>(p).to_native()))

PAIR(i8x16_narrow, v128_t, (v128_t x, v128_t y), (wasm_i8x16_narrow_i16x8(x, y)),
     (native::narrow_sat<i8>(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(u8x16_narrow, v128_t, (v128_t x, v128_t y), (wasm_u8x16_narrow_i16x8(x, y)),
     (native::narrow_sat<u8>(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i16x8_narrow, v128_t, (v128_t x, v128_t y), (wasm_i16x8_narrow_i32x4(x, y)),
     (native::narrow_sat<i16>(V<i32>::from_native(x), V<i32>::from_native(y)).to_native()))

PAIR(u16x8_narrow, v128_t, (v128_t x, v128_t y), (wasm_u16x8_narrow_i32x4(x, y)),
     (native::narrow_sat<u16>(V<i32>::from_native(x), V<i32>::from_native(y)).to_native()))

PAIR(i16x8_q15mulr_sat, v128_t, (v128_t x, v128_t y), (wasm_i16x8_q15mulr_sat(x, y)),
     (native::q15mulr_sat(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i32x4_dot, v128_t, (v128_t x, v128_t y), (wasm_i32x4_dot_i16x8(x, y)),
     (native::dot(V<i16>::from_native(x), V<i16>::from_native(y)).to_native()))

PAIR(i8x16_swizzle, v128_t, (v128_t x, v128_t y), (wasm_i8x16_swizzle(x, y)),
     (native::swizzle(V<u8>::from_native(x), V<u8>::from_native(y)).to_native()))

PAIR(i8x16_shuffle, v128_t, (v128_t x, v128_t y),
     (wasm_i8x16_shuffle(x, y, 0, 17, 2, 19, 4, 21, 6, 23, 8, 25, 10, 27, 12, 29, 14, 31)),
     (native::shuffle<0, 17, 2, 19, 4, 21, 6, 23, 8, 25, 10, 27, 12, 29, 14, 31>(
          V<u8>::from_native(x), V<u8>::from_native(y))
          .to_native()))

PAIR(i8x16_popcnt, v128_t, (v128_t x), (wasm_i8x16_popcnt(x)),
     (native::popcount(V<u8>::from_native(x)).to_native()))

PAIR(i32_trunc_f32x4, v128_t, (v128_t x), (wasm_i32x4_trunc_sat_f32x4(x)),
     (native::trunc_sat<i32>(V<float>::from_native(x)).to_native()))

PAIR(u32_trunc_f32x4, v128_t, (v128_t x), (wasm_u32x4_trunc_sat_f32x4(x)),
     (native::trunc_sat<u32>(V<float>::from_native(x)).to_native()))

PAIR(i32_trunc_f64x2, v128_t, (v128_t x), (wasm_i32x4_trunc_sat_f64x2_zero(x)),
     (native::trunc_sat<i32>(V<double>::from_native(x)).to_native()))

PAIR(u32_trunc_f64x2, v128_t, (v128_t x), (wasm_u32x4_trunc_sat_f64x2_zero(x)),
     (native::trunc_sat<u32>(V<double>::from_native(x)).to_native()))

PAIR(f32x4_convert_i32, v128_t, (v128_t x), (wasm_f32x4_convert_i32x4(x)),
     (native::convert<float>(V<i32>::from_native(x)).to_native()))

PAIR(f64x2_convert_i32, v128_t, (v128_t x), (wasm_f64x2_convert_low_i32x4(x)),
     (native::convert<double>(V<i32>::from_native(x)).to_native()))

PAIR(f32x4_convert_u32, v128_t, (v128_t x), (wasm_f32x4_convert_u32x4(x)),
     (native::convert<float>(V<u32>::from_native(x)).to_native()))

PAIR(f64x2_convert_u32, v128_t, (v128_t x), (wasm_f64x2_convert_low_u32x4(x)),
     (native::convert<double>(V<u32>::from_native(x)).to_native()))

PAIR(f32x4_demote, v128_t, (v128_t x), (wasm_f32x4_demote_f64x2_zero(x)),
     (native::convert<float>(V<double>::from_native(x)).to_native()))

PAIR(f64x2_promote, v128_t, (v128_t x), (wasm_f64x2_promote_low_f32x4(x)),
     (native::convert<double>(V<float>::from_native(x)).to_native()))

PAIR(mask8_all, bool, (v128_t x), (wasm_i8x16_all_true(x)),
     (all(native::simd<native::mask_lane<u8>, 16, architecture>::unsafe_from_native(x))))

PAIR(mask8_any, bool, (v128_t x), (wasm_v128_any_true(x)),
     (any(native::simd<native::mask_lane<u8>, 16, architecture>::unsafe_from_native(x))))

PAIR(mask8_bitset, std::uint64_t, (v128_t x), (wasm_i8x16_bitmask(x)),
     (native::simd<native::mask_lane<u8>, 16, architecture>::unsafe_from_native(x).to_bitset()))

PAIR(mask8_select, v128_t, (v128_t x, v128_t y, v128_t m), (wasm_v128_bitselect(x, y, m)),
     (native::select(native::simd<native::mask_lane<u8>, 16, architecture>::unsafe_from_native(m),
                     V<u8>::from_native(x), V<u8>::from_native(y))
          .to_native()))

PAIR(mask16_all, bool, (v128_t x), (wasm_i16x8_all_true(x)),
     (all(native::simd<native::mask_lane<u16>, 8, architecture>::unsafe_from_native(x))))

PAIR(mask16_any, bool, (v128_t x), (wasm_v128_any_true(x)),
     (any(native::simd<native::mask_lane<u16>, 8, architecture>::unsafe_from_native(x))))

PAIR(mask16_bitset, std::uint64_t, (v128_t x), (wasm_i16x8_bitmask(x)),
     (native::simd<native::mask_lane<u16>, 8, architecture>::unsafe_from_native(x).to_bitset()))

PAIR(mask16_select, v128_t, (v128_t x, v128_t y, v128_t m), (wasm_v128_bitselect(x, y, m)),
     (native::select(native::simd<native::mask_lane<u16>, 8, architecture>::unsafe_from_native(m),
                     V<u16>::from_native(x), V<u16>::from_native(y))
          .to_native()))

PAIR(mask32_all, bool, (v128_t x), (wasm_i32x4_all_true(x)),
     (all(native::simd<native::mask_lane<u32>, 4, architecture>::unsafe_from_native(x))))

PAIR(mask32_any, bool, (v128_t x), (wasm_v128_any_true(x)),
     (any(native::simd<native::mask_lane<u32>, 4, architecture>::unsafe_from_native(x))))

PAIR(mask32_bitset, std::uint64_t, (v128_t x), (wasm_i32x4_bitmask(x)),
     (native::simd<native::mask_lane<u32>, 4, architecture>::unsafe_from_native(x).to_bitset()))

PAIR(mask32_select, v128_t, (v128_t x, v128_t y, v128_t m), (wasm_v128_bitselect(x, y, m)),
     (native::select(native::simd<native::mask_lane<u32>, 4, architecture>::unsafe_from_native(m),
                     V<u32>::from_native(x), V<u32>::from_native(y))
          .to_native()))

PAIR(mask64_all, bool, (v128_t x), (wasm_i64x2_all_true(x)),
     (all(native::simd<native::mask_lane<u64>, 2, architecture>::unsafe_from_native(x))))

PAIR(mask64_any, bool, (v128_t x), (wasm_v128_any_true(x)),
     (any(native::simd<native::mask_lane<u64>, 2, architecture>::unsafe_from_native(x))))

PAIR(mask64_bitset, std::uint64_t, (v128_t x), (wasm_i64x2_bitmask(x)),
     (native::simd<native::mask_lane<u64>, 2, architecture>::unsafe_from_native(x).to_bitset()))

PAIR(mask64_select, v128_t, (v128_t x, v128_t y, v128_t m), (wasm_v128_bitselect(x, y, m)),
     (native::select(native::simd<native::mask_lane<u64>, 2, architecture>::unsafe_from_native(m),
                     V<u64>::from_native(x), V<u64>::from_native(y))
          .to_native()))

PAIR(v128_constant, v128_t, (), (wasm_i32x4_const(1, 2, 3, 4)), (V<i32>{1, 2, 3, 4}.to_native()))

PAIR(v128_zero, v128_t, (), (wasm_i32x4_const(0, 0, 0, 0)), (V<i32>{}.to_native()))

#undef PAIR
