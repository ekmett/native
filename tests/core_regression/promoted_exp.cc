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
#include <utility>
#include <vector>
#include "support/fp_environment.h"
#include "support/profile.h"
#if NATIVE_TEST_IMPORT
import native.wide;
import native.math;
#else
#include <native/wide.h>
#include <native/wide_math.h>
#endif

#include "support/exp_scaling.h"

using scalar = native::simd<float, 1, native::scalar>;
using scalar_mask = typename scalar::mask_type;
using scalar_pack = std::array<scalar, 1>;

static_assert(wide::pack<std::array<float, 2>>);
static_assert(!wide::pack<std::tuple<float, scalar>>);
static_assert(!wide::pack<float> && !wide::pack<scalar>);
static_assert(wide::promotable<float const &> && wide::promotable<scalar>);
static_assert(!wide::promotable<std::tuple<>> && !wide::promotable<std::tuple<float, scalar>>);
static_assert(std::same_as<wide::canonical_t<float const &>, scalar_pack>);
static_assert(std::same_as<wide::canonical_t<scalar>, scalar_pack>);
static_assert(std::same_as<wide::canonical_t<std::array<float, 2>>, std::array<scalar, 2>>);
static_assert(std::same_as<wide::canonical_t<native::wide<scalar, 2>>, std::array<scalar, 2>>);
static_assert(std::same_as<decltype(math::exp(0.f)), float>);
static_assert(std::same_as<decltype(math::exp(scalar{})), scalar>);
static_assert(std::same_as<decltype(math::exp(std::array<float, 1>{})), std::array<float, 1>>);
static_assert(std::same_as<decltype(math::exp(std::array<float, 0>{})), std::array<float, 0>>);
static_assert(std::same_as<decltype(std::array{1.f, 2.f} == std::array{1.f, 2.f}), bool>);
static_assert(std::same_as<decltype(std::array{1.f, 2.f} < std::array{2.f, 1.f}), bool>);
static_assert(std::array{1.f, 2.f} == std::array{1.f, 2.f});
static_assert(std::array{1.f, 2.f} < std::array{2.f, 1.f});

template<class T> concept has_math_exp = requires(T const & x) { math::exp(x); };
template<class T> concept has_math_flush_exp = requires(T const & x) { math::exp<true>(x); };
template<class T> concept has_wide_exp = requires(T const & x) { wide::exp(x); };
template<class T> concept has_wide_flush_exp = requires(T const & x) { wide::exp<true>(x); };
template<class T> concept has_adl_exp = requires(T const & x) { exp(x); };
template<class T> concept has_adl_flush_exp = requires(T const & x) { exp<true>(x); };
template<class T> constexpr bool rejected_exp_tuple =
  !has_math_exp<T> && !has_math_flush_exp<T> && !has_wide_exp<T> &&
  !has_wide_flush_exp<T> && !has_adl_exp<T> && !has_adl_flush_exp<T>;
static_assert(rejected_exp_tuple<std::tuple<>>);
static_assert(rejected_exp_tuple<std::tuple<float>>);
static_assert(rejected_exp_tuple<std::tuple<float, float>>);
static_assert(rejected_exp_tuple<std::tuple<scalar, scalar>>);
static_assert(rejected_exp_tuple<std::tuple<float, scalar>>);
static_assert(has_math_exp<float> && has_math_flush_exp<float>);
static_assert(has_wide_exp<scalar> && has_wide_flush_exp<scalar>);
static_assert(has_math_exp<std::array<float, 0>> && has_math_flush_exp<std::array<float, 0>>);
static_assert(has_wide_exp<std::array<float, 0>> && has_wide_flush_exp<std::array<float, 0>>);

