// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <vector>
#if defined(NATIVE_TEST_MPFR)
#include <mpfr.h>
#endif
#if !defined(__wasm__)
#include "support/fp_environment.h"
#include "support/profile.h"
#elif NATIVE_TEST_IMPORT
import native;
#else
#include <native/simd.h>
#include <native/simd/math/exp.h>
#endif
#if NATIVE_TEST_IMPORT
import native.wide;
import native.math;
#else
#include <native/wide.h>
#include <native/wide_math.h>
#endif
#include "support/exp_scaling.h"

namespace custom {
  struct plain { float value{}; };
  constexpr plain exp(plain x) noexcept(false) { return {x.value + 1.f}; }
  struct tagged { float value{}; };
  template<bool F> constexpr tagged exp(tagged x, std::bool_constant<F>) noexcept {
    return {x.value + (F ? 10.f : 1.f)};
  }
  struct degree_element { float value{}; };
  template<bool F, unsigned D>
  constexpr degree_element exp(degree_element x, std::bool_constant<F>, std::integral_constant<unsigned, D>) noexcept {
    return {x.value + float(D) + (F ? 10.f : 0.f)};
  }
  // Both protocols exist: the degree-aware overload must take precedence at 6.
  template<bool F> constexpr degree_element exp(degree_element, std::bool_constant<F>) noexcept { return {-1000.f}; }
  struct degree_batch { float value{}; };
  template<bool F, unsigned D>
  constexpr degree_batch exp(degree_batch x, std::bool_constant<F>, std::integral_constant<unsigned, D>) noexcept {
    return {x.value + float(D) + (F ? 10.f : 0.f)};
  }
  template<bool F, unsigned D, std::size_t N>
  constexpr auto exp(std::array<degree_batch, N> const & input,
      std::bool_constant<F> f, std::integral_constant<unsigned, D> d) noexcept {
    auto result = input;
    for (auto & x : result) { x = exp(x, f, d); x.value += 100.f; }
    return result;
  }
}
namespace {
  using word = std::uint32_t;
  using scalar = native::simd<float, 1, native::scalar>;
#if defined(__wasm__)
  constexpr auto architecture = native::feature_closure(native::wasm_feature::simd128);
#else
  constexpr auto architecture = test_arch;
#endif
  template<unsigned N> using vector = native::simd<float, N, architecture>;
  template<class T> using register_type = std::conditional_t<std::same_as<T, float>, scalar, T>;
  template<class T> constexpr auto lanes = register_type<T>::lanes;
  constexpr word bits(float x) { return std::bit_cast<word>(x); }
  constexpr float value(word x) { return std::bit_cast<float>(x); }
  constexpr word magnitude(word x) { return x & 0x7fffffffu; }
  constexpr bool nan(word x) { return magnitude(x) > 0x7f800000u; }
  constexpr bool same(word x, word y) { return x == y || (nan(x) && nan(y)); }
  constexpr std::uint64_t ulp(word x, word y) { return x > y ? std::uint64_t(x) - y : std::uint64_t(y) - x; }
  void require(bool result, char const * message) {
    if (!result) { std::fprintf(stderr, "exp degree: %s\n", message); std::abort(); }
  }
  template<unsigned D, class T> concept math_argument = requires(T const & x) { math::exp<false, D>(x); };
  template<unsigned D, class T> concept wide_argument = requires(T const & x) { wide::exp<true, D>(x); };
  template<bool F, unsigned D, class T> concept native_argument = requires(T const & x) { native::exp<F, D>(x); };
  static_assert(!math_argument<0, float> && !math_argument<8, scalar>);
  static_assert(!math_argument<0, std::array<float, 0>> && !wide_argument<8, native::wide<scalar, 0>>);
  static_assert(!math_argument<6, double> && !math_argument<6, int> && !math_argument<6, std::tuple<float, float>>);
  static_assert(!native_argument<false, 0, native::wide<custom::degree_element, 0>>);
  static_assert(!native_argument<false, 8, native::wide<custom::degree_batch, 0>>);
  static_assert(native_argument<false, 6, native::wide<custom::plain, 2>>);
  static_assert(!native_argument<true, 6, native::wide<custom::plain, 2>>);
  static_assert(!native_argument<false, 5, native::wide<custom::plain, 2>>);
  static_assert(!native_argument<false, 7, native::wide<custom::plain, 0>>);
  static_assert(native_argument<true, 6, native::wide<custom::tagged, 2>>);
  static_assert(!native_argument<true, 5, native::wide<custom::tagged, 2>>);
  static_assert(!noexcept(native::exp(native::wide<custom::plain, 2>{})));
  static_assert(noexcept(native::exp<true>(native::wide<custom::tagged, 2>{})));

