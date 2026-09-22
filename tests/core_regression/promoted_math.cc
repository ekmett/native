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

using scalar = native::simd<float, 1, native::scalar>;
extern "C" void promoted_trig_reference(unsigned, float const *, float *, float *, float *, float *);

template<class P, class T> concept can_constant = requires(P const & p, T value) {
  wide::constant_like(p, value);
};
template<class P> concept has_pack_not = requires(P const & p) { wide::mask_not(p); };
using scalar_bits = typename scalar::bits_type;
static_assert(can_constant<std::array<scalar, 2>, float>);
static_assert(!can_constant<std::array<scalar, 2>, double>);
static_assert(!can_constant<std::array<scalar, 2>, std::uint32_t>);
static_assert(can_constant<std::array<scalar_bits, 2>, std::uint32_t>);
static_assert(!can_constant<std::array<scalar_bits, 2>, float>);
static_assert(!can_constant<std::tuple<scalar, scalar_bits>, float>);
static_assert(!can_constant<std::tuple<scalar, scalar_bits>, std::uint32_t>);
static_assert(!has_pack_not<std::array<scalar, 2>>);
static_assert(!has_pack_not<std::tuple<scalar, scalar>>);
static_assert(has_pack_not<std::array<typename scalar::mask_type, 2>>);

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

// Tuples are not promoted, including homogeneous and empty tuples.
template<class T> constexpr bool rejects_tuple_math =
  !requires(T const & x) { math::sin(x); } &&
  !requires(T const & x) { math::cos(x); } &&
  !requires(T const & x) { math::sincos(x); } &&
  !requires(T const & x) { math::flush_to_zero(x); } &&
  !requires(T const & x) { math::abs(x); } &&
  !requires(T const & x) { math::sqrt(x); } &&
  !requires(T const & x) { math::floor(x); } &&
  !requires(T const & x) { math::ceil(x); } &&
  !requires(T const & x) { math::trunc(x); } &&
  !requires(T const & x) { math::round_even(x); };
static_assert(rejects_tuple_math<std::tuple<>>);
static_assert(rejects_tuple_math<std::tuple<float>>);
static_assert(rejects_tuple_math<std::tuple<float, float>>);
static_assert(rejects_tuple_math<std::tuple<float, scalar>>);

template<class Operation> static void empty_shapes(Operation operation) {
  auto a = operation(std::array<float, 0>{});
  auto b = operation(native::wide<scalar, 0>{std::array<scalar, 0>{}});
  static_assert(std::same_as<decltype(a), std::array<float, 0>>);
  static_assert(std::same_as<decltype(b), native::wide<scalar, 0>>);
  require(a.empty() && b.registers.empty(), "unary operation changed empty shape");
}

// The two array chains have different inputs, so duplication or reordering is
// observable independently of the operation's scalar/SIMD return shape.
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
  auto array_result = operation(std::array{x, input[0]});
  auto vector_array = operation(std::array<V, 2>{{v, V(x)}});
  auto legacy = operation(native::wide<V, 2>{v, V(x)});
  static_assert(std::same_as<decltype(array_result), std::array<float, 2>>);
  static_assert(std::same_as<decltype(vector_array), std::array<V, 2>>);
  static_assert(std::same_as<decltype(legacy), native::wide<V, 2>>);
  exact(array_result[0], expected_x, name);
  exact(array_result[1], expected[0], name);
  std::array<float, V::lanes> broadcast_expected{};
  broadcast_expected.fill(expected_x);
  exact_vector(vector_array[0], expected, name);
  exact_vector(vector_array[1], broadcast_expected, name);
  exact_vector(legacy.registers[0], expected, name);
  exact_vector(legacy.registers[1], broadcast_expected, name);
}

static constexpr auto sine = [](auto const & x) { return math::sin(x); };
static constexpr auto cosine = [](auto const & x) { return math::cos(x); };
static constexpr auto paired_sine = [](auto const & x) { return math::sincos(x).first; };
static constexpr auto paired_cosine = [](auto const & x) { return math::sincos(x).second; };
static constexpr auto flush = [](auto const & x) { return math::flush_to_zero(x); };

