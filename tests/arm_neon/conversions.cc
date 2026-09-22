// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <array>
#include <bit>
#include <cfenv>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <type_traits>
#if NATIVE_CONVERSION_HEADER
#include <native/simd/math/exp.h>
#else
import native.arm.neon;
import native.math;
#endif

#if NATIVE_HOST_NEON
namespace fixture {
  constexpr auto neon = native::neon;
  constexpr auto stronger = native::feature_closure(neon & native::arm_feature::rdm);
  template<std::size_t N, native::isa<> A = neon>
  using floats = native::simd<float, N, A>;
  template<std::size_t N, native::isa<> A = neon>
  using integers = native::simd<std::int32_t, N, A>;

  template<class T> concept converts = requires(T x) { native::fcvtzs(x); };
  static_assert(std::same_as<decltype(native::fcvtzs(1.f)), std::int32_t>);
  static_assert(std::same_as<decltype(native::fcvtzs<neon>(1.f)), std::int32_t>);
  static_assert(std::same_as<decltype(native::fcvtzs(floats<1, native::scalar>{})),
    integers<1, native::scalar>>);
  static_assert(std::same_as<decltype(native::fcvtzs(floats<1>{})), integers<1>>);
  static_assert(std::same_as<decltype(native::fcvtzs(floats<2>{})), integers<2>>);
  static_assert(std::same_as<decltype(native::fcvtzs(floats<3>{})), integers<3>>);
  static_assert(std::same_as<decltype(native::fcvtzs(floats<4>{})), integers<4>>);
  static_assert(std::same_as<decltype(native::fcvtzs(floats<4, stronger>{})),
    integers<4, stronger>>);
  static_assert(!converts<native::simd<std::int32_t, 4, neon>>);
  static_assert(!converts<native::simd<std::uint32_t, 4, neon>>);
  static_assert(!converts<native::simd<double, 2, neon>>);
  static_assert(!converts<native::simd<float, 8, neon>>);
  static_assert(noexcept(native::fcvtzs(1.f)));
  static_assert(noexcept(native::fcvtzs(floats<4>{})));

  template<class T> concept converts_unsigned = requires(T x) { native::fcvtzu(x); };
  template<class V> constexpr bool unsigned_result = std::same_as<
    decltype(native::fcvtzu(V{})), typename V::template rebind<std::uint32_t>>;
  static_assert(std::same_as<decltype(native::fcvtzu(1.f)), std::uint32_t>);
  static_assert(std::same_as<decltype(native::fcvtzu<neon>(1.f)), std::uint32_t>);
  static_assert(unsigned_result<floats<1, native::scalar>>);
  static_assert(unsigned_result<floats<1>> && unsigned_result<floats<2>>);
  static_assert(unsigned_result<floats<3>> && unsigned_result<floats<4>>);
  static_assert(unsigned_result<floats<4, stronger>>);
  static_assert(!converts_unsigned<native::simd<std::int32_t, 4, neon>>);
  static_assert(!converts_unsigned<native::simd<std::uint32_t, 4, neon>>);
  static_assert(!converts_unsigned<native::simd<double, 2, neon>>);
  static_assert(!converts_unsigned<native::simd<float, 8, neon>>);
  static_assert(noexcept(native::fcvtzu(1.f)));
  static_assert(noexcept(native::fcvtzu(floats<4>{})));

  constexpr bool is_nan(std::uint32_t word) noexcept {
    return (word & 0x7fffffffu) > 0x7f800000u;
  }

  // Integer-only oracle: decode binary32, truncate its significand, then saturate.
  // It never casts an exceptional or out-of-range floating value to an integer.
  constexpr std::int32_t expected(std::uint32_t word) noexcept {
    auto magnitude = word & 0x7fffffffu;
    auto exponent = (word >> 23) & 255u;
    bool negative = (word >> 31) != 0;
    if (magnitude > 0x7f800000u || exponent < 127u) return 0;
    if (exponent >= 158u)
      return negative ? std::numeric_limits<std::int32_t>::min()
                      : std::numeric_limits<std::int32_t>::max();
    auto significand = (word & 0x007fffffu) | 0x00800000u;
    auto integer = exponent >= 150u ? significand << (exponent - 150u)
                                   : significand >> (150u - exponent);
    return std::bit_cast<std::int32_t>(negative ? 0u - integer : integer);
  }
  static_assert(expected(0x4effffffu) == 2147483520);
  static_assert(expected(0x4f000000u) == 2147483647);
  static_assert(expected(0xcf000000u) == (-2147483647 - 1));
  static_assert(expected(0xceffffffu) == -2147483520);
  static_assert(expected(0x3fffffffu) == 1 && expected(0xbfffffffu) == -1);
  static_assert(expected(0x7f800001u) == 0 && expected(0xff800001u) == 0);

