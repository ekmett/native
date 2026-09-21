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

#include "native/detail/constexpr_float.h"
#include <limits>

namespace native::detail::wasm_relaxed_constant {
  template<class V>
  constexpr auto lanes(V value) noexcept {
    std::array<typename V::value_type, V::lanes> result{};
    value.store(result.data());
    return result;
  }

  template<class T>
  using format = std::conditional_t<sizeof(T) == 4,
    constexpr_float::binary32, constexpr_float::binary64>;

  template<class V>
  constexpr V swizzle(V value, V indices) noexcept {
    auto input = lanes(value);
    auto result = lanes(indices);
    for (auto & index : result) {
      index = index < 16 ? input[index] : 0;
    }
    return V::load(result.data());
  }

  // Select the saturating result. Inspect encodings before conversion so NaNs,
  // infinities and out-of-range finite values never invoke a C++ invalid cast.
  template<class To, class From>
  constexpr To truncate(From value) noexcept {
    using format_type = format<From>;
    auto bits = std::bit_cast<typename format_type::bits_type>(value);
    if (constexpr_float::is_nan<format_type>(bits)) {
      return 0;
    }
    constexpr bool signed_result = std::is_signed_v<To>;
    constexpr std::uint64_t positive_limit = std::numeric_limits<To>::max();
    constexpr std::uint64_t negative_limit = signed_result ? std::uint64_t{1} << 31 : 0;
    auto parts = constexpr_float::unpack<format_type>(bits);
    auto limit = parts.sign ? negative_limit : positive_limit;
    std::uint64_t magnitude = 0;
    if (constexpr_float::is_infinite<format_type>(bits) || parts.exponent > 32) {
      magnitude = limit;
    } else if (parts.exponent >= 0) {
      magnitude = parts.significand > (limit >> parts.exponent)
        ? limit : parts.significand << parts.exponent;
    } else if (parts.exponent > -64) {
      magnitude = parts.significand >> -parts.exponent;
      if (magnitude > limit) {
        magnitude = limit;
      }
    }
    if constexpr (signed_result) {
      return static_cast<To>(parts.sign ? -static_cast<std::int64_t>(magnitude)
        : static_cast<std::int64_t>(magnitude));
    } else {
      return static_cast<To>(magnitude);
    }
  }

  template<class R, class V>
  constexpr R truncation(V value) noexcept {
    auto input = lanes(value);
    std::array<typename R::value_type, R::lanes> result{};
    for (std::size_t i = 0; i < V::lanes; ++i) {
      result[i] = truncate<typename R::value_type>(input[i]);
    }
    return R::load(result.data());
  }

  // The constant policy is fused, round-to-nearest-even with gradual underflow.
  template<bool Negative, class V>
  constexpr V multiply_add(V a, V b, V c) noexcept {
    using lane_type = typename V::value_type;
    using format_type = format<lane_type>;
    using word_type = typename format_type::bits_type;
    auto av = lanes(a);
    auto bv = lanes(b);
    auto cv = lanes(c);
    for (std::size_t i = 0; i < V::lanes; ++i) {
      auto x = std::bit_cast<word_type>(av[i]);
      if constexpr (Negative) {
        x ^= format_type::sign_mask;
      }
      cv[i] = std::bit_cast<lane_type>(constexpr_float::fma_bits<format_type>(
        x, std::bit_cast<word_type>(bv[i]), std::bit_cast<word_type>(cv[i])));
    }
    return V::load(cv.data());
  }

  template<class V>
  constexpr V lane_select(V a, V b, V mask) noexcept {
    auto av = lanes(a);
    auto bv = lanes(b);
    auto mv = lanes(mask);
    for (std::size_t i = 0; i < V::lanes; ++i) {
      av[i] = (av[i] & mv[i]) | (bv[i] & ~mv[i]);
    }
    return V::load(av.data());
  }

  // Choose strict Wasm min/max: quiet NaN, negative zero for min, positive zero
  // for max. Runtime relaxed operations may select different permitted values.
  template<bool Maximum, class V>
  constexpr V minimum_maximum(V a, V b) noexcept {
    using lane_type = typename V::value_type;
    using format_type = format<lane_type>;
    using word_type = typename format_type::bits_type;
    auto av = lanes(a);
    auto bv = lanes(b);
    for (std::size_t i = 0; i < V::lanes; ++i) {
      auto x = std::bit_cast<word_type>(av[i]);
      auto y = std::bit_cast<word_type>(bv[i]);
      word_type result;
      if (constexpr_float::is_nan<format_type>(x) || constexpr_float::is_nan<format_type>(y)) {
        result = constexpr_float::default_nan<format_type>();
      } else if (constexpr_float::is_zero<format_type>(x) && constexpr_float::is_zero<format_type>(y)) {
        result = Maximum ? x & y : x | y;
      } else {
        result = constexpr_float::less_bits<format_type>(x, y) != Maximum ? x : y;
      }
      av[i] = std::bit_cast<lane_type>(result);
    }
    return V::load(av.data());
  }

  template<class V>
  constexpr V q15_multiply(V a, V b) noexcept {
    auto av = lanes(a);
    auto bv = lanes(b);
    for (std::size_t i = 0; i < V::lanes; ++i) {
      auto result = (std::int32_t{av[i]} * bv[i] + 0x4000) >> 15;
      av[i] = static_cast<std::int16_t>(result > 32767 ? 32767 : result);
    }
    return V::load(av.data());
  }

  template<class V, class W>
  constexpr auto dot_pairs(V a, W b) noexcept {
    auto av = lanes(a);
    auto bv = lanes(b);
    std::array<std::int16_t, 8> result{};
    for (std::size_t i = 0; i < result.size(); ++i) {
      auto first = std::int32_t{av[2 * i]} * std::bit_cast<std::int8_t>(bv[2 * i]);
      auto second = std::int32_t{av[2 * i + 1]} * std::bit_cast<std::int8_t>(bv[2 * i + 1]);
      auto sum = first + second;
      result[i] = static_cast<std::int16_t>(sum > 32767 ? 32767 : sum < -32768 ? -32768 : sum);
    }
    return result;
  }

  template<class R, class V, class W>
  constexpr R dot(V a, W b) noexcept {
    auto result = dot_pairs(a, b);
    return R::load(result.data());
  }

  template<class V, class W, class R>
  constexpr R dot_add(V a, W b, R c) noexcept {
    auto pairs = dot_pairs(a, b);
    auto result = lanes(c);
    for (std::size_t i = 0; i < result.size(); ++i) {
      auto sum = std::int32_t{pairs[2 * i]} + pairs[2 * i + 1];
      result[i] = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(result[i]) + static_cast<std::uint32_t>(sum));
    }
    return R::load(result.data());
  }
}
