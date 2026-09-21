// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <wasm_simd128.h>
#if NATIVE_REJECT_PRIVATE
#include "native/wasm/relaxed.h"
#else
import native.wasm.relaxed;
#endif

#if (NATIVE_REJECT_WEAK || NATIVE_REJECT_PRIVATE) && !NATIVE_POSITIVE
constexpr auto profile = native::feature_closure(native::wasm_feature::simd128);
#else
constexpr auto profile = native::feature_closure(native::wasm_feature::relaxed_simd);
#endif

#if NATIVE_REJECT_TARGET
__attribute__((target("simd128")))
#else
__attribute__((target("relaxed-simd")))
#endif
v128_t probe(v128_t a, v128_t b, v128_t c) {
#if NATIVE_RELAXED_OP == 0
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i8x16_relaxed_swizzle<profile>(a, b);
#elif NATIVE_REJECT_SHAPE
  return native::i8x16_relaxed_swizzle(a, b);
#else
  return native::i8x16_relaxed_swizzle(native::simd<std::uint8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 1
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i32x4_relaxed_trunc_f32x4<profile>(a);
#elif NATIVE_REJECT_SHAPE
  return native::i32x4_relaxed_trunc_f32x4(a);
#else
  return native::i32x4_relaxed_trunc_f32x4(native::simd<float, 4, profile>::from_native(a)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 2
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::u32x4_relaxed_trunc_f32x4<profile>(a);
#elif NATIVE_REJECT_SHAPE
  return native::u32x4_relaxed_trunc_f32x4(a);
#else
  return native::u32x4_relaxed_trunc_f32x4(native::simd<float, 4, profile>::from_native(a)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 3
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i32x4_relaxed_trunc_f64x2_zero<profile>(a);
#elif NATIVE_REJECT_SHAPE
  return native::i32x4_relaxed_trunc_f64x2_zero(a);
#else
  return native::i32x4_relaxed_trunc_f64x2_zero(native::simd<double, 2, profile>::from_native(a)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 4
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::u32x4_relaxed_trunc_f64x2_zero<profile>(a);
#elif NATIVE_REJECT_SHAPE
  return native::u32x4_relaxed_trunc_f64x2_zero(a);
#else
  return native::u32x4_relaxed_trunc_f64x2_zero(native::simd<double, 2, profile>::from_native(a)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 5
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f32x4_relaxed_madd<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::f32x4_relaxed_madd(a, b, c);
#else
  return native::f32x4_relaxed_madd(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b), native::simd<float, 4, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 6
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f32x4_relaxed_nmadd<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::f32x4_relaxed_nmadd(a, b, c);
#else
  return native::f32x4_relaxed_nmadd(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b), native::simd<float, 4, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 7
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f64x2_relaxed_madd<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::f64x2_relaxed_madd(a, b, c);
#else
  return native::f64x2_relaxed_madd(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b), native::simd<double, 2, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 8
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f64x2_relaxed_nmadd<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::f64x2_relaxed_nmadd(a, b, c);
#else
  return native::f64x2_relaxed_nmadd(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b), native::simd<double, 2, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 9
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i8x16_relaxed_laneselect<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::i8x16_relaxed_laneselect(a, b, c);
#else
  return native::i8x16_relaxed_laneselect(native::simd<std::uint8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b), native::simd<std::uint8_t, 16, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 10
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i16x8_relaxed_laneselect<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::i16x8_relaxed_laneselect(a, b, c);
#else
  return native::i16x8_relaxed_laneselect(native::simd<std::uint16_t, 8, profile>::from_native(a), native::simd<std::uint16_t, 8, profile>::from_native(b), native::simd<std::uint16_t, 8, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 11
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i32x4_relaxed_laneselect<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::i32x4_relaxed_laneselect(a, b, c);
#else
  return native::i32x4_relaxed_laneselect(native::simd<std::uint32_t, 4, profile>::from_native(a), native::simd<std::uint32_t, 4, profile>::from_native(b), native::simd<std::uint32_t, 4, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 12
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i64x2_relaxed_laneselect<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::i64x2_relaxed_laneselect(a, b, c);
#else
  return native::i64x2_relaxed_laneselect(native::simd<std::uint64_t, 2, profile>::from_native(a), native::simd<std::uint64_t, 2, profile>::from_native(b), native::simd<std::uint64_t, 2, profile>::from_native(c)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 13
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f32x4_relaxed_min<profile>(a, b);
#elif NATIVE_REJECT_SHAPE
  return native::f32x4_relaxed_min(a, b);
#else
  return native::f32x4_relaxed_min(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 14
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f32x4_relaxed_max<profile>(a, b);
#elif NATIVE_REJECT_SHAPE
  return native::f32x4_relaxed_max(a, b);
#else
  return native::f32x4_relaxed_max(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 15
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f64x2_relaxed_min<profile>(a, b);
#elif NATIVE_REJECT_SHAPE
  return native::f64x2_relaxed_min(a, b);
#else
  return native::f64x2_relaxed_min(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 16
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::f64x2_relaxed_max<profile>(a, b);
#elif NATIVE_REJECT_SHAPE
  return native::f64x2_relaxed_max(a, b);
#else
  return native::f64x2_relaxed_max(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 17
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i16x8_relaxed_q15mulr<profile>(a, b);
#elif NATIVE_REJECT_SHAPE
  return native::i16x8_relaxed_q15mulr(a, b);
#else
  return native::i16x8_relaxed_q15mulr(native::simd<std::int16_t, 8, profile>::from_native(a), native::simd<std::int16_t, 8, profile>::from_native(b)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 18
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i16x8_relaxed_dot_i8x16_i7x16<profile>(a, b);
#elif NATIVE_REJECT_SHAPE
  return native::i16x8_relaxed_dot_i8x16_i7x16(a, b);
#else
  return native::i16x8_relaxed_dot_i8x16_i7x16(native::simd<std::int8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b)).to_native();
#endif
#endif
#if NATIVE_RELAXED_OP == 19
#if NATIVE_REJECT_PRIVATE
  return native::detail::wasm_relaxed::i32x4_relaxed_dot_i8x16_i7x16_add<profile>(a, b, c);
#elif NATIVE_REJECT_SHAPE
  return native::i32x4_relaxed_dot_i8x16_i7x16_add(a, b, c);
#else
  return native::i32x4_relaxed_dot_i8x16_i7x16_add(native::simd<std::int8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b), native::simd<std::int32_t, 4, profile>::from_native(c)).to_native();
#endif
#endif
}