  constexpr std::uint32_t expected_unsigned(std::uint32_t word) noexcept {
    auto magnitude = word & 0x7fffffffu;
    auto exponent = (word >> 23) & 255u;
    if ((word >> 31) || magnitude > 0x7f800000u || exponent < 127u) return 0;
    if (exponent >= 159u) return std::numeric_limits<std::uint32_t>::max();
    auto significand = (word & 0x007fffffu) | 0x00800000u;
    return exponent >= 150u ? significand << (exponent - 150u)
                            : significand >> (150u - exponent);
  }
  static_assert(expected_unsigned(0x4f000000u) == 2147483648u);
  static_assert(expected_unsigned(0x4f7fffffu) == 4294967040u);
  static_assert(expected_unsigned(0x4f800000u) == 4294967295u);
  static_assert(expected_unsigned(0x3fffffffu) == 1u);
  static_assert(expected_unsigned(0xbfffffffu) == 0u);
  static_assert(expected_unsigned(0x7f800000u) == 4294967295u);
  static_assert(expected_unsigned(0xff800000u) == 0u);
  static_assert(expected_unsigned(0x7f800001u) == 0u);

  constexpr std::array edge_words{
    0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u,
    0x007fffffu, 0x807fffffu, 0x00800000u, 0x80800000u,
    0x3effffffu, 0xbeffffffu, 0x3f000000u, 0xbf000000u,
    0x3f7fffffu, 0xbf7fffffu, 0x3f800000u, 0xbf800000u,
    0x3f800001u, 0xbf800001u, 0x3fc00000u, 0xbfc00000u,
    0x3fffffffu, 0xbfffffffu, 0x40000000u, 0xc0000000u,
    0x437e0000u, 0xc37e0000u, 0x4b7fffffu, 0xcb7fffffu,
    0x4b800000u, 0xcb800000u, 0x4efffffeu, 0xcefffffeu,
    0x4effffffu, 0xceffffffu, 0x4f000000u, 0xcf000000u,
    0x4f000001u, 0xcf000001u, 0x4f7fffffu, 0xcf7fffffu,
    0x4f7ffffeu, 0xcf7ffffeu, 0x4f800001u, 0xcf800001u,
    0x4f800000u, 0xcf800000u, 0x7f7fffffu, 0xff7fffffu,
    0x7f800000u, 0xff800000u, 0x7f800001u, 0xff800001u,
    0x7fbfffffu, 0xffbfffffu, 0x7fc00000u, 0xffc00000u,
    0x7fc12345u, 0xffc12345u, 0x7fffffffu, 0xffffffffu
  };
  constexpr std::array nan_words{
    0x7f800001u, 0xff800001u, 0x7fbfffffu, 0xffbfffffu,
    0x7fc00000u, 0xffc00000u, 0x7fc12345u, 0xffc12345u,
    0x7fffffffu, 0xffffffffu
  };

  template<class V>
  constexpr bool check_conversion(std::array<std::uint32_t, V::lanes> const & words) {
    std::array<float, V::lanes> input{};
    std::array<std::int32_t, V::lanes> output{};
    std::array<std::uint32_t, V::lanes> unsigned_output{};
    for (std::size_t i = 0; i < V::lanes; ++i) input[i] = std::bit_cast<float>(words[i]);
    native::fcvtzs(V::load(input.data())).store(output.data());
    native::fcvtzu(V::load(input.data())).store(unsigned_output.data());
    for (std::size_t i = 0; i < V::lanes; ++i)
      if (output[i] != expected(words[i]) ||
          unsigned_output[i] != expected_unsigned(words[i])) return false;
    return true;
  }

