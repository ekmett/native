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
#if SIMD_TEST_IMPORT
import simd.wide;
#else
#include <simd/wide.h>
#include <simd/wide_math.h>
#endif

using scalar = simd::vec<float, 1, simd::scalar>;
using scalar_mask = typename scalar::mask_type;
using scalar_pack = wide::array<scalar, 1>;

static_assert(wide::pack<wide::array<float, 2>>);
static_assert(wide::pack<wide::tuple<float, scalar>>);
static_assert(!wide::pack<float> && !wide::pack<scalar>);
static_assert(wide::promotable<float const &> && wide::promotable<scalar>);
static_assert(std::same_as<wide::canonical_t<float const &>, scalar_pack>);
static_assert(std::same_as<wide::canonical_t<scalar>, scalar_pack>);
static_assert(std::same_as<wide::canonical_t<std::array<float, 2>>, wide::array<scalar, 2>>);
static_assert(std::same_as<wide::canonical_t<std::tuple<float, scalar>>, wide::tuple<scalar, scalar>>);
static_assert(std::same_as<wide::canonical_t<wide::array<float, 2>>, wide::array<scalar, 2>>);
static_assert(std::same_as<wide::canonical_t<wide::tuple<float, scalar>>, wide::tuple<scalar, scalar>>);
static_assert(std::same_as<decltype(math::exp(0.f)), float>);
static_assert(std::same_as<decltype(math::exp(scalar{})), scalar>);
static_assert(std::same_as<decltype(math::exp(std::array<float, 1>{})), std::array<float, 1>>);
static_assert(std::same_as<decltype(math::exp(wide::array<float, 1>{})), wide::array<float, 1>>);
static_assert(std::same_as<decltype(math::exp(std::tuple<float>{})), std::tuple<float>>);
static_assert(std::same_as<decltype(math::exp(wide::tuple<float>{})), wide::tuple<float>>);
static_assert(std::same_as<decltype(math::exp(std::array<float, 0>{})), std::array<float, 0>>);
static_assert(std::same_as<decltype(math::exp(wide::array<float, 0>{})), wide::array<float, 0>>);
static_assert(std::same_as<decltype(math::exp(std::tuple<>{})), std::tuple<>>);
static_assert(std::same_as<decltype(math::exp(wide::tuple<>{})), wide::tuple<>>);

template<class F, class... P> concept can_map = requires(F f, P... p) { wide::map(f, p...); };
struct no_arguments { int operator()() const; };
struct void_result { void operator()(int) const; };
struct increment { constexpr int operator()(int x) const { return x + 1; } };
struct second_value { template<class T> T operator()(int, T x) const { return x; } };
static_assert(!can_map<no_arguments, wide::array<int, 1>>);
static_assert(!can_map<void_result, wide::array<int, 1>>);
static_assert(can_map<no_arguments, wide::tuple<>>);
static_assert(can_map<increment, wide::array<int, 0>>);
static_assert(!can_map<second_value, wide::array<int, 2>, wide::tuple<int, float>>);
static_assert(!can_map<second_value, wide::array<int, 2>, wide::array<int, 1>>);
static_assert(wide::get<0>(wide::map(increment{}, wide::array<int, 1>{{4}})) == 5);
template<class P> concept has_pack_product = requires(P const & p) { p * p; };
static_assert(!has_pack_product<wide::array<float, 1>>);
static_assert(has_pack_product<scalar_pack>);

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

// Independent scalar form of reference.h's historical split-scale graph. It
// remains available to an import consumer without using hidden native names.
template<bool Flush> static float reference(float x) {
  if (std::isnan(x)) return x;
  if (x < (Flush ? -87.33654022216796875f : -104.f)) return 0.f;
  float r = x > 88.72283935546875f ? 88.72283935546875f : x;
  float n = std::nearbyint(r * 1.4426950408889634f);
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
    check_word(input[lane], output[lane], reference<Flush>(input[lane]));
}

static void shapes_and_masks() {
  float value = -2.f;
  auto promoted = wide::promote(value);
  static_assert(std::same_as<decltype(promoted), scalar_pack>);
  value = 4.f;
  check_word(-2.f, wide::demote<float>(promoted), -2.f);
  auto scalar_result = wide::demote<float>(promoted < wide::promote(0.f));
  static_assert(std::same_as<decltype(scalar_result), bool>);
  require(scalar_result, "scalar mask was not demoted to true");
  auto vector_result = wide::demote<scalar>(promoted < wide::promote(0.f));
  static_assert(std::same_as<decltype(vector_result), scalar_mask>);
  require(vector_result.to_bitset() == 1, "one-lane vector mask lost its shape");

  auto array_input = wide::promote(std::array{-1.f, 2.f});
  auto array_mask = array_input < wide::promote(std::array{0.f, 0.f});
  auto standard_array = wide::demote<std::array<float, 2>>(array_mask);
  auto wide_array = wide::demote<wide::array<float, 2>>(array_mask);
  static_assert(std::same_as<decltype(standard_array), std::array<bool, 2>>);
  static_assert(std::same_as<decltype(wide_array), wide::array<bool, 2>>);
  require(standard_array[0] && !standard_array[1], "array mask changed order");
  require(wide::get<0>(wide_array) && !wide::get<1>(wide_array), "wide array mask changed order");

  auto tuple_input = wide::promote(std::tuple{-1.f, scalar(2.f)});
  auto tuple_mask = tuple_input < wide::promote(std::tuple{0.f, scalar(0.f)});
  auto standard_tuple = wide::demote<std::tuple<float, scalar>>(tuple_mask);
  auto wide_tuple = wide::demote<wide::tuple<float, scalar>>(tuple_mask);
  static_assert(std::same_as<decltype(standard_tuple), std::tuple<bool, scalar_mask>>);
  static_assert(std::same_as<decltype(wide_tuple), wide::tuple<bool, scalar_mask>>);
  require(std::get<0>(standard_tuple) && std::get<1>(standard_tuple).to_bitset() == 0,
    "tuple comparison used a whole-tuple ordering");
  require(wide::get<0>(wide_tuple) && wide::get<1>(wide_tuple).to_bitset() == 0,
    "wide tuple mask demotion lost element shape");

  require(math::exp(std::array<float, 0>{}).empty(), "empty array acquired an element");
  require(math::exp(wide::array<float, 0>{}).values.empty(), "empty wide array acquired an element");
  (void)math::exp(std::tuple<>{});
  (void)math::exp(wide::tuple<>{});
  check_word(0.f, math::exp(std::array{0.f})[0], 1.f);
  check_word(0.f, wide::get<0>(math::exp(wide::array<float, 1>{{0.f}})), 1.f);
  check_word(0.f, std::get<0>(math::exp(std::tuple{0.f})), 1.f);
  check_word(0.f, wide::get<0>(math::exp(wide::tuple<float>{0.f})), 1.f);
}