template<class V> static void trig_samples(std::vector<float> const & inputs) {
  static_assert(rejects_tuple_math<std::tuple<V, V>>);
  static_assert(std::same_as<decltype(math::sincos(0.f)), std::pair<float, float>>);
  static_assert(std::same_as<decltype(math::sincos(std::declval<std::array<V, 2> const &>())), std::pair<std::array<V, 2>, std::array<V, 2>>>);

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
  auto before = native::test::read_fp_state();
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
  require(native::test::read_fp_state() == before, "bitwise flushing changed FP flags or controls");
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
  using P = std::array<V, 2>;
  using M = typename V::mask_type;
  static_assert(!can_max<P, std::array<V, 1>>);
  static_assert(!can_max<P, float>);
  static_assert(!can_select<std::array<M, 1>, P, V>);
  std::array<float, V::lanes> input{};
  for (std::size_t i = 0; i < V::lanes; ++i) input[i] = static_cast<float>(i) - 3.f;
  V x = V::loadu(input.data());
  P p{{x, -x}};
  auto low = wide::min(V(0.f), p), high = wide::max(p, V(0.f));
  auto mask = wide::cmp_lt(p, V(0.f));
  auto inverted = wide::mask_not(mask);
  std::array comparisons{wide::cmp_eq(p, V(0.f)), wide::cmp_ne(p, V(0.f)), mask,
    wide::cmp_le(p, V(0.f)), wide::cmp_gt(p, V(0.f)), wide::cmp_ge(p, V(0.f))};
  auto both = wide::bit_and(mask, inverted), either = wide::bit_or(mask, inverted);
  static_assert(std::same_as<decltype(mask), std::array<M, 2>>);
  auto selected = wide::select(mask, V(8.f), p);
  auto sum = wide::add(V(2.f), p), difference = wide::sub(p, V(2.f)), divided = wide::div(p, V(2.f));
  auto product = wide::mul(p, V(2.f)), negated = wide::negate(p);
  if constexpr(requires { wide::scaleb(p,V(2.f)); }) {
    auto scaled=wide::scaleb(p,V(2.f));
    auto merged=wide::masked_scaleb(mask,V(77.f),p,V(2.f));
    auto cleared=wide::masked_scaleb_zero(mask,p,V(2.f));
    for(std::size_t element=0;element<2;++element) {
      std::array<float,V::lanes> scale{},merge{},clear{};
      scaled[element].storeu(scale.data());merged[element].storeu(merge.data());
      cleared[element].storeu(clear.data());
      for(std::size_t lane=0;lane<V::lanes;++lane) {
        float value=element==0?input[lane]:-input[lane];
        exact(scale[lane],value*4.f,"scaleb broadcast");
        exact(merge[lane],value<0.f?value*4.f:77.f,"masked_scaleb inactive prior");
        exact(clear[lane],value<0.f?value*4.f:0.f,"masked_scaleb_zero inactive zero");
      }
    }
  }
  static_assert(std::same_as<decltype(selected), P>);
  for (std::size_t element = 0; element < 2; ++element) {
    std::array<float, V::lanes> lo{}, hi{}, choice{}, plus{}, minus{}, quotient{};
    std::array<float, V::lanes> times{}, negative{};
    low[element].storeu(lo.data()); high[element].storeu(hi.data());
    selected[element].storeu(choice.data()); sum[element].storeu(plus.data());
    difference[element].storeu(minus.data()); divided[element].storeu(quotient.data());
    product[element].storeu(times.data()); negated[element].storeu(negative.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      float value = element == 0 ? input[lane] : -input[lane];
      std::array expected_comparisons{value == 0.f, value != 0.f, value < 0.f,
        value <= 0.f, value > 0.f, value >= 0.f};
      for (std::size_t comparison = 0; comparison < expected_comparisons.size(); ++comparison)
        require(((comparisons[comparison][element].to_bitset() >> lane) & 1u) == expected_comparisons[comparison],
          "named comparison reduced or reordered array masks");
      require(((inverted[element].to_bitset() >> lane) & 1u) == !(value < 0.f),
        "named mask complement changed a lane");
      require(((both[element].to_bitset() >> lane) & 1u) == 0 &&
        ((either[element].to_bitset() >> lane) & 1u) == 1, "named mask bit operations changed a lane");
      exact(lo[lane], 0.f < value ? 0.f : value, "min broadcast");
      exact(hi[lane], value > 0.f ? value : 0.f, "max broadcast");
      exact(choice[lane], value < 0.f ? 8.f : value, "select broadcast");
      exact(plus[lane], 2.f + value, "addition broadcast");
      exact(minus[lane], value - 2.f, "subtraction broadcast");
      exact(quotient[lane], value / 2.f, "division broadcast");
      exact(times[lane], value * 2.f, "multiplication broadcast");
      exact(negative[lane], -value, "negation pointwise");
    }
  }
}

