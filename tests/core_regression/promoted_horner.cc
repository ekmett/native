// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#if !defined(__wasm__)
#include "support/fp_environment.h"
#include "support/profile.h"
#elif NATIVE_TEST_IMPORT
import native;
#else
#include <native/simd.h>
#endif
#if NATIVE_TEST_IMPORT
import native.wide;
import native.math;
#else
#include <native/wide.h>
#include <native/wide_math.h>
#endif

namespace {
  using word = std::uint32_t;
  using scalar = native::simd<float, 1, native::scalar>;
#if defined(__wasm__)
  constexpr auto architecture = native::feature_closure(native::wasm_feature::simd128);
#else
  constexpr auto architecture = test_arch;
#endif
  template<unsigned N> using vector = native::simd<float, N, architecture>;
  constexpr float from_bits(word value) { return std::bit_cast<float>(value); }
  constexpr word bits(float value) { return std::bit_cast<word>(value); }
  constexpr bool nan(word value) { return (value & 0x7fffffffu) > 0x7f800000u; }
  constexpr float infinity = from_bits(0x7f800000u);
  constexpr float quiet_nan = from_bits(0x7fc12345u);
  std::size_t checks = 0;

  void require(bool value, char const * message) {
    if (!value) {
      std::fprintf(stderr, "horner: %s\n", message);
      std::abort();
    }
    ++checks;
  }
  void inspect(float actual, float expected) {
    if (nan(bits(expected)) ? !nan(bits(actual)) : bits(actual) != bits(expected)) {
      std::fprintf(stderr, "horner: actual=%08x reference=%08x\n", bits(actual), bits(expected));
      std::abort();
    }
    ++checks;
  }
  template<class... C> concept coefficient_list = requires(C const &... c) {
    math::horner(c...);
  };
  template<class T, class... C> concept argument = requires(T const & z, C const &... c) {
    math::horner(c...)(z);
  };
  struct convertible_coefficient { operator float() const { return 1.f; } };
  static_assert(coefficient_list<float> && coefficient_list<float, scalar, float>);
  static_assert(!coefficient_list<> && !coefficient_list<int> && !coefficient_list<double>);
  static_assert(!coefficient_list<float, int> && !coefficient_list<convertible_coefficient>);
  static_assert(argument<float, float> && argument<float, scalar, float>);
  static_assert(!argument<double, float> && !argument<int, float>);
  static_assert(!argument<std::tuple<float, float>, float>);
  static_assert(!argument<std::array<double, 2>, float>);
  static_assert(!argument<float, std::array<float, 1>>);
  static_assert(!argument<scalar, native::simd<int, 1, native::scalar>>);

  constexpr auto polynomial = math::horner(3.f, 5.f, 7.f);
  static_assert(polynomial(2.f) == 29.f && polynomial(-2.f) == 9.f);
  static_assert(wide::horner(1.f, 2.f, 3.f, 4.f, 5.f)(2.f) == 57.f);
  static_assert(std::same_as<decltype(polynomial(1.f)), float>);
  static_assert(std::same_as<decltype(polynomial(std::array<float, 3>{})), std::array<float, 3>>);
  static_assert(std::same_as<decltype(polynomial(native::wide<float, 3>{})), native::wide<float, 3>>);
  static_assert(noexcept(polynomial(1.f)));
  static_assert(bits(math::horner(-0.f)(infinity)) == 0x80000000u);
  static_assert(bits(math::horner(quiet_nan)(infinity)) == 0x7fc12345u);
  static_assert(bits(math::horner(from_bits(1u))(quiet_nan)) == 1u);
  static_assert(bits(math::horner(infinity)(quiet_nan)) == 0x7f800000u);
  static_assert(bits(math::horner(scalar(-0.f))(quiet_nan)) == 0x80000000u);
  static_assert(bits(math::horner(0x1.000002p0f, -1.f)(0x1.fffffcp-1f)) == bits(-0x1p-46f));
  static_assert(math::horner(1.f)(std::array<float, 0>{}).empty());
  static_assert(math::horner(0.f, infinity)(std::array<float, 0>{}).empty());
  static_assert(std::same_as<decltype(polynomial(native::wide<float, 0>{})), native::wide<float, 0>>);