template<class F, class... P> concept can_map = requires(F f, P... p) { wide::map(f, p...); };
struct no_arguments { int operator()() const; };
struct void_result { void operator()(int) const; };
struct increment { constexpr int operator()(int x) const { return x + 1; } };
struct second_value { template<class T> T operator()(int, T x) const { return x; } };
static_assert(!can_map<no_arguments, std::array<int, 1>>);
static_assert(!can_map<void_result, std::array<int, 1>>);
static_assert(can_map<increment, std::array<int, 0>>);
static_assert(!can_map<increment, std::tuple<int>>);
static_assert(!can_map<second_value, std::array<int, 2>, std::array<int, 1>>);
static_assert(std::get<0>(wide::map(increment{}, std::array<int, 1>{{4}})) == 5);
template<class P> concept has_array_product = requires(P const & p) { p * p; };
template<class P> concept has_pointwise_product = requires(P const & p) { wide::mul(p, p); };
static_assert(!has_array_product<std::array<float, 1>> && !has_array_product<scalar_pack>);
static_assert(!has_pointwise_product<std::array<float, 1>> && has_pointwise_product<scalar_pack>);

template<class A, class B, class C> concept can_fma = requires(A const & a, B const & b, C const & c) {
  wide::fma(a, b, c);
};
using scalar_pair = std::array<scalar, 2>;
static_assert(can_fma<scalar_pair, scalar, scalar>);
static_assert(can_fma<scalar, scalar_pair, scalar>);
static_assert(can_fma<scalar, scalar, scalar_pair>);
static_assert(!can_fma<scalar_pair, scalar, float>);
static_assert(!can_fma<scalar, scalar_pair, float>);
static_assert(!can_fma<float, scalar, scalar_pair>);
static_assert(!can_fma<scalar_pair, std::array<scalar, 1>, scalar>);
static_assert(!can_fma<scalar_pair, std::tuple<scalar, scalar>, scalar>);
static_assert(!can_fma<scalar_pair, scalar, std::array<scalar, 3>>);
static_assert(!can_fma<scalar_pair, native::simd<double, 1, native::scalar>, scalar>);

static void require(bool value, char const * message) {
  if (!value) {
    std::fprintf(stderr, "%s\n", message);
    std::abort();
  }
}

static bool equal_bits(std::uint32_t left, std::uint32_t right) {
  // Only NaN class is stable; payload and sign are outside the exp contract.
  return left == right || ((left & 0x7fffffffu) > 0x7f800000u &&
                           (right & 0x7fffffffu) > 0x7f800000u);
}

static void check_word(float input, float actual, float expected) {
  auto a = std::bit_cast<std::uint32_t>(actual);
  auto e = std::bit_cast<std::uint32_t>(expected);
  if (!equal_bits(a, e)) {
    std::fprintf(stderr, "promoted exp mismatch: input=%08x actual=%08x expected=%08x\n",
      std::bit_cast<std::uint32_t>(input), a, e);
    std::abort();
  }
}

// Independent scalar form of wide_exp.cc's historical split-scale graph. It
// remains available to an import consumer without using hidden native names.
template<bool Flush, class V = scalar> static float reference(float x) {
  if (std::isnan(x)) return x;
  if (x < (Flush ? -87.33654022216796875f : -104.f)) return 0.f;
  // The general exp contract intentionally overflows at its first n=128 input.
  if (x >= 88.3762664794921875f) return std::bit_cast<float>(0x7f800000u);
  float r = x > 88.72283935546875f ? 88.72283935546875f : x;
  float n = std::nearbyint(r * 1.4426950408889634f);
  // Software scaling collapses exactly this band; native VSCALEF keeps its
  // existing subnormal results. Everywhere else retain the independent graph.
  if constexpr (native::test::exp_uses_single_factor<V>)
    if (n <= -127.f) return 0.f;
  r = std::fma(n, -0x1.62e400p-1f, r);
  r = std::fma(n, -0x1.7f7d1cp-20f, r);
  float y = std::fma(r, 0x1.a1d714d7b1510dp-13f, 0x1.6da756e670ea6p-10f);
  y = std::fma(r, y, 0x1.11105b3161a6fp-7f);
  y = std::fma(r, y, 0x1.5554649b7487fp-5f);
  y = std::fma(r, y, 0x1.555555c673724p-3f);
  y = std::fma(r, y, 0x1.0000005c8dd89p-1f);
  y = std::fma(r, y, 1.f);
  y = std::fma(r, y, 1.f);
  int exponent = static_cast<int>(n);
  int first = exponent < -126 ? -126 : exponent > 127 ? 127 : exponent;
  float low = std::bit_cast<float>(std::uint32_t(exponent - first + 127) << 23);
  float high = std::bit_cast<float>(std::uint32_t(first + 127) << 23);
  return (y * low) * high;
}

