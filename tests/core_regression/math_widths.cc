// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#if defined(__wasm__)
#if NATIVE_TEST_IMPORT
import native;
#else
#include <native/simd.h>
#include <native/simd/math/exp.h>
#endif
constexpr auto test_arch = native::feature_closure(native::wasm_feature::simd128);
#else
#include "support/profile.h"
#endif
#if NATIVE_TEST_IMPORT
import native.math;
#else
#include <native/wide.h>
#include <native/wide_math.h>
#endif

namespace {
  using namespace native;
  constexpr auto arm_a = neon;
  constexpr auto x86_a = avx2;
  constexpr auto x86_z = avx512;
  constexpr auto wasm_a = feature_closure(wasm_feature::simd128);
  constexpr auto wasm_r = feature_closure(wasm_feature::relaxed_simd);

  // Query all families on every build without instantiating foreign registers.
  static_assert(exp_width<float, 4, arm_a> == 4);
  static_assert(exp_width<float, 8, x86_a> == 6);
  static_assert(exp_width<float, 16, x86_z> == 6);
  static_assert(exp_width<float, 4, wasm_a> == 2);
  static_assert(exp_width<float, 4, wasm_r> == exp_width<float, 4, wasm_a>);
  static_assert(atan2_width<float, 4, arm_a> == 2);
  static_assert(atan2_width<float, 8, x86_a> == 2);
  static_assert(atan2_width<float, 16, x86_z> == 2);
  static_assert(exp2_width<float, 8, x86_a> == 2);
  static_assert(sincos_width<float, 4, arm_a> == 4);
  static_assert(sincos_width<float, 3, arm_a> == 2);
  static_assert(sincos_width<float, 4, x86_z> == 2);

  // Extra features must not change the shape or pretend short vectors are ZMMs.
  static_assert(exp_width<float, 4, x86_z> == exp_width<float, 4, x86_a>);
  static_assert(exp_width<float, 8, x86_z> == exp_width<float, 8, x86_a>);
  static_assert(exp_width<float, 4, arm_a & arm_feature::aes> == 4);
  static_assert(exp_width<float, 8, x86_a & x86_feature::aes> == 6);
  constexpr auto foundation = feature_closure(x86_a & x86_feature::avx512f & x86_feature::avx512dq);
  static_assert(exp_width<float, 16, foundation> == 6);
  static_assert(exp_width<float, 16, foundation & x86_feature::avx512vl> == 6);
  static_assert(exp_width<float, 16, x86_a> == 1);
  static_assert(exp_width<float, 8, isa<x86>{x86_feature::avx2}> == 1);
  static_assert(exp_width<float, 4, isa<arm>{}> == 1);
  static_assert(exp_width<float, 4, isa<wasm>{}> == 1);
  static_assert(exp_width<float, 4, isa<architecture::unknown>{}> == 1);
  constexpr auto invalid = [] { auto a = arm_a; a.set(static_cast<arm_feature>(1000), true); return a; }();
  static_assert(exp_width<float, 4, invalid> == 1);

  // A conservative result does not make an unsupported math shape callable.
  struct custom_element {};
  static_assert(exp_width<float, 1, x86_z> == 1);
  static_assert(exp_width<float, 1, arm_a> == 1);
  static_assert(exp_width<double, 4, x86_a> == 1);
  static_assert(exp_width<int, 4, arm_a> == 1);
  static_assert(exp_width<custom_element, 4, arm_a> == 1);
  static_assert(exp_width<float, 5, x86_z> == 1);
  static_assert(exp_width<float, 8, arm_a> == 1);
  static_assert(exp_width<float, 2, arm_a> == 4);
  static_assert(exp_width<float, 3, x86_a> == 4);