  template<class V> constexpr std::array<word, V::lanes> words(V const & value) {
    std::array<word, V::lanes> result{};
    value.store_bits(result.data());
    return result;
  }
  template<class V> constexpr V lane_values(float first, float step) {
    std::array<word, V::lanes> values{};
    for (std::size_t lane = 0; lane < V::lanes; ++lane)
      values[lane] = bits(first + float(lane) * step);
    return V::load_bits(values.data());
  }
  template<class V> constexpr bool constexpr_shapes() {
    using pack = std::array<V, 3>;
    using wide_pack = native::wide<V, 3>;
    constexpr auto first = lane_values<V>(1.f, 1.f);
    constexpr auto last = lane_values<V>(4.f, -1.f);
    constexpr auto p = math::horner(first, -2.f, last);
    constexpr pack z{V(2.f), V(3.f), V(-1.f)};
    constexpr auto array_result = p(z);
    constexpr auto wide_result = p(wide_pack{z});
    constexpr auto constant = math::horner(last)(wide_pack{z});
    constexpr std::array<float, 3> inputs{2.f, 3.f, -1.f};
    for (std::size_t reg = 0; reg < z.size(); ++reg) {
      auto actual = words(array_result[reg]);
      if (actual != words(wide_result.registers[reg]) || actual != words(p(z[reg]))) return false;
      if (words(constant.registers[reg]) != words(last)) return false;
      for (std::size_t lane = 0; lane < V::lanes; ++lane) {
        auto x = inputs[reg];
        auto expected = (float(lane + 1) * x - 2.f) * x + 4.f - float(lane);
        if (actual[lane] != bits(expected)) return false;
      }
    }
    return true;
  }

  // Baseline Wasm SIMD128 intentionally has two roundings per stage. Scalar
  // leaves and the other native profiles use one correctly rounded std::fma.
  template<class V = scalar> float reference_stage(float a, float z, float b) {
#if defined(__wasm__)
    if constexpr (V::architecture != native::scalar) {
#pragma clang fp contract(off)
      float product = a * z;
      return product + b;
    } else
#endif
      return std::fma(a, z, b);
  }
  word random_state = 0x6fd4a931u;
  float finite_sample() {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    // Normal numbers in [1/2, 2) avoid platform-specific denormal behavior.
    return from_bits((random_state & 0x807fffffu) | ((126u + ((random_state >> 24) & 1u)) << 23));
  }
  constexpr std::array<word, 12> special{
    0u, 0x80000000u, 0x3f800000u, 0xbf800000u, 0x40000000u, 0xc0000000u,
    0x7f800000u, 0xff800000u, 0x7fc12345u, 0xffc12345u, 1u, 0x80000001u};

  void check_scalars() {
    float first = 3.f, second = 5.f, last = 7.f;
    auto const owned = math::horner(first, second, last);
    first = 10.f; second = 20.f; last = 30.f;
    inspect(owned(2.f), 29.f);
    inspect(owned(-2.f), 9.f);
    inspect(math::horner(0x1.000002p0f, -1.f)(0x1.fffffcp-1f), -0x1p-46f);
    for (word input : special) {
      float z = from_bits(input);
      for (word coefficient : special) {
        auto actual = math::horner(from_bits(coefficient))(z);
        require(bits(actual) == coefficient, "constant polynomial changed coefficient bits");
      }
      inspect(math::horner(0.f, 0.f)(z), reference_stage(0.f, z, 0.f));
      inspect(math::horner(0.f, 1.f, 2.f)(z), reference_stage(reference_stage(0.f, z, 1.f), z, 2.f));
      inspect(math::horner(-0.f, -0.f)(z), reference_stage(-0.f, z, -0.f));
    }
    for (unsigned sample = 0; sample < 256; ++sample) {
      std::array<float, 3> z{finite_sample(), finite_sample(), finite_sample()};
      auto c0 = finite_sample(), c1 = finite_sample(), c2 = finite_sample(), c3 = finite_sample();
      auto const p = math::horner(c0, c1, c2, c3);
      auto const array_result = p(z);
      auto const wide_result = p(native::wide<float, 3>{z});
      for (std::size_t reg = 0; reg < z.size(); ++reg) {
        float expected = std::fma(std::fma(std::fma(c0, z[reg], c1), z[reg], c2), z[reg], c3);
        inspect(p(z[reg]), expected);
        inspect(array_result[reg], expected);
        inspect(wide_result.registers[reg], expected);
      }
    }
  }