  template<class V> consteval bool constant_edges() {
    for (std::size_t offset = 0; offset < edge_words.size(); ++offset) {
      std::array<std::uint32_t, V::lanes> words{};
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        words[lane] = edge_words[(offset + lane) % edge_words.size()];
      if (!check_conversion<V>(words)) return false;
    }
    return true;
  }
  static_assert(constant_edges<floats<1, native::scalar>>());
  static_assert(constant_edges<floats<1>>());
  static_assert(constant_edges<floats<2>>());
  static_assert(constant_edges<floats<3>>());
  static_assert(constant_edges<floats<4>>());
  static_assert(constant_edges<floats<4, stronger>>());
  static_assert([] consteval {
    for (auto word : edge_words) {
      auto value = std::bit_cast<float>(word);
      if (native::fcvtzs(value) != expected(word) ||
          native::fcvtzs<neon>(value) != expected(word) ||
          native::fcvtzu(value) != expected_unsigned(word) ||
          native::fcvtzu<neon>(value) != expected_unsigned(word)) return false;
    }
    return true;
  }());

  // Keep runtime inputs opaque to the production leaf, including NaN-only banks.
  template<bool Flush, class T> [[gnu::noinline]] auto runtime_exp(T input) {
    return math::exp<Flush>(input);
  }
  template<bool Flush, class T> constexpr auto checked_exp(T input) {
    if consteval { return math::exp<Flush>(input); }
    else { return runtime_exp<Flush>(input); }
  }

  template<bool Flush, class V>
  constexpr bool check_nan_exp(std::uint32_t nan, std::size_t position) {
    std::array<float, V::lanes> input{};
    std::array<float, V::lanes> output{};
    for (std::size_t lane = 0; lane < V::lanes; ++lane)
      input[lane] = std::bit_cast<float>(lane == position ? nan :
        (lane % 2 ? 0xff800000u : 0u));
    checked_exp<Flush>(V::load(input.data())).store(output.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      auto word = std::bit_cast<std::uint32_t>(output[lane]);
      if (lane == position) {
        if (!is_nan(word)) return false;
      } else if (word != (lane % 2 ? 0u : 0x3f800000u)) return false;
    }
    return true;
  }
  template<class V, std::uint32_t Nan> inline constexpr bool constant_one_nan_exp = [] {
    for (std::size_t lane = 0; lane < V::lanes; ++lane)
      if (!check_nan_exp<false, V>(Nan, lane) || !check_nan_exp<true, V>(Nan, lane))
        return false;
    return true;
  }();
  template<class V> consteval bool constant_nan_exp() {
    // Each template variable gets an independent constexpr evaluation budget.
    return constant_one_nan_exp<V, nan_words[0]> && constant_one_nan_exp<V, nan_words[1]> &&
      constant_one_nan_exp<V, nan_words[2]> && constant_one_nan_exp<V, nan_words[3]> &&
      constant_one_nan_exp<V, nan_words[4]> && constant_one_nan_exp<V, nan_words[5]> &&
      constant_one_nan_exp<V, nan_words[6]> && constant_one_nan_exp<V, nan_words[7]> &&
      constant_one_nan_exp<V, nan_words[8]> && constant_one_nan_exp<V, nan_words[9]>;
  }
  static_assert(constant_nan_exp<floats<1, native::scalar>>());
  static_assert(constant_nan_exp<floats<1>>());
  static_assert(constant_nan_exp<floats<2>>());
  static_assert(constant_nan_exp<floats<3>>());
  static_assert(constant_nan_exp<floats<4>>());
  static_assert([] consteval {
    for (auto word : nan_words) {
      auto value = std::bit_cast<float>(word);
      if (!is_nan(std::bit_cast<std::uint32_t>(math::exp(value))) ||
          !is_nan(std::bit_cast<std::uint32_t>(math::exp<true>(value)))) return false;
    }
    return true;
  }());

  constexpr std::array overflow_inputs{
    177.79f, 93032648.f, std::numeric_limits<float>::max(),
    std::numeric_limits<float>::infinity()
  };
  template<bool Flush, class V> constexpr bool check_overflow_exp(float value) {
    for (std::size_t sign = 0; sign < 2; ++sign) {
      std::array<float, V::lanes> input{}, output{};
      auto magnitude = std::bit_cast<std::uint32_t>(value);
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        input[lane] = std::bit_cast<float>(magnitude | (((lane + sign) % 2) ? 0x80000000u : 0u));
      checked_exp<Flush>(V::load(input.data())).store(output.data());
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        if (std::bit_cast<std::uint32_t>(output[lane]) !=
            (((lane + sign) % 2) ? 0u : 0x7f800000u)) return false;
    }
    return true;
  }
  template<class V, std::size_t I> inline constexpr bool constant_overflow_exp =
    check_overflow_exp<false, V>(overflow_inputs[I]) &&
    check_overflow_exp<true, V>(overflow_inputs[I]);
  template<class V> consteval bool constant_overflow_edges() {
    return constant_overflow_exp<V, 0> && constant_overflow_exp<V, 1> &&
      constant_overflow_exp<V, 2> && constant_overflow_exp<V, 3>;
  }
  static_assert(constant_overflow_edges<floats<1, native::scalar>>());
  static_assert(constant_overflow_edges<floats<1>>());
  static_assert(constant_overflow_edges<floats<2>>());
  static_assert(constant_overflow_edges<floats<3>>());
  static_assert(constant_overflow_edges<floats<4>>());