  template<std::size_t K> concept has_width = requires { exp_width<float, K, arm_a>; };
  static_assert(!has_width<0> && has_width<1>);
  static_assert(std::same_as<decltype(exp_width<float, 4, arm_a>), std::size_t const>);
  static_assert(::math::exp_width<float, 4, arm_a> == native::exp_width<float, 4, arm_a>);
  static_assert(damping_gain_width<float, 8, x86_a> == expm1_width<float, 8, x86_a>);
  static_assert(log2_width<float, 4, arm_a> == log_width<float, 4, arm_a>);
  static_assert(sin_width<float, 4, arm_a> == sincos_width<float, 4, arm_a>);
  static_assert(cos_width<float, 4, arm_a> == sincos_width<float, 4, arm_a>);
#if NATIVE_TEST_IMPORT
  // Both defaults come from their module providers, not this consumer's flags.
  constexpr auto default_a = simd<float, 1>::architecture;
  static_assert(exp_width<float, 4> == exp_width<float, 4, default_a>);
  static_assert(native::math::atan2_width<float, 4, arm_a> == ::math::atan2_width<float, 4, arm_a>);
#else
  static_assert(exp_width<float, 4> == exp_width<float, 4, NATIVE_BASELINE>);
#endif

#if NATIVE_TEST_PROFILE == 512
  constexpr std::size_t lanes = 16;
#elif NATIVE_TEST_PROFILE == 256
  constexpr std::size_t lanes = 8;
#elif NATIVE_TEST_PROFILE == 128 || defined(__wasm__)
  constexpr std::size_t lanes = 4;
#else
  constexpr std::size_t lanes = 1;
#endif
  using V = simd<float, lanes, test_arch>;
  template<std::size_t N, class F>
  bool check(F f, float x, float expected) {
    std::array<V, N> input;
    for (auto & v : input) v = V(x);
    auto array_result = f(input);
    auto wide_result = f(native::wide<V, N>{input});
    static_assert(std::same_as<decltype(array_result), std::array<V, N>>);
    static_assert(std::same_as<decltype(wide_result), native::wide<V, N>>);
    for (std::size_t i = 0; i < N; ++i) {
      std::array<float, lanes> a{}, b{};
      array_result[i].store(a.data());
      wide_result.registers[i].store(b.data());
      for (std::size_t j = 0; j < lanes; ++j)
        if (a[j] != expected || b[j] != expected) return false;
    }
    return true;
  }
}

int main() {
  return !(
    check<native::exp_width<float, lanes, test_arch>>([](auto x) { return ::math::exp(x); }, 0.f, 1.f) &&
    check<native::exp2_width<float, lanes, test_arch>>([](auto x) { return ::math::exp2(x); }, 0.f, 1.f) &&
    check<native::expm1_width<float, lanes, test_arch>>([](auto x) { return ::math::expm1(x); }, 0.f, 0.f) &&
    check<native::damping_gain_width<float, lanes, test_arch>>([](auto x) { return ::math::damping_gain(x); }, 0.f, 0.f) &&
    check<native::log_width<float, lanes, test_arch>>([](auto x) { return ::math::log(x); }, 1.f, 0.f) &&
    check<native::log2_width<float, lanes, test_arch>>([](auto x) { return ::math::log2(x); }, 1.f, 0.f) &&
    check<native::log1p_width<float, lanes, test_arch>>([](auto x) { return ::math::log1p(x); }, 0.f, 0.f) &&
    check<native::tanh_width<float, lanes, test_arch>>([](auto x) { return ::math::tanh(x); }, 0.f, 0.f) &&
    check<native::atan2_width<float, lanes, test_arch>>([](auto x) { return ::math::atan2(x, x); }, 0.f, 0.f) &&
    check<native::sin_width<float, lanes, test_arch>>([](auto x) { return ::math::sin(x); }, 0.f, 0.f) &&
    check<native::cos_width<float, lanes, test_arch>>([](auto x) { return ::math::cos(x); }, 0.f, 1.f) &&
    check<native::sincos_width<float, lanes, test_arch>>([](auto x) { return ::math::sincos(x).first; }, 0.f, 0.f));
}