  template<unsigned D> void custom_adl() {
    constexpr native::wide<custom::degree_element, 2> x{custom::degree_element{2.f}, custom::degree_element{4.f}};
    constexpr native::wide<custom::degree_batch, 2> b{custom::degree_batch{2.f}, custom::degree_batch{4.f}};
    constexpr auto a = native::exp<true, D>(x);
    constexpr auto batch = native::exp<false, D>(b);
    static_assert(a.registers[0].value == 12.f + D && a.registers[1].value == 14.f + D);
    static_assert(batch.registers[0].value == 102.f + D && batch.registers[1].value == 104.f + D);
    auto runtime = native::exp<true, D>(x);
    require(runtime.registers[1].value == 14.f + D, "ADL degree tag was lost");
    if constexpr (D == 6) {
      constexpr native::wide<custom::plain, 2> p{custom::plain{2.f}, custom::plain{4.f}};
      constexpr native::wide<custom::tagged, 2> t{custom::tagged{2.f}, custom::tagged{4.f}};
      constexpr auto old = native::exp(p);
      constexpr auto tagged = native::exp<true>(t);
      static_assert(old.registers[1].value == 5.f && tagged.registers[1].value == 14.f);
    }
  }
  template<class T> constexpr auto words(T const & x) {
    if constexpr (std::same_as<T, float>) return std::array{bits(x)};
    else { std::array<word, T::lanes> result{}; x.store_bits(result.data()); return result; }
  }
  template<class T> constexpr T from_words(std::array<word, lanes<T>> const & x) {
    if constexpr (std::same_as<T, float>) return value(x[0]);
    else return T::load_bits(x.data());
  }
  template<class T> void equal(T const & a, T const & b) {
    auto x = words(a), y = words(b);
    for (std::size_t lane = 0; lane < lanes<T>; ++lane)
      require(same(x[lane], y[lane]), "shape, alias, default degree, or constexpr/runtime mismatch");
  }
  struct row { word input, reference; };
  std::vector<row> make_bank() {
    std::vector<word> inputs{0, 0x80000000u, 1, 0x80000001u, 0x007fffffu, 0x807fffffu,
      0x00800000u, 0x80800000u, 0x7f7fffffu, 0xff7fffffu, 0x7f800000u, 0xff800000u,
      0x7fc12345u, 0xffc12345u, 0x7f812345u, 0xff812345u};
    auto neighbors = [&](word center, int radius) {
      for (int d = -radius; d <= radius; ++d) inputs.push_back(center + word(d));
    };
    for (word boundary : {0xc2aeac50u, 0xc2af5dc3u, 0xc2d00000u, 0x42b0c0a6u, 0x42b17218u}) neighbors(boundary, 32);
    for (int n = -115; n <= 115; ++n) {
      neighbors(bits(float(double(n) * 0.69314718055994530942)), 2);
      neighbors(bits(float((double(n) + 0.5) * 0.69314718055994530942)), 2);
    }
    for (int i = -2048; i <= 2048; ++i) inputs.push_back(bits(float(i) * (0.346574f / 2048.f)));
    word state = 0xf481dea7u;
    auto next = [&] { state ^= state << 13; state ^= state >> 17; state ^= state << 5; return state; };
    for (unsigned i = 0; i < 8192; ++i) {
      auto w = next();
      inputs.push_back(bits(-80.f + float(w & 0x00ffffffu) * (160.f * 0x1p-24f)));
      if (i < 2048) inputs.push_back(w);
    }
    std::sort(inputs.begin(), inputs.end()); inputs.erase(std::unique(inputs.begin(), inputs.end()), inputs.end());
    std::vector<row> result; result.reserve(inputs.size());
#if defined(NATIVE_TEST_MPFR)
    mpfr_t x, y; mpfr_inits2(256, x, y, static_cast<mpfr_ptr>(nullptr));
#endif
    for (word w : inputs) {
      auto x_value = value(w); word expected;
      if (nan(w)) expected = 0x7fc00000u;
      else if (x_value < -104.f) expected = 0;
      else if (x_value >= 88.3762664794921875f) expected = 0x7f800000u;
      else {
#if defined(NATIVE_TEST_MPFR)
        mpfr_set_flt(x, x_value, MPFR_RNDN); mpfr_exp(y, x, MPFR_RNDN);
        expected = bits(mpfr_get_flt(y, MPFR_RNDN));
#else
        expected = bits(float(std::exp(double(x_value))));
#endif
      }
      result.push_back({w, expected});
    }
#if defined(NATIVE_TEST_MPFR)
    mpfr_clears(x, y, static_cast<mpfr_ptr>(nullptr));
#endif
    return result;
  }
  template<class T> float stage(float a, float b, float c) {
#if defined(__wasm__)
    if constexpr (register_type<T>::architecture != native::scalar) {
#pragma clang fp contract(off)
      volatile float product = a * b;
      return product + c;
    } else
#endif
      return std::fma(a, b, c);
  }
  // Frozen degree-seven graph, independent of the promoted polynomial selector.
  // Called only for bounded normal results, where scaling by 2^n is exact.
  template<class T> float historical(float x) {
    float n = std::nearbyint(x * 1.4426950408889634f);
    float r = stage<T>(n, -0x1.62e400p-1f, x);
    r = stage<T>(n, -0x1.7f7d1cp-20f, r);
    float y = stage<T>(r, 0x1.a1d714d7b1510dp-13f, 0x1.6da756e670ea6p-10f);
    y = stage<T>(r, y, 0x1.11105b3161a6fp-7f);
    y = stage<T>(r, y, 0x1.5554649b7487fp-5f);
    y = stage<T>(r, y, 0x1.555555c673724p-3f);
    y = stage<T>(r, y, 0x1.0000005c8dd89p-1f);
    y = stage<T>(r, y, 1.f); y = stage<T>(r, y, 1.f);
    return y * value(word(int(n) + 127) << 23);
  }
  constexpr std::array<double, 4> relative_limits{0.0572, 0.001965, 0.000102, 0.000003};
  std::array<std::uint64_t, 7> largest_ulp{};
  std::array<double, 7> largest_relative{};
  std::size_t checks = 0;
  template<bool F, unsigned D, class T> void inspect(word actual, row const & r) {
    auto x = value(r.input); word expected = r.reference;
    bool exact = false;
    if (!nan(r.input)) {
      if (x < (F ? -87.33654022216796875f : -104.f)) { expected = 0; exact = true; }
      else if (x >= 88.3762664794921875f) { expected = 0x7f800000u; exact = true; }
      else if (magnitude(r.input) == 0) { expected = 0x3f800000u; exact = true; }
      else if constexpr (native::test::exp_uses_single_factor<register_type<T>>)
        if (std::nearbyint(x * 1.4426950408889634f) <= -127.f) { expected = 0; exact = true; }
    }
    bool const bounded = x >= -80.f && x <= 80.f;
    auto const error = ulp(actual, expected);
    double const relative = !exact && bounded ? std::abs(double(value(actual)) / double(value(expected)) - 1.) : 0.;
    bool good;
    if (nan(expected)) good = nan(actual);
    else if (exact) good = actual == expected;
    else if (bounded) {
      if constexpr (D <= 4) good = actual < 0x7f800000u && relative <= relative_limits[D - 1];
      else good = actual < 0x7f800000u && error <= (D == 5 ? 2u : 1u);
      largest_ulp[D - 1] = std::max(largest_ulp[D - 1], error);
      largest_relative[D - 1] = std::max(largest_relative[D - 1], relative);
    } else good = actual < 0x7f800000u;
    if (!good) {
      std::fprintf(stderr, "exp degree=%u flush=%d input=%08x actual=%08x reference=%08x ulp=%llu relative=%.12g\n",
        D, F, r.input, actual, expected, static_cast<unsigned long long>(error), relative);
      std::abort();
    }
    if constexpr (D == 7) if (bounded) require(same(actual, bits(historical<T>(x))), "degree seven changed its historical graph");
    ++checks;
  }
  template<class T> constexpr auto constant_input() {
    constexpr std::array<float, 8> bank{-0.3f, 0.f, 0.3f, 0.5f, -2.f, 5.f, -7.f, 10.f};
    std::array<T, 2> result{};
    for (unsigned reg = 0; reg < 2; ++reg) {
      std::array<word, lanes<T>> w{};
      for (std::size_t lane = 0; lane < lanes<T>; ++lane) w[lane] = bits(bank[(reg * lanes<T> + lane) % bank.size()]);
      result[reg] = from_words<T>(w);
    }
    return result;
  }
  template<bool F, unsigned D, class T> void check_shapes(std::vector<row> const & rows) {
    using pack = std::array<T, 2>; using wide_pack = native::wide<T, 2>;
    using empty = std::array<T, 0>; using empty_wide = native::wide<T, 0>;
    static_assert(std::same_as<decltype(math::exp<F, D>(pack{})), pack>);
    static_assert(std::same_as<decltype(math::exp<F, D>(wide_pack{})), wide_pack>);
    static_assert(noexcept(math::exp<F, D>(pack{})));
    static_assert(math::exp<F, D>(empty{}).empty());
    static_assert(std::same_as<decltype(math::exp<F, D>(empty_wide{})), empty_wide>);
    static_assert(!math_argument<0, empty> && !wide_argument<8, empty_wide>);
    static_assert(bits(math::exp<F, D>(0.f)) == 0x3f800000u && bits(math::exp<F, D>(-0.f)) == 0x3f800000u);
    if constexpr (!std::same_as<T, float>) {
      static_assert(!native_argument<F, 0, T> && !native_argument<F, 8, empty>);
      static_assert(!native_argument<F, 0, empty_wide>);
      require(native::exp<F, D>(empty{}).empty(), "empty native array changed size");
      (void)native::exp<F, D>(empty_wide{});
    }
    constexpr auto cx = constant_input<T>();
    constexpr auto cy = math::exp<F, D>(cx);
    constexpr auto cw = math::exp<F, D>(wide_pack{cx});
    auto runtime = cx;
    for (auto & x : runtime) {
      auto w = words(x);
      for (auto & lane : w) { volatile word v = lane; lane = v; }
      x = from_words<T>(w);
    }
    auto runtime_y = math::exp<F, D>(runtime);
    for (unsigned reg = 0; reg < 2; ++reg) { equal(cy[reg], runtime_y[reg]); equal(cy[reg], cw.registers[reg]); }
    for (std::size_t base = 0; base < rows.size(); base += 2 * lanes<T>) {
      pack input{};
      for (unsigned reg = 0; reg < 2; ++reg) {
        std::array<word, lanes<T>> w{};
        for (std::size_t lane = 0; lane < lanes<T>; ++lane) w[lane] = rows[(base + reg * lanes<T> + lane) % rows.size()].input;
        input[reg] = from_words<T>(w);
      }
      auto y = math::exp<F, D>(input), alias = wide::exp<F, D>(input);
      auto wide_y = math::exp<F, D>(wide_pack{input});
      if constexpr (!std::same_as<T, float>) {
        auto native_y = native::exp<F, D>(input);
        auto native_wide = native::exp<F, D>(wide_pack{input});
        for (unsigned reg = 0; reg < 2; ++reg) {
          equal(y[reg], native_y[reg]); equal(y[reg], native_wide.registers[reg]);
          equal(y[reg], native::exp<F, D>(input[reg]));
        }
        if constexpr (D == 6) {
          auto default_y = native::exp<F>(input);
          auto default_wide = native::exp<F>(wide_pack{input});
          for (unsigned reg = 0; reg < 2; ++reg) { equal(y[reg], default_y[reg]); equal(y[reg], default_wide.registers[reg]); }
        }
      }
      if constexpr (D == 6) {
        auto default_y = math::exp<F>(input);
        for (unsigned reg = 0; reg < 2; ++reg) equal(y[reg], default_y[reg]);
      }
      for (unsigned reg = 0; reg < 2; ++reg) {
        equal(y[reg], alias[reg]); equal(y[reg], wide_y.registers[reg]); equal(y[reg], math::exp<F, D>(input[reg]));
        auto w = words(y[reg]);
        for (std::size_t lane = 0; lane < lanes<T>; ++lane) inspect<F, D, T>(w[lane], rows[(base + reg * lanes<T> + lane) % rows.size()]);
      }
    }
  }
  template<unsigned D, class T> void check_type(std::vector<row> const & rows) {
    check_shapes<false, D, T>(rows); check_shapes<true, D, T>(rows);
  }
  template<unsigned D> void run(std::vector<row> const & rows) {
    custom_adl<D>(); check_type<D, float>(rows); check_type<D, scalar>(rows);
#if defined(__wasm__)
    check_type<D, vector<4>>(rows);
#elif NATIVE_TEST_PROFILE != 0
    check_type<D, vector<1>>(rows); check_type<D, vector<2>>(rows);
    check_type<D, vector<3>>(rows); check_type<D, vector<4>>(rows);
#if NATIVE_TEST_PROFILE == 256 || NATIVE_TEST_PROFILE == 512
    check_type<D, vector<8>>(rows);
#endif
#if NATIVE_TEST_PROFILE == 512
    check_type<D, vector<16>>(rows);
#endif
#endif
    if constexpr (D < 7) {
      bool differs = false;
      for (auto const & row : rows) if (value(row.input) >= -0.35f && value(row.input) <= 0.35f)
        differs |= bits(math::exp<false, D>(value(row.input))) != bits(math::exp<false, D + 1>(value(row.input)));
      require(differs, "adjacent degree selectors never changed the polynomial result");
    }
  }
  void run_all(std::vector<row> const & rows) {
    run<1>(rows); run<2>(rows); run<3>(rows); run<4>(rows); run<5>(rows); run<6>(rows); run<7>(rows);
  }
}
int main() {
#if defined(__wasm__)
  auto rows = make_bank(); run_all(rows);
#else
  auto saved = native::test::read_fp_state(); std::vector<row> rows;
  { native::test::fp_scope scope(native::test::fp_mode::gradual); rows = make_bank(); }
  for (auto mode : {native::test::fp_mode::gradual, native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode); run_all(rows);
    require(scope.controls_match(), "degree selection changed FP controls");
  }
  require(native::test::read_fp_state() == saved, "fixture failed to restore FP state");
#endif
#if defined(NATIVE_TEST_MPFR)
  constexpr char reference[] = "MPFR256 RNDN";
#else
  constexpr char reference[] = "double libm";
#endif
  std::printf("exp degrees 1..7: %zu %s inputs; %zu checks; shapes/constexpr/default/ADL passed\n", rows.size(), reference, checks);
  for (unsigned i = 0; i < 7; ++i) std::printf("degree %u: maximum %llu ULP, relative %.12g on bounded normal outputs\n",
    i + 1, static_cast<unsigned long long>(largest_ulp[i]), largest_relative[i]);
}