  constexpr auto cutoff_word = std::bit_cast<std::uint32_t>(88.3762664794921875f);
  template<bool Flush, class V> constexpr bool check_threshold_exp() {
    for (std::size_t offset = 0; offset < 3; ++offset) {
      std::array<float, V::lanes> input{}, output{};
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        input[lane] = std::bit_cast<float>(cutoff_word - 1u + std::uint32_t((lane + offset) % 3));
      checked_exp<Flush>(V::load(input.data())).store(output.data());
      for (std::size_t lane = 0; lane < V::lanes; ++lane) {
        auto word = std::bit_cast<std::uint32_t>(output[lane]);
        if ((lane + offset) % 3 == 0) {
          if (word == 0 || word >= 0x7f800000u) return false;
        } else if (word != 0x7f800000u) return false;
      }
    }
    return true;
  }
  template<class V, bool Flush> inline constexpr bool constant_threshold_exp =
    check_threshold_exp<Flush, V>();
  static_assert(constant_threshold_exp<floats<1, native::scalar>, false>);
  static_assert(constant_threshold_exp<floats<1, native::scalar>, true>);
  static_assert(constant_threshold_exp<floats<1>, false> && constant_threshold_exp<floats<1>, true>);
  static_assert(constant_threshold_exp<floats<2>, false> && constant_threshold_exp<floats<2>, true>);
  static_assert(constant_threshold_exp<floats<3>, false> && constant_threshold_exp<floats<3>, true>);
  static_assert(constant_threshold_exp<floats<4>, false> && constant_threshold_exp<floats<4>, true>);