template<bool Flush, class V> static void check_vector(V actual, std::array<float, V::lanes> const & input) {
  std::array<float, V::lanes> output{};
  actual.storeu(output.data());
  for (std::size_t lane = 0; lane < V::lanes; ++lane)
    check_word(input[lane], output[lane], reference<Flush, V>(input[lane]));
}

static void shapes_and_masks() {
  float value = -2.f;
  auto promoted = wide::promote(value);
  static_assert(std::same_as<decltype(promoted), scalar_pack>);
  value = 4.f;
  check_word(-2.f, wide::demote<float>(promoted), -2.f);
  auto comparison = wide::cmp_lt(promoted, wide::promote(0.f));
  auto scalar_result = wide::demote<float>(comparison);
  static_assert(std::same_as<decltype(scalar_result), bool>);
  require(scalar_result, "scalar mask was not demoted to true");
  auto vector_result = wide::demote<scalar>(comparison);
  static_assert(std::same_as<decltype(vector_result), scalar_mask>);
  require(vector_result.to_bitset() == 1, "one-lane vector mask lost its shape");

  auto array_input = wide::promote(std::array{-1.f, 2.f});
  auto array_mask = wide::cmp_lt(array_input, wide::promote(std::array{0.f, 0.f}));
  auto standard_array = wide::demote<std::array<float, 2>>(array_mask);
  auto inverted = wide::demote<std::array<float, 2>>(wide::mask_not(array_mask));
  static_assert(std::same_as<decltype(standard_array), std::array<bool, 2>>);
  require(standard_array[0] && !standard_array[1], "pointwise array comparison changed order");
  require(!inverted[0] && inverted[1], "pointwise mask complement changed order");
  require(std::array{-1.f, 2.f} < std::array{0.f, 0.f}, "standard array ordering was changed");
  require(std::array{-1.f, 2.f} != std::array{0.f, 0.f}, "standard array equality was changed");
  unsigned calls = 0;
  auto empty_map = wide::map([&](int x) { ++calls; return x + 1; }, std::array<int, 0>{});
  require(empty_map.empty() && calls == 0, "empty map invoked its callable");

  require(math::exp(std::array<float, 0>{}).empty(), "empty array acquired an element");
  check_word(0.f, math::exp(std::array{0.f})[0], 1.f);
}

template<bool Packed, class P, class V>
static auto const & fma_argument(P const & pack, V const & value) {
  if constexpr (Packed) return pack;
  else return value;
}

template<class T> static void check_array_alignment(T const & value) {
  using batch = std::array<T, 3>;
  using nested = std::array<batch, 2>;
  struct embedded { unsigned char prefix; batch values; };
  static_assert(alignof(std::array<T, 1>) == alignof(T));
  static_assert(alignof(batch) == alignof(T));
  static_assert(alignof(nested) == alignof(T));
  static_assert(alignof(embedded) == alignof(T));
  auto check = [](auto const & values) {
    for (auto const & element : values)
      require(reinterpret_cast<std::uintptr_t>(&element) % alignof(T) == 0,
        "promoted exp array element is underaligned");
  };
  std::array<T, 1> singleton{value};
  batch values{value, value, value};
  nested rows{values, values};
  embedded object{7, values};
  check(singleton);
  check(values);
  for (auto const & row : rows) check(row);
  check(object.values);
}

