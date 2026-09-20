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
extern "C" void promoted_trig_reference(unsigned, float const *, float *, float *, float *, float *);

template<class P, class T> concept can_constant = requires(P const & p, T value) {
  wide::constant_like(p, value);
};
template<class P> concept has_pack_not = requires(P const & p) { !p; };
using scalar_bits = typename scalar::bits_type;
static_assert(can_constant<wide::array<scalar, 2>, float>);
static_assert(!can_constant<wide::array<scalar, 2>, double>);
static_assert(!can_constant<wide::array<scalar, 2>, std::uint32_t>);
static_assert(can_constant<wide::array<scalar_bits, 2>, std::uint32_t>);
static_assert(!can_constant<wide::array<scalar_bits, 2>, float>);
static_assert(!can_constant<wide::tuple<scalar, scalar_bits>, float>);
static_assert(!can_constant<wide::tuple<scalar, scalar_bits>, std::uint32_t>);
static_assert(!has_pack_not<wide::array<scalar, 2>>);
static_assert(!has_pack_not<wide::tuple<scalar, scalar>>);
static_assert(has_pack_not<wide::array<typename scalar::mask_type, 2>>);

static void require(bool value, char const * message) {
  if (!value) { std::fprintf(stderr, "%s\n", message); std::abort(); }
}
static void exact(float actual, float expected, char const * operation) {
  auto a = std::bit_cast<std::uint32_t>(actual), e = std::bit_cast<std::uint32_t>(expected);
  if (a != e) {
    std::fprintf(stderr, "%s mismatch actual=%08x expected=%08x\n", operation, a, e);
    std::abort();
  }
}
template<class V> static void exact_vector(V value, std::array<float, V::lanes> const & expected,
    char const * operation) {
  std::array<float, V::lanes> actual{};
  value.storeu(actual.data());
  for (std::size_t i = 0; i < V::lanes; ++i) exact(actual[i], expected[i], operation);
}

template<class Operation> static void empty_shapes(Operation operation) {
  auto a = operation(std::array<float, 0>{});
  auto b = operation(wide::array<float, 0>{});
  auto c = operation(std::tuple<>{});
  auto d = operation(wide::tuple<>{});
  static_assert(std::same_as<decltype(a), std::array<float, 0>>);
  static_assert(std::same_as<decltype(b), wide::array<float, 0>>);
  static_assert(std::same_as<decltype(c), std::tuple<>>);
  static_assert(std::same_as<decltype(d), wide::tuple<>>);
  require(a.empty() && b.values.empty(), "unary operation changed empty shape");
  (void)c; (void)d;
}

// Scalar values deliberately differ from the vector's first lane. Every
// operation must restore both outer family and each heterogeneous leaf shape.
template<class V, class Operation> static void unary_shapes(Operation operation, char const * name,
    float x, std::array<float, V::lanes> const & input, float expected_x,
    std::array<float, V::lanes> const & expected) {
  V v = V::loadu(input.data());
  scalar s(x);
  auto scalar_result = operation(x);
  auto one_lane = operation(s);
  auto vector_result = operation(v);
  static_assert(std::same_as<decltype(scalar_result), float>);
  static_assert(std::same_as<decltype(one_lane), scalar>);
  static_assert(std::same_as<decltype(vector_result), V>);
  exact(scalar_result, expected_x, name);
  exact_vector(one_lane, std::array{expected_x}, name);
  exact_vector(vector_result, expected, name);
  auto array_result = operation(std::array{x, x});
  auto wide_result = operation(wide::array<V, 2>{{v, v}});
  static_assert(std::same_as<decltype(array_result), std::array<float, 2>>);
  static_assert(std::same_as<decltype(wide_result), wide::array<V, 2>>);
  for (float value : array_result) exact(value, expected_x, name);
  for (V value : wide_result.values) exact_vector(value, expected, name);
  auto tuple_result = operation(std::tuple{x, s, v});
  auto wide_tuple = operation(wide::tuple<float, scalar, V>{x, s, v});
  static_assert(std::same_as<decltype(tuple_result), std::tuple<float, scalar, V>>);
  static_assert(std::same_as<decltype(wide_tuple), wide::tuple<float, scalar, V>>);
  exact(std::get<0>(tuple_result), expected_x, name);
  exact_vector(std::get<1>(tuple_result), std::array{expected_x}, name);
  exact_vector(std::get<2>(tuple_result), expected, name);
  exact(wide::get<0>(wide_tuple), expected_x, name);
  exact_vector(wide::get<1>(wide_tuple), std::array{expected_x}, name);
  exact_vector(wide::get<2>(wide_tuple), expected, name);
}