  [[noreturn]] void fail(char const * what, std::size_t lanes, std::uint32_t word) {
    std::fprintf(stderr, "%s: lanes=%zu input=%08x\n", what, lanes, word);
    std::abort();
  }
  std::uint32_t random_word(std::uint32_t & state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
  }
  template<class V> void runtime_shape() {
    if (std::fegetround() == FE_TONEAREST &&
        (!check_threshold_exp<false, V>() || !check_threshold_exp<true, V>()))
      fail("exp early-infinity threshold mismatch", V::lanes, cutoff_word);
    for (auto value : overflow_inputs)
      if (!check_overflow_exp<false, V>(value) || !check_overflow_exp<true, V>(value))
        fail("exp overflow/underflow classification mismatch", V::lanes,
          std::bit_cast<std::uint32_t>(value));
    std::array<std::uint32_t, V::lanes> words{};
    for (std::size_t offset = 0; offset < edge_words.size(); ++offset) {
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        words[lane] = edge_words[(offset + lane) % edge_words.size()];
      if (!check_conversion<V>(words)) fail("FCVTZS/FCVTZU edge mismatch", V::lanes, words[0]);
    }
    std::uint32_t state = 0x16c247a9u;
    for (std::size_t i = 0; i < 100000; ++i) {
      for (auto & word : words) word = random_word(state);
      if (!check_conversion<V>(words)) fail("FCVTZS/FCVTZU random mismatch", V::lanes, words[0]);
    }
    for (auto nan : nan_words)
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        if (!check_nan_exp<false, V>(nan, lane) || !check_nan_exp<true, V>(nan, lane))
          fail("exp lost a NaN or changed an adjacent lane", V::lanes, nan);
  }
  void runtime_scalar() {
    if (std::fegetround() == FE_TONEAREST) {
      for (auto word : {cutoff_word - 1u, cutoff_word, cutoff_word + 1u}) {
        auto value = std::bit_cast<float>(word);
        auto ordinary = std::bit_cast<std::uint32_t>(runtime_exp<false>(value));
        auto flush = std::bit_cast<std::uint32_t>(runtime_exp<true>(value));
        if (word < cutoff_word ?
            (ordinary == 0 || ordinary >= 0x7f800000u || flush == 0 || flush >= 0x7f800000u) :
            (ordinary != 0x7f800000u || flush != 0x7f800000u))
          fail("scalar exp early-infinity threshold mismatch", 1, word);
      }
    }
    for (auto value : overflow_inputs) {
      if (std::bit_cast<std::uint32_t>(runtime_exp<false>(value)) != 0x7f800000u ||
          std::bit_cast<std::uint32_t>(runtime_exp<true>(value)) != 0x7f800000u ||
          std::bit_cast<std::uint32_t>(runtime_exp<false>(-value)) != 0u ||
          std::bit_cast<std::uint32_t>(runtime_exp<true>(-value)) != 0u)
        fail("scalar exp overflow/underflow classification mismatch", 1,
          std::bit_cast<std::uint32_t>(value));
    }
    for (auto word : edge_words) {
      auto value = std::bit_cast<float>(word);
      if (native::fcvtzs(value) != expected(word) ||
          native::fcvtzs<neon>(value) != expected(word) ||
          native::fcvtzu(value) != expected_unsigned(word) ||
          native::fcvtzu<neon>(value) != expected_unsigned(word))
        fail("scalar FCVTZS/FCVTZU edge mismatch", 1, word);
    }
    std::uint32_t state = 0x239e91fbu;
    for (std::size_t i = 0; i < 100000; ++i) {
      auto word = random_word(state);
      if (native::fcvtzs(std::bit_cast<float>(word)) != expected(word) ||
          native::fcvtzu(std::bit_cast<float>(word)) != expected_unsigned(word))
        fail("scalar FCVTZS/FCVTZU random mismatch", 1, word);
    }
    for (auto nan : nan_words) {
      auto value = std::bit_cast<float>(nan);
      if (!is_nan(std::bit_cast<std::uint32_t>(runtime_exp<false>(value))) ||
          !is_nan(std::bit_cast<std::uint32_t>(runtime_exp<true>(value))))
        fail("scalar exp lost a NaN", 1, nan);
    }
  }

  // Optional exhaustive runtime check: every signed binary32 NaN encoding.
  // This is kept behind an argument so normal CTest runs stay bounded.
  template<bool Flush> void all_nan_exp() {
    using V = floats<4>;
    std::array<float, 4> input{}, output{};
    for (auto sign : {0u, 0x80000000u}) {
      for (std::uint32_t first = 1; first <= 0x007fffffu; first += 4) {
        for (std::size_t lane = 0; lane < 4; ++lane) {
          auto payload = first + std::uint32_t(lane);
          if (payload > 0x007fffffu) payload = 1;
          input[lane] = std::bit_cast<float>(sign | 0x7f800000u | payload);
        }
        checked_exp<Flush>(V::load(input.data())).store(output.data());
        for (std::size_t lane = 0; lane < 4; ++lane)
          if (!is_nan(std::bit_cast<std::uint32_t>(output[lane])))
            fail("exhaustive exp NaN mismatch", 4, std::bit_cast<std::uint32_t>(input[lane]));
      }
    }
  }
}

int main(int argc, char ** argv) {
  bool exhaustive = argc == 2 && std::strcmp(argv[1], "--all-nans") == 0;
  if (argc > 1 && !exhaustive) return 2;
  auto original_rounding = std::fegetround();
  for (auto mode : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
    if (std::fesetround(mode) != 0) return 3;
    fixture::runtime_scalar();
    fixture::runtime_shape<fixture::floats<1, native::scalar>>();
    fixture::runtime_shape<fixture::floats<1>>();
    fixture::runtime_shape<fixture::floats<2>>();
    fixture::runtime_shape<fixture::floats<3>>();
    fixture::runtime_shape<fixture::floats<4>>();
    fixture::runtime_shape<fixture::floats<4, fixture::stronger>>();
  }
  if (std::fesetround(original_rounding) != 0) return 4;
  if (exhaustive) {
    fixture::all_nan_exp<false>();
    fixture::all_nan_exp<true>();
  }
  std::puts("ARM FCVTZS/FCVTZU: scalar/1/2/3/4 lanes, signed/unsigned saturation, constexpr, four rounding modes, random inputs, exp NaN, overflow and early-infinity threshold passed");
}
#else
int main() { return 77; }
#endif