template<bool Flush, class V> static void samples(std::vector<std::uint32_t> const & words) {
  using standard_mixed = std::tuple<float, scalar, V>;
  using wide_mixed = wide::tuple<float, scalar, V>;
  static_assert(std::same_as<decltype(math::exp<Flush>(std::declval<standard_mixed>())), standard_mixed>);
  static_assert(std::same_as<decltype(math::exp<Flush>(std::declval<wide_mixed>())), wide_mixed>);
  static_assert(std::same_as<wide::canonical_t<V>, wide::array<V, 1>>);
  static_assert(std::same_as<decltype(math::exp<Flush>(simd::wide<V, 2>{})), simd::wide<V, 2>>);
  for (std::size_t base = 0; base < words.size(); base += V::lanes) {
    std::array<float, V::lanes> input{};
    for (std::size_t lane = 0; lane < V::lanes; ++lane)
      input[lane] = std::bit_cast<float>(words[(base + lane) % words.size()]);
    float x = input[0];
    scalar s(x);
    V v = V::loadu(input.data());
    float expected = reference<Flush>(x);
    check_word(x, math::exp<Flush>(x), expected);
    check_vector<Flush>(math::exp<Flush>(s), std::array{x});
    check_vector<Flush>(math::exp<Flush>(v), input);

    auto standard = math::exp<Flush>(standard_mixed{x, s, v});
    check_word(x, std::get<0>(standard), expected);
    check_vector<Flush>(std::get<1>(standard), std::array{x});
    check_vector<Flush>(std::get<2>(standard), input);
    auto packed = math::exp<Flush>(wide_mixed{x, s, v});
    check_word(x, wide::get<0>(packed), expected);
    check_vector<Flush>(wide::get<1>(packed), std::array{x});
    check_vector<Flush>(wide::get<2>(packed), input);

    auto array = math::exp<Flush>(std::array{x, x});
    for (float result : array) check_word(x, result, expected);
    auto wide_array = math::exp<Flush>(wide::array<V, 2>{{v, v}});
    for (auto result : wide_array.values) check_vector<Flush>(result, input);
    auto legacy = math::exp<Flush>(simd::wide<V, 2>{v, v});
    for (auto result : legacy.registers) check_vector<Flush>(result, input);
  }
}

int main() {
  std::vector<std::uint32_t> words{0u, 0x80000000u, 1u, 0x80000001u,
    0x007fffffu, 0x807fffffu, 0x00800000u, 0x80800000u,
    0x7f7fffffu, 0xff7fffffu, 0x7f800000u, 0xff800000u,
    0x7fc12345u, 0xffc12345u, 0x7f800001u, 0xff800001u};
  for (auto boundary : {0xc2aeac50u, 0xc2d00000u, 0x42b17218u})
    for (int offset = -128; offset <= 128; ++offset)
      words.push_back(boundary + static_cast<std::uint32_t>(offset));
  std::uint32_t seed = 0x379bae12u;
  for (unsigned i = 0; i < 4096; ++i) {
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
    words.push_back(seed);
    // Supplement random bit patterns with ordinary polynomial inputs.
    words.push_back(std::bit_cast<std::uint32_t>(float(int(seed & 0xffffu) - 32768) * 0x1p-8f));
  }
  auto saved = simd::test::read_fp_state();
  for (auto mode : {simd::test::fp_mode::gradual, simd::test::fp_mode::flush}) {
    simd::test::fp_scope scope(mode);
    shapes_and_masks();
    samples<false, scalar>(words);
    samples<true, scalar>(words);
#if defined(__AVX2__)
    samples<false, simd::vec<float, 8, simd::avx2>>(words);
    samples<true, simd::vec<float, 8, simd::avx2>>(words);
#elif defined(__ARM_NEON)
    samples<false, simd::vec<float, 4, simd::neon>>(words);
    samples<true, simd::vec<float, 4, simd::neon>>(words);
#endif
    require(scope.controls_match(), "promoted exp changed FP controls");
  }
  require(simd::test::read_fp_state() == saved, "promoted exp fixture failed to restore FP state");
  std::printf("promoted exp shapes, masks, and %zu inputs per width/cutoff/FP mode passed\n", words.size());
}