template<class V> static void bit_helpers_and_aliases() {
  using U = typename V::bits_type;
  using P = std::array<V, 2>;
  constexpr std::array<std::uint32_t, 6> bank{
    0x80000000u, 0x7f800001u, 0xffc12345u, 0x01000001u, 0x00000001u, 0x7f800000u};
  std::array<std::uint32_t, V::lanes> words{};
  for (std::size_t lane = 0; lane < V::lanes; ++lane) words[lane] = bank[lane % bank.size()];
  auto before = native::test::read_fp_state();
  V v = V::from_bits(U::load(words.data()));
  auto encoded = wide::bits(P{{v, v}});
  static_assert(std::same_as<decltype(encoded), std::array<U, 2>>);
  auto integer_constant = wide::constant_like(encoded, std::uint32_t(0xfffffffeu));
  static_assert(std::same_as<decltype(integer_constant), U>);
  auto flipped = wide::bit_xor(wide::bit_and(encoded, integer_constant), U(1u));
  auto sign_set = wide::bit_or(encoded, U(0x80000000u));
  auto complemented = wide::bit_not(encoded);
  auto shifted = wide::left<1>(flipped);
  auto roundtrip = wide::from_bits(encoded);
  auto equality = wide::cmp_eq(encoded, encoded);
  auto mask_words = wide::mask_bits<std::uint32_t>(equality);
  for (std::size_t element = 0; element < 2; ++element) {
    std::array<std::uint32_t, V::lanes> constants{}, altered{}, shift{}, restored{}, masks{};
    std::array<std::uint32_t, V::lanes> signed_words{}, complements{};
    integer_constant.storeu(constants.data()); flipped[element].storeu(altered.data());
    shifted[element].storeu(shift.data()); roundtrip[element].store_bits(restored.data());
    mask_words[element].storeu(masks.data());
    sign_set[element].storeu(signed_words.data()); complemented[element].storeu(complements.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      auto expected = (words[lane] & 0xfffffffeu) ^ 1u;
      require(constants[lane] == 0xfffffffeu, "uint32 constant was narrowed through float");
      require(altered[lane] == expected && shift[lane] == (expected << 1), "wide integer bit helper changed bits");
      require(restored[lane] == words[lane], "wide bit roundtrip changed a payload or sign");
      require(signed_words[lane] == (words[lane] | 0x80000000u) && complements[lane] == ~words[lane],
        "named bitwise operations changed an integer word");
      require(masks[lane] == 0xffffffffu, "wide mask_bits failed full true lanes");
    }
  }
  require(native::test::read_fp_state() == before, "wide bit helpers changed FP state");

  std::array<V, 1> zeros{{V(-0.f)}};
  auto aliased_sine = wide::sin(zeros);
  auto aliased_cosine = wide::cos(zeros);
  auto aliased_pair = wide::sincos(zeros);
  auto aliased_flush = wide::flush_to_zero(zeros);
  std::array<float, V::lanes> negative_zero{}, one{};
  negative_zero.fill(-0.f); one.fill(1.f);
  exact_vector(aliased_sine[0], negative_zero, "wide::sin alias");
  exact_vector(aliased_cosine[0], one, "wide::cos alias");
  exact_vector(aliased_pair.first[0], negative_zero, "wide::sincos sine alias");
  exact_vector(aliased_pair.second[0], one, "wide::sincos cosine alias");
  exact_vector(aliased_flush[0], negative_zero, "wide::flush_to_zero alias");
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
  auto saved = native::test::read_fp_state();
  for (auto mode : {native::test::fp_mode::gradual, native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);
    empty_shapes(sine); empty_shapes(cosine); empty_shapes(paired_sine); empty_shapes(paired_cosine); empty_shapes(flush);
    check_width<scalar>(trig, words);
#if defined(__AVX2__)
    check_width<native::simd<float, 2, native::avx2>>(trig, words);
    check_width<native::simd<float, 3, native::avx2>>(trig, words);
    check_width<native::simd<float, 4, native::avx2>>(trig, words);
    check_width<native::simd<float, 8, native::avx2>>(trig, words);
#elif defined(__ARM_NEON)
    check_width<native::simd<float, 4, native::neon>>(trig, words);
#endif
    require(scope.controls_match(), "promoted math changed FP controls");
  }
  require(native::test::read_fp_state() == saved, "promoted math fixture failed to restore FP state");
  std::printf("promoted trig/unary shapes passed; %zu trig inputs, %zu flush words per width/mode\n", trig.size(), words.size());
}