  template<class V> void check_vectors() {
    using pack = std::array<V, 3>;
    using wide_pack = native::wide<V, 3>;
    static_assert(argument<V, float> && argument<V, V> && argument<pack, V, float, V>);
    static_assert(argument<wide_pack, V, float>);
    static_assert(!argument<V, std::array<V, 1>> && !argument<pack, pack>);
    static_assert(std::same_as<decltype(math::horner(V(1.f), 2.f)(V{})), V>);
    static_assert(std::same_as<decltype(math::horner(1.f, V(2.f))(pack{})), pack>);
    static_assert(std::same_as<decltype(wide::horner(1.f)(wide_pack{})), wide_pack>);
    using evaluator = decltype(math::horner(V(1.f), 2.f));
    static_assert(noexcept(std::declval<evaluator const &>()(std::declval<pack const &>())));
    static_assert(constexpr_shapes<V>());
    static_assert(std::same_as<decltype(math::horner(1.f)(std::array<V, 0>{})), std::array<V, 0>>);
    static_assert(std::same_as<decltype(math::horner(V(1.f))(native::wide<V, 0>{})), native::wide<V, 0>>);
    require(math::horner(infinity, V(quiet_nan))(std::array<V, 0>{}).empty(), "empty SIMD array changed size");
    (void)math::horner(V(infinity), quiet_nan)(native::wide<V, 0>{});

    V coefficient = lane_values<V>(1.f, 1.f);
    auto const owned = math::horner(coefficient);
    auto expected_constant = words(coefficient);
    coefficient = V(-9.f);
    require(words(owned(V(infinity))) == expected_constant, "SIMD coefficients were not retained by value");
    auto const fused = math::horner(0x1.000002p0f, -1.f)(V(0x1.fffffcp-1f));
    for (word actual : words(fused)) inspect(from_bits(actual), reference_stage<V>(0x1.000002p0f, 0x1.fffffcp-1f, -1.f));

    for (word input : special) {
      V z = V::from_bits(input);
      auto zero = words(math::horner(0.f, V(0.f))(z));
      auto leading_zero = words(math::horner(V(0.f), 1.f, V(2.f))(z));
      auto negative_zero = words(math::horner(-0.f, V(-0.f))(z));
      auto constant = words(math::horner(-0.f)(z));
      for (std::size_t lane = 0; lane < V::lanes; ++lane) {
        inspect(from_bits(zero[lane]), reference_stage<V>(0.f, from_bits(input), 0.f));
        inspect(from_bits(leading_zero[lane]), reference_stage<V>(reference_stage<V>(0.f, from_bits(input), 1.f), from_bits(input), 2.f));
        inspect(from_bits(negative_zero[lane]), reference_stage<V>(-0.f, from_bits(input), -0.f));
        require(constant[lane] == 0x80000000u, "constant SIMD polynomial lost negative zero");
      }
    }
    for (unsigned sample = 0; sample < 128; ++sample) {
      std::array<word, V::lanes> a{}, b{}, values{};
      for (std::size_t lane = 0; lane < V::lanes; ++lane) {
        a[lane] = bits(finite_sample()); b[lane] = bits(finite_sample());
      }
      auto c0 = V::load_bits(a.data()), c2 = V::load_bits(b.data());
      auto c1 = finite_sample(), c3 = finite_sample();
      auto const p = math::horner(c0, c1, c2, c3);
      pack z{};
      for (auto & reg : z) {
        for (auto & lane : values) lane = bits(finite_sample());
        reg = V::load_bits(values.data());
      }
      auto array_result = p(z);
      auto wide_result = p(wide_pack{z});
      auto alias_result = wide::horner(c0, c1, c2, c3)(z);
      for (std::size_t reg = 0; reg < z.size(); ++reg) {
        auto input = words(z[reg]), actual = words(array_result[reg]);
        require(actual == words(wide_result.registers[reg]), "SIMD array/wide results differ");
        require(actual == words(p(z[reg])), "SIMD array/register results differ");
        require(actual == words(alias_result[reg]), "wide namespace alias differs");
        for (std::size_t lane = 0; lane < V::lanes; ++lane) {
          float x = from_bits(input[lane]);
          float expected = reference_stage<V>(reference_stage<V>(reference_stage<V>(from_bits(a[lane]), x, c1), x, from_bits(b[lane])), x, c3);
          inspect(from_bits(actual[lane]), expected);
        }
      }
    }
  }

  void run() {
    check_scalars();
    check_vectors<scalar>();
#if defined(__wasm__)
    static_assert(!argument<vector<4>, scalar> && !argument<scalar, vector<4>>);
    check_vectors<vector<4>>();
#elif NATIVE_TEST_PROFILE != 0
    static_assert(!argument<vector<1>, scalar> && !argument<scalar, vector<1>>);
    static_assert(!argument<vector<2>, vector<3>> && !argument<vector<4>, vector<2>>);
    check_vectors<vector<1>>();
    check_vectors<vector<2>>();
    check_vectors<vector<3>>();
    check_vectors<vector<4>>();
#if NATIVE_TEST_PROFILE == 256 || NATIVE_TEST_PROFILE == 512
    check_vectors<vector<8>>();
#endif
#if NATIVE_TEST_PROFILE == 512
    check_vectors<vector<16>>();
#endif
#endif
  }
}

int main() {
#if defined(__wasm__)
  run();
#else
  auto saved = native::test::read_fp_state();
  for (auto mode : {native::test::fp_mode::gradual, native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);
    run();
    require(scope.controls_match(), "evaluation changed FP controls");
  }
  require(native::test::read_fp_state() == saved, "fixture failed to restore FP state");
#endif
  std::printf("horner: %zu checks; curried ownership, shapes, constexpr, constraints and multiply-add graph passed\n", checks);
}
