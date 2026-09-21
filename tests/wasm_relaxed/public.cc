// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <wasm_simd128.h>
import native.wasm.relaxed;

constexpr auto profile = native::feature_closure(native::wasm_feature::relaxed_simd);

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i8x16_relaxed_swizzle(v128_t a, v128_t b) {
  return native::i8x16_relaxed_swizzle(native::simd<std::uint8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_trunc_f32x4(v128_t a) {
  return native::i32x4_relaxed_trunc_f32x4(native::simd<float, 4, profile>::from_native(a)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_u32x4_relaxed_trunc_f32x4(v128_t a) {
  return native::u32x4_relaxed_trunc_f32x4(native::simd<float, 4, profile>::from_native(a)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_trunc_f64x2_zero(v128_t a) {
  return native::i32x4_relaxed_trunc_f64x2_zero(native::simd<double, 2, profile>::from_native(a)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_u32x4_relaxed_trunc_f64x2_zero(v128_t a) {
  return native::u32x4_relaxed_trunc_f64x2_zero(native::simd<double, 2, profile>::from_native(a)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_madd(v128_t a, v128_t b, v128_t c) {
  return native::f32x4_relaxed_madd(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b), native::simd<float, 4, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_nmadd(v128_t a, v128_t b, v128_t c) {
  return native::f32x4_relaxed_nmadd(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b), native::simd<float, 4, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_madd(v128_t a, v128_t b, v128_t c) {
  return native::f64x2_relaxed_madd(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b), native::simd<double, 2, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_nmadd(v128_t a, v128_t b, v128_t c) {
  return native::f64x2_relaxed_nmadd(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b), native::simd<double, 2, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i8x16_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return native::i8x16_relaxed_laneselect(native::simd<std::uint8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b), native::simd<std::uint8_t, 16, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i16x8_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return native::i16x8_relaxed_laneselect(native::simd<std::uint16_t, 8, profile>::from_native(a), native::simd<std::uint16_t, 8, profile>::from_native(b), native::simd<std::uint16_t, 8, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return native::i32x4_relaxed_laneselect(native::simd<std::uint32_t, 4, profile>::from_native(a), native::simd<std::uint32_t, 4, profile>::from_native(b), native::simd<std::uint32_t, 4, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i64x2_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return native::i64x2_relaxed_laneselect(native::simd<std::uint64_t, 2, profile>::from_native(a), native::simd<std::uint64_t, 2, profile>::from_native(b), native::simd<std::uint64_t, 2, profile>::from_native(c)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_min(v128_t a, v128_t b) {
  return native::f32x4_relaxed_min(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_max(v128_t a, v128_t b) {
  return native::f32x4_relaxed_max(native::simd<float, 4, profile>::from_native(a), native::simd<float, 4, profile>::from_native(b)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_min(v128_t a, v128_t b) {
  return native::f64x2_relaxed_min(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_max(v128_t a, v128_t b) {
  return native::f64x2_relaxed_max(native::simd<double, 2, profile>::from_native(a), native::simd<double, 2, profile>::from_native(b)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i16x8_relaxed_q15mulr(v128_t a, v128_t b) {
  return native::i16x8_relaxed_q15mulr(native::simd<std::int16_t, 8, profile>::from_native(a), native::simd<std::int16_t, 8, profile>::from_native(b)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i16x8_relaxed_dot_i8x16_i7x16(v128_t a, v128_t b) {
  return native::i16x8_relaxed_dot_i8x16_i7x16(native::simd<std::int8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b)).to_native();
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_dot_i8x16_i7x16_add(v128_t a, v128_t b, v128_t c) {
  return native::i32x4_relaxed_dot_i8x16_i7x16_add(native::simd<std::int8_t, 16, profile>::from_native(a), native::simd<std::uint8_t, 16, profile>::from_native(b), native::simd<std::int32_t, 4, profile>::from_native(c)).to_native();
}