// Each broadcast is an entire SIMD value: distinct lanes and distinct outer
// elements catch scalar splats, reordered arguments, and whole-pack broadcasts.
template<class V> static void fma_broadcasts() {
  using P = std::array<V, 2>;
  using lanes = std::array<float, V::lanes>;
  std::array<lanes, 2> a{}, b{}, c{};
  for (std::size_t lane = 0; lane < V::lanes; ++lane) {
    float step = static_cast<float>(lane + 1);
    // On even lanes a*b rounds to 1, but fused a*b-1 is exactly -2^-46.
    a[0][lane] = lane % 2 == 0 ? 0x1.000002p0f : 0.25f * step;
    b[0][lane] = lane % 2 == 0 ? 0x1.fffffcp-1f : -0.5f * step;
    c[0][lane] = lane % 2 == 0 ? -1.f : 0.125f * step;
    a[1][lane] = -3.25f + 0.5f * step;
    b[1][lane] = 2.f + 0.25f * step;
    c[1][lane] = 7.f - 0.125f * step;
  }
  V av = V::loadu(a[0].data()), bv = V::loadu(b[0].data()), cv = V::loadu(c[0].data());
  P ap{{av, V::loadu(a[1].data())}};
  P bp{{bv, V::loadu(b[1].data())}};
  P cp{{cv, V::loadu(c[1].data())}};
  auto check = [&]<unsigned Packed>(auto const & result) {
    auto element = [&]<std::size_t I> {
      lanes actual{};
      std::get<I>(result).storeu(actual.data());
      for (std::size_t lane = 0; lane < V::lanes; ++lane) {
        float expected = std::fma(a[(Packed & 1) ? I : 0][lane],
          b[(Packed & 2) ? I : 0][lane], c[(Packed & 4) ? I : 0][lane]);
        require(equal_bits(std::bit_cast<std::uint32_t>(actual[lane]),
          std::bit_cast<std::uint32_t>(expected)), "wide fma broadcast lost a lane, element, or fused rounding");
      }
    };
    element.template operator()<0>();
    element.template operator()<1>();
  };
  auto evaluate = [&]<unsigned Packed> {
    auto result = wide::fma(fma_argument<(Packed & 1) != 0>(ap, av),
      fma_argument<(Packed & 2) != 0>(bp, bv), fma_argument<(Packed & 4) != 0>(cp, cv));
    static_assert(std::same_as<decltype(result), P>);
    check.template operator()<Packed>(result);
  };
  // Every nonempty subset of argument positions is a pack.
  [&]<std::size_t... I>(std::index_sequence<I...>) {
    (evaluate.template operator()<static_cast<unsigned>(I + 1)>(), ...);
  }(std::make_index_sequence<7>{});

  auto empty_array = wide::fma(std::array<V, 0>{}, bv, cv);
  static_assert(std::same_as<decltype(empty_array), std::array<V, 0>>);
  require(empty_array.empty(), "broadcast fma changed empty array shape");
}

template<class V> static void check_fma_broadcasts() {
  using M = typename V::mask_type;
  check_array_alignment(V(0.f));
  check_array_alignment(M(false));
  static_assert(std::same_as<decltype(wide::constant_like(
    std::declval<std::array<V, 2> const &>(), 1.f)), V>);
  static_assert(std::same_as<decltype(wide::constant_like(
    std::declval<std::array<V, 0> const &>(), 1.f)), V>);
  fma_broadcasts<V>();
}

template<bool Flush, class V> static void samples(std::vector<std::uint32_t> const & words) {
  static_assert(rejected_exp_tuple<std::tuple<V, V>>);
  static_assert(rejected_exp_tuple<std::tuple<float, scalar, V>>);
  static_assert(std::same_as<wide::canonical_t<V>, std::array<V, 1>>);
  static_assert(std::same_as<decltype(math::exp<Flush>(std::array<V, 3>{})), std::array<V, 3>>);
  static_assert(std::same_as<decltype(math::exp<Flush>(native::wide<V, 3>{})), native::wide<V, 3>>);
  for (std::size_t base = 0; base < words.size(); base += V::lanes) {
    std::array<std::array<float, V::lanes>, 3> input{};
    for (std::size_t chain = 0; chain < input.size(); ++chain)
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        input[chain][lane] = std::bit_cast<float>(words[(base + chain * (V::lanes + 7) + lane) % words.size()]);
    std::array<V, 3> registers{V::loadu(input[0].data()), V::loadu(input[1].data()), V::loadu(input[2].data())};
    float x = input[0][0];
    scalar s(x);
    float expected = reference<Flush>(x);
    check_word(x, math::exp<Flush>(x), expected);
    check_vector<Flush>(math::exp<Flush>(s), std::array{x});
    check_vector<Flush>(math::exp<Flush>(registers[0]), input[0]);

    std::array<float, 3> scalar_input{input[0][0], input[1][0], input[2][0]};
    auto scalar_array = math::exp<Flush>(scalar_input);
    auto scalar_wide = wide::exp<Flush>(scalar_input);
    auto standard = math::exp<Flush>(registers);
    auto packed = wide::exp<Flush>(registers);
    auto legacy = math::exp<Flush>(native::wide<V, 3>{registers[0], registers[1], registers[2]});
    for (std::size_t chain = 0; chain < input.size(); ++chain) {
      check_word(scalar_input[chain], scalar_array[chain], reference<Flush>(scalar_input[chain]));
      check_word(scalar_input[chain], scalar_wide[chain], reference<Flush>(scalar_input[chain]));
      check_vector<Flush>(standard[chain], input[chain]);
      check_vector<Flush>(packed[chain], input[chain]);
      check_vector<Flush>(legacy.registers[chain], input[chain]);
    }
  }
}