static constexpr auto sine = [](auto const & x) { return math::sin(x); };
static constexpr auto cosine = [](auto const & x) { return math::cos(x); };
static constexpr auto paired_sine = [](auto const & x) { return math::sincos(x).first; };
static constexpr auto paired_cosine = [](auto const & x) { return math::sincos(x).second; };
static constexpr auto flush = [](auto const & x) { return math::flush_to_zero(x); };

template<class V> static void trig_samples(std::vector<float> const & inputs) {
  using mixed = std::tuple<float, scalar, V>;
  static_assert(std::same_as<decltype(math::sincos(0.f)), std::pair<float, float>>);
  static_assert(std::same_as<decltype(math::sincos(std::declval<mixed const &>())), std::pair<mixed, mixed>>);
  static_assert(std::same_as<decltype(math::sincos(wide::tuple<>{})), std::pair<wide::tuple<>, wide::tuple<>>>);
  static_assert(std::same_as<decltype(math::sincos(std::array<float, 0>{})),
    std::pair<std::array<float, 0>, std::array<float, 0>>>);
  for (std::size_t base = 0; base < inputs.size(); base += V::lanes) {
    std::array<float, V::lanes> input{}, s{}, c{}, ps{}, pc{};
    for (std::size_t lane = 0; lane < V::lanes; ++lane) input[lane] = inputs[(base + lane) % inputs.size()];
    float x = inputs[(base + V::lanes + 1) % inputs.size()], xs{}, xc{}, xps{}, xpc{};
    promoted_trig_reference(static_cast<unsigned>(V::lanes), input.data(), s.data(), c.data(), ps.data(), pc.data());
    promoted_trig_reference(1, &x, &xs, &xc, &xps, &xpc);
    unary_shapes<V>(sine, "sin", x, input, xs, s);
    unary_shapes<V>(cosine, "cos", x, input, xc, c);
    unary_shapes<V>(paired_sine, "sincos sine", x, input, xps, ps);
    // Paired cosine intentionally retains the historical subtraction selection,
    // whose signed zero can differ from the standalone cosine graph.
    unary_shapes<V>(paired_cosine, "sincos cosine", x, input, xpc, pc);
  }
}

static constexpr std::uint32_t flushed(std::uint32_t word) {
  return (word & 0x7fffffffu) < 0x00800000u ? word & 0x80000000u : word;
}
template<class V> static void flush_samples(std::vector<std::uint32_t> const & words) {
  auto before = simd::test::read_fp_state();
  for (std::size_t base = 0; base < words.size(); base += V::lanes) {
    std::array<float, V::lanes> input{}, expected{};
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      auto word = words[(base + lane) % words.size()];
      input[lane] = std::bit_cast<float>(word);
      expected[lane] = std::bit_cast<float>(flushed(word));
    }
    auto x = words[(base + V::lanes + 1) % words.size()];
    unary_shapes<V>(flush, "flush_to_zero", std::bit_cast<float>(x), input,
      std::bit_cast<float>(flushed(x)), expected);
  }
  require(simd::test::read_fp_state() == before, "bitwise flushing changed FP flags or controls");
}

template<class V, class Operation, class Reference> static void primitive_samples(Operation operation,
    Reference reference, char const * name, bool positive = false) {
  constexpr std::array<float, 16> bank{-0.f, 0.f, -0.25f, 0.25f, -0.5f, 0.5f,
    -1.5f, 1.5f, -2.5f, 2.5f, -3.75f, 3.75f, -16.f, 16.f, -4095.5f, 4095.5f};
  empty_shapes(operation);
  for (std::size_t base = 0; base < bank.size(); ++base) {
    std::array<float, V::lanes> input{}, expected{};
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      input[lane] = bank[(base + lane) % bank.size()];
      if (positive) input[lane] = std::fabs(input[lane]);
      expected[lane] = reference(input[lane]);
    }
    float x = bank[(base + V::lanes + 1) % bank.size()];
    if (positive) x = std::fabs(x);
    unary_shapes<V>(operation, name, x, input, reference(x), expected);
  }
}

