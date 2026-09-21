// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/attributes.h"
#include "native/config.h"
#include "native/isa.h"

#if NATIVE_HOST_WASM
#include <wasm_simd128.h>

namespace native::detail::wasm_relaxed {
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i8x16_relaxed_swizzle(v128_t a, v128_t b) noexcept {
    return wasm_i8x16_relaxed_swizzle(a, b);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i32x4_relaxed_trunc_f32x4(v128_t a) noexcept {
    return wasm_i32x4_relaxed_trunc_f32x4(a);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t u32x4_relaxed_trunc_f32x4(v128_t a) noexcept {
    return wasm_u32x4_relaxed_trunc_f32x4(a);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i32x4_relaxed_trunc_f64x2_zero(v128_t a) noexcept {
    return wasm_i32x4_relaxed_trunc_f64x2_zero(a);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t u32x4_relaxed_trunc_f64x2_zero(v128_t a) noexcept {
    return wasm_u32x4_relaxed_trunc_f64x2_zero(a);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f32x4_relaxed_madd(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_f32x4_relaxed_madd(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f32x4_relaxed_nmadd(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_f32x4_relaxed_nmadd(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f64x2_relaxed_madd(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_f64x2_relaxed_madd(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f64x2_relaxed_nmadd(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_f64x2_relaxed_nmadd(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i8x16_relaxed_laneselect(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_i8x16_relaxed_laneselect(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i16x8_relaxed_laneselect(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_i16x8_relaxed_laneselect(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i32x4_relaxed_laneselect(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_i32x4_relaxed_laneselect(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i64x2_relaxed_laneselect(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_i64x2_relaxed_laneselect(a, b, c);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f32x4_relaxed_min(v128_t a, v128_t b) noexcept {
    return wasm_f32x4_relaxed_min(a, b);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f32x4_relaxed_max(v128_t a, v128_t b) noexcept {
    return wasm_f32x4_relaxed_max(a, b);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f64x2_relaxed_min(v128_t a, v128_t b) noexcept {
    return wasm_f64x2_relaxed_min(a, b);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t f64x2_relaxed_max(v128_t a, v128_t b) noexcept {
    return wasm_f64x2_relaxed_max(a, b);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i16x8_relaxed_q15mulr(v128_t a, v128_t b) noexcept {
    return wasm_i16x8_relaxed_q15mulr(a, b);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i16x8_relaxed_dot_i8x16_i7x16(v128_t a, v128_t b) noexcept {
    return wasm_i16x8_relaxed_dot_i8x16_i7x16(a, b);
  }

  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  v128_t i32x4_relaxed_dot_i8x16_i7x16_add(v128_t a, v128_t b, v128_t c) noexcept {
    return wasm_i32x4_relaxed_dot_i8x16_i7x16_add(a, b, c);
  }

}
#endif
