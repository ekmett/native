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
  template<class T> constexpr std::size_t lanes = register_type<T>::lanes;
  constexpr std::uint64_t accuracy_ulp = 2;
  constexpr float from_bits(word x) { return std::bit_cast<float>(x); }
  constexpr word bits(float x) { return std::bit_cast<word>(x); }
  constexpr word magnitude(word x) { return x & 0x7fffffffu; }
  constexpr bool nan(word x) { return magnitude(x) > 0x7f800000u; }
  constexpr word ordered(word x) { return x >> 31 ? ~x : x ^ 0x80000000u; }
  constexpr std::uint64_t distance(word a, word b) {
    auto x = std::uint64_t(ordered(a)), y = std::uint64_t(ordered(b));
    return x > y ? x - y : y - x;
  }
  constexpr bool same(word a, word b) { return a == b || (nan(a) && nan(b)); }
  void require(bool value, char const * message) {
    if (!value) { std::fprintf(stderr, "base2: %s\n", message); std::abort(); }
  }
  template<class T> concept exponential_argument = requires(T const & x) {
    math::exp2(x); math::exp2<true>(x); wide::exp2(x); wide::exp2<true>(x);
  };
  template<class T> concept logarithm_argument = requires(T const & x) { math::log2(x); wide::log2(x); };
  template<class T> constexpr bool rejected =
    !requires(T const & x) { math::exp2(x); } &&
    !requires(T const & x) { math::exp2<true>(x); } &&
    !requires(T const & x) { wide::exp2(x); } &&
    !requires(T const & x) { wide::exp2<true>(x); } &&
    !requires(T const & x) { math::log2(x); } &&
    !requires(T const & x) { wide::log2(x); };
  static_assert(exponential_argument<float> && logarithm_argument<scalar>);
  static_assert(rejected<double> && rejected<int> && rejected<std::tuple<>>);
  static_assert(rejected<std::tuple<float, float>> && rejected<std::tuple<scalar, scalar>>);
  static_assert(rejected<std::array<double, 2>> && rejected<std::array<int, 0>>);
  static_assert(rejected<native::wide<double, 2>>);
  static_assert(rejected<native::simd<double, 1, native::scalar>>);
  static_assert(std::same_as<decltype(math::exp2(0.f)), float>);
  static_assert(std::same_as<decltype(math::log2(std::array<float, 0>{})), std::array<float, 0>>);
  static_assert(bits(math::exp2(0.f)) == 0x3f800000u);
  static_assert(bits(math::exp2(-0.f)) == 0x3f800000u);
  static_assert(bits(math::exp2(-126.f)) == 0x00800000u);
  static_assert(bits(math::exp2(127.f)) == 0x7f000000u);
  static_assert(bits(math::exp2(127.5f)) == 0x7f800000u);
  static_assert(bits(math::exp2<true>(-126.25f)) == 0);
  static_assert(bits(math::log2(1.f)) == 0);
  static_assert(bits(math::log2(0.f)) == 0xff800000u);
  static_assert(bits(math::log2(from_bits(0x807fffffu))) == 0xff800000u);
  static_assert(bits(math::log2(-1.f)) == 0x7fc00000u);
  static_assert(bits(math::log2(from_bits(0x00800000u))) == bits(-126.f));

  struct row { word input, exponential, logarithm; };
  // Fixed encodings cover classes and exact values independently of libm/MPFR.
  constexpr std::array fixed{
    row{0x00000000u, 0x3f800000u, 0xff800000u},
    row{0x80000000u, 0x3f800000u, 0xff800000u},
    row{0x00000001u, 0x3f800000u, 0xff800000u},
    row{0x80000001u, 0x3f800000u, 0xff800000u},
    row{0x007fffffu, 0x3f800000u, 0xff800000u},
    row{0x807fffffu, 0x3f800000u, 0xff800000u},
    row{0x00800000u, 0x3f800000u, 0xc2fc0000u},
    row{0x80800000u, 0x3f800000u, 0x7fc00000u},
    row{0x3f000000u, 0x3fb504f3u, 0xbf800000u},
    row{0x3f800000u, 0x40000000u, 0x00000000u},
    row{0xbf800000u, 0x3f000000u, 0x7fc00000u},
    row{0x40000000u, 0x40800000u, 0x3f800000u},
    row{0xc0000000u, 0x3e800000u, 0x7fc00000u},
    row{0x40800000u, 0x41800000u, 0x40000000u},
    row{0x7f7fffffu, 0x7f800000u, 0x43000000u},
    row{0xff7fffffu, 0x00000000u, 0x7fc00000u},
    row{0x7f800000u, 0x7f800000u, 0x7f800000u},
    row{0xff800000u, 0x00000000u, 0x7fc00000u},
    row{0x7fc12345u, 0x7fc00000u, 0x7fc00000u},
    row{0xff812345u, 0x7fc00000u, 0x7fc00000u}};

  row reference(word input) {
    auto const x = from_bits(input);
    word e = 0, l = 0;
    if (nan(input)) return {input, 0x7fc00000u, 0x7fc00000u};
    bool const exp_finite = x >= -150.f && x < 127.5f;
    bool const log_finite = input >= 0x00800000u && input < 0x7f800000u;
#if defined(NATIVE_TEST_MPFR)
    mpfr_t argument, result;
    mpfr_inits2(256, argument, result, static_cast<mpfr_ptr>(nullptr));
    mpfr_set_flt(argument, x, MPFR_RNDN);
    if (exp_finite) {
      mpfr_exp2(result, argument, MPFR_RNDN);
      e = bits(mpfr_get_flt(result, MPFR_RNDN));
    }
    if (log_finite) {
      mpfr_log2(result, argument, MPFR_RNDN);
      l = bits(mpfr_get_flt(result, MPFR_RNDN));
    }
    mpfr_clears(argument, result, static_cast<mpfr_ptr>(nullptr));
#else
    if (exp_finite) e = bits(float(std::exp2(double(x))));
    if (log_finite) l = bits(float(std::log2(double(x))));
#endif
    if (x >= 127.5f) e = 0x7f800000u;
    if (magnitude(input) < 0x00800000u) l = 0xff800000u;
    else if (input >> 31) l = 0x7fc00000u;
    else if (input == 0x7f800000u) l = input;
    return {input, e, l};
  }
  std::vector<row> make_bank() {
    std::vector<word> inputs;
    auto neighbors = [&](word center, int radius) {
      for (int d = -radius; d <= radius; ++d) inputs.push_back(center + word(d));
    };
    for (auto const & r : fixed) inputs.push_back(r.input);
    // exp2 reduction ties and exact integer powers, including both cutoffs.
    for (int n = -151; n <= 128; ++n) {
      neighbors(bits(float(n)), 4);
      neighbors(bits(float(n) + 0.5f), 16);
    }
    for (float x : {-150.f, -126.5f, -126.f, 127.f, 127.5f}) neighbors(bits(x), 256);
    // log2 near one, all exponent transitions, and the sqrt(2) mantissa split.
    neighbors(0x3f800000u, 2048);
    neighbors(0x3fb504f3u, 1024);
    // Worst neighborhoods from the larger fused and separate MPFR searches.
    neighbors(0x3fb500ceu, 8);
    neighbors(0x3fb10e99u, 8);
    for (unsigned e = 1; e < 255; ++e) {
      neighbors(e << 23, 4);
      neighbors((e << 23) | 0x003504f3u, 8);
      neighbors((e << 23) | 0x00400000u, 4);
    }
    word state = 0x952cad37u;
    auto next = [&] { state ^= state << 13; state ^= state >> 17; state ^= state << 5; return state; };
    for (unsigned i = 0; i < 16384; ++i) {
      auto value = next();
      inputs.push_back(value);
      // Uniform normal exp2 results and mantissas supplement random encodings.
      inputs.push_back(bits(-126.f + float(value & 0x00ffffffu) * (253.5f * 0x1p-24f)));
      inputs.push_back(0x3f000000u | (next() & 0x00ffffffu));
    }
    std::sort(inputs.begin(), inputs.end());
    inputs.erase(std::unique(inputs.begin(), inputs.end()), inputs.end());
    std::vector<row> result;
    result.reserve(inputs.size() + fixed.size());
    result.insert(result.end(), fixed.begin(), fixed.end());
    for (word x : inputs) result.push_back(reference(x));
    return result;
  }

  bool flush_environment = false;
  std::size_t checks = 0, unbudgeted_subnormals = 0;
  std::uint64_t largest_exp_ulp = 0, largest_log_ulp = 0;
  void failure(char const * operation, word actual, word expected, row const & r, bool policy) {
    std::fprintf(stderr, "%s input=%08x actual=%08x reference=%08x policy=%d environment_flush=%d ulp=%llu\n",
      operation, r.input, actual, expected, policy, flush_environment,
      static_cast<unsigned long long>(distance(actual, expected)));
    std::abort();
  }
  template<bool Flush, class T> void inspect_exp(word actual, row const & r) {
    word expected = r.exponential;
    float const x = from_bits(r.input);
    bool exact = false, unbudgeted = false;
    if (!nan(r.input)) {
      if (x < (Flush ? -126.f : -150.f)) { expected = 0; exact = true; }
      else if (x >= 127.5f) { expected = 0x7f800000u; exact = true; }
      else if constexpr (native::test::exp_uses_single_factor<register_type<T>>) {
        if (std::nearbyint(x) <= -127.f) { expected = 0; exact = true; }
      }
      if (!exact && magnitude(expected) < 0x00800000u) {
        // The inherited scaler does not promise gradual-subnormal accuracy.
        // Still reject negative, normal-large, infinite, or NaN results.
        unbudgeted = true;
      } else if (!exact) exact = x == std::nearbyint(x);
    }
    bool good;
    if (nan(expected)) good = nan(actual);
    else if (unbudgeted) good = actual <= 0x00800000u;
    else if (exact) good = actual == expected;
    else good = magnitude(actual) < 0x7f800000u && distance(actual, expected) <= accuracy_ulp;
    if (!good) failure("exp2", actual, expected, r, Flush);
    if (unbudgeted) ++unbudgeted_subnormals;
    else if (!exact && !nan(expected)) largest_exp_ulp = std::max(largest_exp_ulp, distance(actual, expected));
    ++checks;
  }
  void inspect_log(word actual, row const & r) {
    auto const expected = r.logarithm;
    bool const power = r.input >= 0x00800000u && r.input < 0x7f800000u && (r.input & 0x007fffffu) == 0;
    bool const exact = magnitude(expected) >= 0x7f800000u || magnitude(expected) == 0 || power;
    bool const good = exact ? actual == expected : magnitude(actual) < 0x7f800000u && distance(actual, expected) <= accuracy_ulp;
    if (!good) failure("log2", actual, expected, r, false);
    if (!exact) largest_log_ulp = std::max(largest_log_ulp, distance(actual, expected));
    ++checks;
  }
  template<class T> constexpr auto words(T const & value) {
    if constexpr (std::same_as<T, float>) return std::array{bits(value)};
    else { std::array<word, T::lanes> result{}; value.store_bits(result.data()); return result; }
  }
  template<class T> constexpr T from_words(std::array<word, lanes<T>> const & value) {
    if constexpr (std::same_as<T, float>) return from_bits(value[0]);
    else return T::load_bits(value.data());
  }
  template<class T> void equal(T const & a, T const & b) {
    auto x = words(a), y = words(b);
    for (std::size_t lane = 0; lane < lanes<T>; ++lane)
      require(same(x[lane], y[lane]), "public shapes, aliases or constexpr/runtime results differ");
  }
  template<class T, std::size_t N> constexpr auto constexpr_inputs(bool exponential) {
    constexpr std::array<float, 8> exp_values{-125.5f, -4.5f, -0.5f, -0.f, 0.5f, 1.5f, 7.5f, 126.5f};
    constexpr std::array<word, 12> log_values{0x3f400000u, 0x3f7fffffu, 0x3f800000u, 0x3f800001u,
      0x3fb504f2u, 0x3fb504f3u, 0x3fb504f4u, 0x3fc00000u,
      0x40000000u, 0x71800000u, 0x03800000u, 0x3f3504f3u};
    std::array<T, N> input{};
    for (std::size_t reg = 0; reg < N; ++reg) {
      std::array<word, lanes<T>> value{};
      for (std::size_t lane = 0; lane < lanes<T>; ++lane) {
        auto i = reg * lanes<T> + lane;
        value[lane] = exponential ? bits(exp_values[i % exp_values.size()]) : log_values[i % log_values.size()];
      }
      input[reg] = from_words<T>(value);
    }
    return input;
  }
  template<class T, std::size_t N> void check_constexpr() {
    constexpr auto ex = constexpr_inputs<T, N>(true), lx = constexpr_inputs<T, N>(false);
    constexpr auto e = math::exp2(ex), f = math::exp2<true>(ex), l = math::log2(lx);
    constexpr auto we = math::exp2(native::wide<T, N>{ex});
    constexpr auto wl = math::log2(native::wide<T, N>{lx});
    std::array<T, N> runtime_ex{}, runtime_lx{};
    for (std::size_t reg = 0; reg < N; ++reg) {
      auto a = words(ex[reg]), b = words(lx[reg]);
      for (std::size_t lane = 0; lane < lanes<T>; ++lane) {
        // Force runtime loads so constexpr/runtime graph comparison is real.
        volatile word x = a[lane], y = b[lane];
        a[lane] = x; b[lane] = y;
      }
      runtime_ex[reg] = from_words<T>(a); runtime_lx[reg] = from_words<T>(b);
    }
    auto re = math::exp2(runtime_ex), rf = math::exp2<true>(runtime_ex), rl = math::log2(runtime_lx);
    for (std::size_t reg = 0; reg < N; ++reg) {
      equal(e[reg], re[reg]); equal(f[reg], rf[reg]); equal(l[reg], rl[reg]);
      equal(e[reg], we.registers[reg]); equal(l[reg], wl.registers[reg]);
      equal(e[reg], math::exp2(runtime_ex[reg])); equal(l[reg], math::log2(runtime_lx[reg]));
    }
  }
  template<bool Flush, class T, std::size_t N> void check_shapes(std::vector<row> const & rows) {
    using pack = std::array<T, N>;
    using wide_pack = native::wide<T, N>;
    static_assert(std::same_as<decltype(math::exp2<Flush>(pack{})), pack>);
    static_assert(std::same_as<decltype(math::log2(pack{})), pack>);
    static_assert(std::same_as<decltype(math::exp2<Flush>(wide_pack{})), wide_pack>);
    static_assert(std::same_as<decltype(math::log2(wide_pack{})), wide_pack>);
    static_assert(noexcept(math::exp2<Flush>(pack{})) && noexcept(math::log2(wide_pack{})));
    for (std::size_t base = 0; base < rows.size(); base += N * lanes<T>) {
      pack input{};
      for (std::size_t reg = 0; reg < N; ++reg) {
        std::array<word, lanes<T>> value{};
        for (std::size_t lane = 0; lane < lanes<T>; ++lane)
          value[lane] = rows[(base + reg * lanes<T> + lane) % rows.size()].input;
        input[reg] = from_words<T>(value);
      }
      auto e = math::exp2<Flush>(input), l = math::log2(input);
      auto we = math::exp2<Flush>(wide_pack{input}), wl = math::log2(wide_pack{input});
      auto ae = wide::exp2<Flush>(input), al = wide::log2(input);
      if constexpr (!std::same_as<T, float>) {
        static_assert(std::same_as<decltype(native::exp2<Flush>(pack{})), pack>);
        static_assert(std::same_as<decltype(native::log2(wide_pack{})), wide_pack>);
        static_assert(noexcept(native::exp2<Flush>(pack{})) && noexcept(native::log2(wide_pack{})));
        auto ne = native::exp2<Flush>(input), nl = native::log2(input);
        auto nwe = native::exp2<Flush>(wide_pack{input}), nwl = native::log2(wide_pack{input});
        for (std::size_t reg = 0; reg < N; ++reg) {
          equal(e[reg], ne[reg]); equal(l[reg], nl[reg]);
          equal(e[reg], nwe.registers[reg]); equal(l[reg], nwl.registers[reg]);
          equal(e[reg], native::exp2<Flush>(input[reg])); equal(l[reg], native::log2(input[reg]));
        }
      }
      for (std::size_t reg = 0; reg < N; ++reg) {
        equal(e[reg], we.registers[reg]); equal(l[reg], wl.registers[reg]);
        equal(e[reg], ae[reg]); equal(l[reg], al[reg]);
        equal(e[reg], math::exp2<Flush>(input[reg])); equal(l[reg], math::log2(input[reg]));
        auto ew = words(e[reg]), lw = words(l[reg]);
        for (std::size_t lane = 0; lane < lanes<T>; ++lane) {
          auto const & r = rows[(base + reg * lanes<T> + lane) % rows.size()];
          inspect_exp<Flush, T>(ew[lane], r); inspect_log(lw[lane], r);
        }
      }
    }
  }
  template<class T> void check_type(std::vector<row> const & rows) {
    using empty = std::array<T, 0>;
    using empty_wide = native::wide<T, 0>;
    static_assert(math::exp2(empty{}).empty() && math::exp2<true>(empty{}).empty() && math::log2(empty{}).empty());
    static_assert(std::same_as<decltype(math::exp2(empty_wide{})), empty_wide>);
    (void)math::exp2<true>(empty_wide{}); (void)math::log2(empty_wide{});
    if constexpr (!std::same_as<T, float>) {
      require(native::exp2(empty{}).empty() && native::exp2<true>(empty{}).empty() && native::log2(empty{}).empty(), "empty native array changed size");
      (void)native::exp2(empty_wide{}); (void)native::exp2<true>(empty_wide{}); (void)native::log2(empty_wide{});
    }
    check_constexpr<T, 1>(); check_constexpr<T, 2>(); check_constexpr<T, 6>();
    check_shapes<false, T, 1>(rows); check_shapes<true, T, 1>(rows);
    check_shapes<false, T, 2>(rows); check_shapes<true, T, 2>(rows);
    check_shapes<false, T, 6>(rows); check_shapes<true, T, 6>(rows);
  }
  void run(std::vector<row> const & rows) {
    check_type<float>(rows); check_type<scalar>(rows);
#if defined(__wasm__)
    check_type<vector<4>>(rows);
#elif NATIVE_TEST_PROFILE != 0
    check_type<vector<1>>(rows); check_type<vector<2>>(rows);
    check_type<vector<3>>(rows); check_type<vector<4>>(rows);
#if NATIVE_TEST_PROFILE == 256 || NATIVE_TEST_PROFILE == 512
    check_type<vector<8>>(rows);
#endif
#if NATIVE_TEST_PROFILE == 512
    check_type<vector<16>>(rows);
#endif
#endif
  }
}
int main() {
#if defined(__wasm__)
  auto rows = make_bank(); run(rows);
#else
  auto saved = native::test::read_fp_state();
  std::vector<row> rows;
  { native::test::fp_scope scope(native::test::fp_mode::gradual); rows = make_bank(); }
  for (auto mode : {native::test::fp_mode::gradual, native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);
    flush_environment = mode == native::test::fp_mode::flush;
    run(rows);
    require(scope.controls_match(), "exp2/log2 changed FP controls");
  }
  require(native::test::read_fp_state() == saved, "fixture failed to restore FP state");
#endif
#if defined(NATIVE_TEST_MPFR)
  constexpr char reference_name[] = "MPFR256 RNDN";
#else
  constexpr char reference_name[] = "double libm";
#endif
  std::printf("base2: %zu sampled %s inputs; %zu checks; exp2 maximum %llu ULP, log2 maximum %llu ULP; %zu unbudgeted subnormal results; shapes/constexpr passed\n",
    rows.size(), reference_name, checks, static_cast<unsigned long long>(largest_exp_ulp),
    static_cast<unsigned long long>(largest_log_ulp), unbudgeted_subnormals);
}