int main() {
  std::vector<std::uint32_t> words{0u, 0x80000000u, 1u, 0x80000001u,
    0x007fffffu, 0x807fffffu, 0x00800000u, 0x80800000u,
    0x7f7fffffu, 0xff7fffffu, 0x7f800000u, 0xff800000u,
    0x7fc12345u, 0xffc12345u, 0x7f800001u, 0xff800001u, 0x43320000u};
  // c2af5dc3 is the first negative input with n=-127; 42b0c0a6 first has n=128.
  for (auto boundary : {0xc2aeac50u, 0xc2af5dc3u, 0xc2d00000u, 0x42b0c0a6u, 0x42b17218u})
    for (int offset = -128; offset <= 128; ++offset)
      words.push_back(boundary + static_cast<std::uint32_t>(offset));
  std::uint32_t seed = 0x379bae12u;
  for (unsigned i = 0; i < 4096; ++i) {
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
    words.push_back(seed);
    // Supplement random bit patterns with ordinary polynomial inputs.
    words.push_back(std::bit_cast<std::uint32_t>(float(int(seed & 0xffffu) - 32768) * 0x1p-8f));
  }
  auto saved = native::test::read_fp_state();
  for (auto mode : {native::test::fp_mode::gradual, native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);
    shapes_and_masks();
    check_fma_broadcasts<scalar>();
    samples<false, scalar>(words);
    samples<true, scalar>(words);
#if defined(__AVX2__)
    check_fma_broadcasts<native::simd<float, 2, native::avx2>>();
    check_fma_broadcasts<native::simd<float, 3, native::avx2>>();
    check_fma_broadcasts<native::simd<float, 8, native::avx2>>();
    samples<false, native::simd<float, 2, native::avx2>>(words);
    samples<true, native::simd<float, 2, native::avx2>>(words);
    samples<false, native::simd<float, 3, native::avx2>>(words);
    samples<true, native::simd<float, 3, native::avx2>>(words);
    samples<false, native::simd<float, 8, native::avx2>>(words);
    samples<true, native::simd<float, 8, native::avx2>>(words);
#elif defined(__ARM_NEON)
    check_fma_broadcasts<native::simd<float, 2, native::neon>>();
    check_fma_broadcasts<native::simd<float, 3, native::neon>>();
    check_fma_broadcasts<native::simd<float, 4, native::neon>>();
    samples<false, native::simd<float, 2, native::neon>>(words);
    samples<true, native::simd<float, 2, native::neon>>(words);
    samples<false, native::simd<float, 3, native::neon>>(words);
    samples<true, native::simd<float, 3, native::neon>>(words);
    samples<false, native::simd<float, 4, native::neon>>(words);
    samples<true, native::simd<float, 4, native::neon>>(words);
#endif
    require(scope.controls_match(), "promoted exp changed FP controls");
  }
  require(native::test::read_fp_state() == saved, "promoted exp fixture failed to restore FP state");
  std::printf("promoted exp shapes, masks, and %zu inputs per width/cutoff/FP mode passed\n", words.size());
}