template<class A, class B> concept can_max = requires(A a, B b) { wide::max(a, b); };
template<class M, class A, class B> concept can_select = requires(M m, A a, B b) { wide::select(m, a, b); };
template<class V> static void primitive_broadcasts() {
  using P = wide::array<V, 2>;
  using M = typename V::mask_type;
  static_assert(!can_max<P, wide::array<V, 1>>);
  static_assert(!can_max<P, float>);
  static_assert(!can_select<wide::array<M, 1>, P, V>);
  std::array<float, V::lanes> input{};
  for (std::size_t i = 0; i < V::lanes; ++i) input[i] = static_cast<float>(i) - 3.f;
  V x = V::loadu(input.data());
  P p{{x, -x}};
  auto low = wide::min(V(0.f), p), high = wide::max(p, V(0.f));
  auto mask = p < V(0.f);
  auto selected = wide::select(mask, V(8.f), p);
  auto sum = V(2.f) + p, difference = p - V(2.f), divided = p / V(2.f);
  auto scaled = wide::scaleb(p, V(2.f));
  auto merged = wide::masked_scaleb(mask, V(77.f), p, V(2.f));
  auto cleared = wide::masked_scaleb_zero(mask, p, V(2.f));
  static_assert(std::same_as<decltype(selected), P>);
  for (std::size_t element = 0; element < 2; ++element) {
    std::array<float, V::lanes> lo{}, hi{}, choice{}, plus{}, minus{}, quotient{};
    std::array<float, V::lanes> scale{}, merge{}, clear{};
    low.values[element].storeu(lo.data()); high.values[element].storeu(hi.data());
    selected.values[element].storeu(choice.data()); sum.values[element].storeu(plus.data());
    difference.values[element].storeu(minus.data()); divided.values[element].storeu(quotient.data());
    scaled.values[element].storeu(scale.data()); merged.values[element].storeu(merge.data());
    cleared.values[element].storeu(clear.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      float value = element == 0 ? input[lane] : -input[lane];
      exact(lo[lane], 0.f < value ? 0.f : value, "min broadcast");
      exact(hi[lane], value > 0.f ? value : 0.f, "max broadcast");
      exact(choice[lane], value < 0.f ? 8.f : value, "select broadcast");
      exact(plus[lane], 2.f + value, "addition broadcast");
      exact(minus[lane], value - 2.f, "subtraction broadcast");
      exact(quotient[lane], value / 2.f, "division broadcast");
      exact(scale[lane], value * 4.f, "scaleb broadcast");
      exact(merge[lane], value < 0.f ? value * 4.f : 77.f, "masked_scaleb inactive prior");
      exact(clear[lane], value < 0.f ? value * 4.f : 0.f, "masked_scaleb_zero inactive zero");
    }
  }
}

template<class V> static void bit_helpers_and_aliases() {
  using U = typename V::bits_type;
  using P = wide::array<V, 2>;
  constexpr std::array<std::uint32_t, 6> bank{
    0x80000000u, 0x7f800001u, 0xffc12345u, 0x01000001u, 0x00000001u, 0x7f800000u};
  std::array<std::uint32_t, V::lanes> words{};
  for (std::size_t lane = 0; lane < V::lanes; ++lane) words[lane] = bank[lane % bank.size()];
  auto before = simd::test::read_fp_state();
  V v = V::from_bits(U::load(words.data()));
  auto encoded = wide::bits(P{{v, v}});
  static_assert(std::same_as<decltype(encoded), wide::array<U, 2>>);
  auto integer_constant = wide::constant_like(encoded, std::uint32_t(0xfffffffeu));
  static_assert(std::same_as<decltype(integer_constant), U>);
  auto flipped = (encoded & integer_constant) ^ U(1u);
  auto shifted = wide::left<1>(flipped);
  auto roundtrip = wide::from_bits(encoded);
  auto equality = encoded == encoded;
  auto mask_words = wide::mask_bits<std::uint32_t>(equality);
  for (std::size_t element = 0; element < 2; ++element) {
    std::array<std::uint32_t, V::lanes> constants{}, altered{}, shift{}, restored{}, masks{};
    integer_constant.storeu(constants.data()); flipped.values[element].storeu(altered.data());
    shifted.values[element].storeu(shift.data()); roundtrip.values[element].store_bits(restored.data());
    mask_words.values[element].storeu(masks.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      auto expected = (words[lane] & 0xfffffffeu) ^ 1u;
      require(constants[lane] == 0xfffffffeu, "uint32 constant was narrowed through float");
      require(altered[lane] == expected && shift[lane] == (expected << 1), "wide integer bit helper changed bits");
      require(restored[lane] == words[lane], "wide bit roundtrip changed a payload or sign");
      require(masks[lane] == 0xffffffffu, "wide mask_bits failed full true lanes");
    }
  }
  require(simd::test::read_fp_state() == before, "wide bit helpers changed FP state");

  wide::array<V, 1> zeros{{V(-0.f)}};
  auto adl_sine = sin(zeros);
  auto aliased_sine = wide::sin(zeros);
  auto aliased_cosine = wide::cos(zeros);
  auto aliased_pair = wide::sincos(zeros);
  auto aliased_flush = wide::flush_to_zero(zeros);
  std::array<float, V::lanes> negative_zero{}, one{};
  negative_zero.fill(-0.f); one.fill(1.f);
  exact_vector(adl_sine.values[0], negative_zero, "ADL sin signed zero");
  exact_vector(aliased_sine.values[0], negative_zero, "wide::sin alias");
  exact_vector(aliased_cosine.values[0], one, "wide::cos alias");
  exact_vector(aliased_pair.first.values[0], negative_zero, "wide::sincos sine alias");
  exact_vector(aliased_pair.second.values[0], one, "wide::sincos cosine alias");
  exact_vector(aliased_flush.values[0], negative_zero, "wide::flush_to_zero alias");
}

