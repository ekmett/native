#pragma once
#include "native/simd.h"
#include "native/wide_math.h"
#define NATIVE_BACKEND_BODY "native/simd/math/exp_body.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY

#if NATIVE_HOST_WASM && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#pragma clang attribute push(__attribute__((target("simd128"))), apply_to=function)
namespace native {
  /// Binary32 SIMD128 exp; multiply/add stages round separately, without relaxed SIMD.
  template<bool Flush = false, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr simd<float,4,A> exp(simd<float,4,A> x) noexcept {
    return ::math::exp<Flush>(x);
  }
  /// Advance every independent SIMD128 polynomial chain stage by stage.
  template<bool Flush = false, std::size_t N, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp(std::array<simd<float,4,A>,N> const & x) noexcept {
    return ::math::exp<Flush>(x);
  }
  template<bool Flush, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp(simd<float,4,A> x, std::bool_constant<Flush>) noexcept {
    return ::math::exp<Flush>(x);
  }
  template<bool Flush, std::size_t N, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp(std::array<simd<float,4,A>,N> const & x,
      std::bool_constant<Flush>) noexcept {
    return ::math::exp<Flush>(x);
  }
#define NATIVE_WASM_TRIG(name) \
  template<isa<> A> requires(A.has(wasm_feature::simd128)) \
  native_nodiscard native_inline constexpr auto name(simd<float,4,A> x) noexcept { \
    return ::math::name(x); \
  } \
  template<std::size_t N, isa<> A> requires(A.has(wasm_feature::simd128)) \
  native_nodiscard native_inline constexpr auto name(std::array<simd<float,4,A>,N> const & x) noexcept { \
    return ::math::name(x); \
  }
  /// Finite binary32 lanes with |x| < 8192; multiply/add stages round separately.
  NATIVE_WASM_TRIG(sin)
  NATIVE_WASM_TRIG(cos)
  NATIVE_WASM_TRIG(sincos)
#undef NATIVE_WASM_TRIG
}
#pragma clang attribute pop
#endif
