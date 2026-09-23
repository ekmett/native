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
  template<bool Flush = false, unsigned Degree = 6, isa<> A> requires(Degree >= 1 && Degree <= 7 && A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr simd<float,4,A> exp(simd<float,4,A> x) noexcept {
    return ::math::exp<Flush, Degree>(x);
  }
  /// Advance every independent SIMD128 polynomial chain stage by stage.
  template<bool Flush = false, unsigned Degree = 6, std::size_t N, isa<> A> requires(Degree >= 1 && Degree <= 7 && A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp(std::array<simd<float,4,A>,N> const & x) noexcept {
    return ::math::exp<Flush, Degree>(x);
  }
  template<bool Flush, unsigned Degree = 6, isa<> A> requires(Degree >= 1 && Degree <= 7 && A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp(simd<float,4,A> x, std::bool_constant<Flush>, std::integral_constant<unsigned, Degree> = {}) noexcept {
    return ::math::exp<Flush, Degree>(x);
  }
  template<bool Flush, unsigned Degree = 6, std::size_t N, isa<> A> requires(Degree >= 1 && Degree <= 7 && A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp(std::array<simd<float,4,A>,N> const & x,
      std::bool_constant<Flush>, std::integral_constant<unsigned, Degree> = {}) noexcept {
    return ::math::exp<Flush, Degree>(x);
  }
  /// Binary32 SIMD128 exp2; multiply/add stages round separately, without relaxed SIMD.
  template<bool Flush = false, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr simd<float,4,A> exp2(simd<float,4,A> x) noexcept {
    return ::math::exp2<Flush>(x);
  }
  /// Advance every independent SIMD128 polynomial chain stage by stage.
  template<bool Flush = false, std::size_t N, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp2(std::array<simd<float,4,A>,N> const & x) noexcept {
    return ::math::exp2<Flush>(x);
  }
  template<bool Flush, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp2(simd<float,4,A> x, std::bool_constant<Flush>) noexcept {
    return ::math::exp2<Flush>(x);
  }
  template<bool Flush, std::size_t N, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto exp2(std::array<simd<float,4,A>,N> const & x,
      std::bool_constant<Flush>) noexcept {
    return ::math::exp2<Flush>(x);
  }
#define NATIVE_WASM_MATH(name) \
  template<isa<> A> requires(A.has(wasm_feature::simd128)) \
  native_nodiscard native_inline constexpr auto name(simd<float,4,A> x) noexcept { \
    return ::math::name(x); \
  } \
  template<std::size_t N, isa<> A> requires(A.has(wasm_feature::simd128)) \
  native_nodiscard native_inline constexpr auto name(std::array<simd<float,4,A>,N> const & x) noexcept { \
    return ::math::name(x); \
  }
  /// Finite binary32 lanes with |x| < 8192; multiply/add stages round separately.
  NATIVE_WASM_MATH(sin)
  NATIVE_WASM_MATH(cos)
  NATIVE_WASM_MATH(sincos)
  /// Binary32 approximations with separately rounded SIMD128 multiply/add.
  NATIVE_WASM_MATH(expm1)
  NATIVE_WASM_MATH(damping_gain)
  NATIVE_WASM_MATH(log)
  NATIVE_WASM_MATH(log2)
  NATIVE_WASM_MATH(log1p)
  NATIVE_WASM_MATH(tanh)
#undef NATIVE_WASM_MATH
  /// Binary32 SIMD128 atan2(y,x), with separately rounded multiply/add stages.
  template<isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto atan2(simd<float,4,A> y, simd<float,4,A> x) noexcept {
    return ::math::atan2(y,x);
  }
  /// Advance each atan2 stage across matching SIMD128 register arrays.
  template<std::size_t N, isa<> A> requires(A.has(wasm_feature::simd128))
  native_nodiscard native_inline constexpr auto atan2(std::array<simd<float,4,A>,N> const & y,
      std::array<simd<float,4,A>,N> const & x) noexcept { return ::math::atan2(y,x); }
}
#pragma clang attribute pop
#endif