template<class V> static void check_width(std::vector<float> const & trig, std::vector<std::uint32_t> const & words) {
  trig_samples<V>(trig);
  flush_samples<V>(words);
  primitive_samples<V>([](auto const & x) { return math::abs(x); }, [](float x) { return std::fabs(x); }, "abs");
  primitive_samples<V>([](auto const & x) { return math::sqrt(x); }, [](float x) { return std::sqrt(x); }, "sqrt", true);
  primitive_samples<V>([](auto const & x) { return math::floor(x); }, [](float x) { return std::floor(x); }, "floor");
  primitive_samples<V>([](auto const & x) { return math::ceil(x); }, [](float x) { return std::ceil(x); }, "ceil");
  primitive_samples<V>([](auto const & x) { return math::trunc(x); }, [](float x) { return std::trunc(x); }, "trunc");
  primitive_samples<V>([](auto const & x) { return math::round_even(x); }, [](float x) { return std::nearbyint(x); }, "round_even");
  primitive_broadcasts<V>();
  bit_helpers_and_aliases<V>();
}

int main() {
  std::vector<float> trig{0.f, -0.f, std::bit_cast<float>(1u), std::bit_cast<float>(0x80000001u),
    std::bit_cast<float>(0x007fffffu), std::bit_cast<float>(0x807fffffu), 8191.99951171875f, -8191.99951171875f};
  for (float center : {0.7853981852531433f, 1.5707963705062866f, 3.1415927410125732f,
                      6.2831854820251465f, 100.f, 4096.f, 8191.f}) {
    auto bits = std::bit_cast<std::uint32_t>(center);
    for (int offset = -3; offset <= 3; ++offset) {
      auto value = std::bit_cast<float>(bits + static_cast<std::uint32_t>(offset));
      trig.push_back(value); trig.push_back(-value);
    }
  }
  std::vector<std::uint32_t> words{0u, 0x80000000u, 1u, 0x80000001u, 0x007fffffu, 0x807fffffu,
    0x00800000u, 0x80800000u, 0x3f800000u, 0xbf800000u, 0x7f800000u, 0xff800000u,
    0x7fc12345u, 0xffc12345u, 0x7f800001u, 0xff800001u, 0x7fffffffu, 0xffffffffu};
  std::uint32_t seed = 0x419cab81u;
  for (unsigned i = 0; i < 1024; ++i) {
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
    words.push_back(seed);
    trig.push_back(float(int(seed & 0xffffu) - 32768) * 0.25f);
  }
  // Exclude the reducer's strict boundary, which the random sampler can hit.
  for (float & value : trig) if (value == -8192.f) value = -8191.75f;
  auto saved = simd::test::read_fp_state();
  for (auto mode : {simd::test::fp_mode::gradual, simd::test::fp_mode::flush}) {
    simd::test::fp_scope scope(mode);
    empty_shapes(sine); empty_shapes(cosine); empty_shapes(paired_sine); empty_shapes(paired_cosine); empty_shapes(flush);
    check_width<scalar>(trig, words);
#if defined(__AVX2__)
    check_width<simd::vec<float, 2, simd::avx2>>(trig, words);
    check_width<simd::vec<float, 3, simd::avx2>>(trig, words);
    check_width<simd::vec<float, 4, simd::avx2>>(trig, words);
    check_width<simd::vec<float, 8, simd::avx2>>(trig, words);
#elif defined(__ARM_NEON)
    check_width<simd::vec<float, 4, simd::neon>>(trig, words);
#endif
    require(scope.controls_match(), "promoted math changed FP controls");
  }
  require(simd::test::read_fp_state() == saved, "promoted math fixture failed to restore FP state");
  std::printf("promoted trig/unary shapes passed; %zu trig inputs, %zu flush words per width/mode\n", trig.size(), words.size());
}
