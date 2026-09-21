// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <wasm_simd128.h>

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i8x16_relaxed_swizzle(v128_t a, v128_t b) {
  return wasm_i8x16_relaxed_swizzle(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_trunc_f32x4(v128_t a) {
  return wasm_i32x4_relaxed_trunc_f32x4(a);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_u32x4_relaxed_trunc_f32x4(v128_t a) {
  return wasm_u32x4_relaxed_trunc_f32x4(a);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_trunc_f64x2_zero(v128_t a) {
  return wasm_i32x4_relaxed_trunc_f64x2_zero(a);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_u32x4_relaxed_trunc_f64x2_zero(v128_t a) {
  return wasm_u32x4_relaxed_trunc_f64x2_zero(a);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_madd(v128_t a, v128_t b, v128_t c) {
  return wasm_f32x4_relaxed_madd(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_nmadd(v128_t a, v128_t b, v128_t c) {
  return wasm_f32x4_relaxed_nmadd(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_madd(v128_t a, v128_t b, v128_t c) {
  return wasm_f64x2_relaxed_madd(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_nmadd(v128_t a, v128_t b, v128_t c) {
  return wasm_f64x2_relaxed_nmadd(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i8x16_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return wasm_i8x16_relaxed_laneselect(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i16x8_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return wasm_i16x8_relaxed_laneselect(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return wasm_i32x4_relaxed_laneselect(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i64x2_relaxed_laneselect(v128_t a, v128_t b, v128_t c) {
  return wasm_i64x2_relaxed_laneselect(a, b, c);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_min(v128_t a, v128_t b) {
  return wasm_f32x4_relaxed_min(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f32x4_relaxed_max(v128_t a, v128_t b) {
  return wasm_f32x4_relaxed_max(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_min(v128_t a, v128_t b) {
  return wasm_f64x2_relaxed_min(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_f64x2_relaxed_max(v128_t a, v128_t b) {
  return wasm_f64x2_relaxed_max(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i16x8_relaxed_q15mulr(v128_t a, v128_t b) {
  return wasm_i16x8_relaxed_q15mulr(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i16x8_relaxed_dot_i8x16_i7x16(v128_t a, v128_t b) {
  return wasm_i16x8_relaxed_dot_i8x16_i7x16(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t native_relaxed_i32x4_relaxed_dot_i8x16_i7x16_add(v128_t a, v128_t b, v128_t c) {
  return wasm_i32x4_relaxed_dot_i8x16_i7x16_add(a, b, c);
}
