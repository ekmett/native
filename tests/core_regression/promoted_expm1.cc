// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <vector>
#include "support/fp_environment.h"

#include "support/profile.h"
#if NATIVE_TEST_IMPORT
import native.wide;
import native.math;
#else
#include <native/wide.h>
#include <native/simd/math/exp.h>
#endif

constexpr std::size_t full_lanes = NATIVE_TEST_PROFILE == 0 ? 1 : NATIVE_TEST_PROFILE / 32;
template<std::size_t N> using vec = native::simd<float, N, test_arch>;
using scalar = native::simd<float, 1, native::scalar>;
constexpr bool nan(std::uint32_t word) { return (word & 0x7fffffffu) > 0x7f800000u; }
constexpr std::uint32_t bits(float value) { return std::bit_cast<std::uint32_t>(value); }
constexpr float value(std::uint32_t word) { return std::bit_cast<float>(word); }
static_assert(bits(math::expm1(0.f)) == 0u);
static_assert(bits(math::expm1(-0.f)) == 0x80000000u);
static_assert(bits(math::expm1(value(1u))) == 1u);
static_assert(bits(math::expm1(value(0x80000001u))) == 0x80000001u);
static_assert(bits(math::expm1(value(0xff800000u))) == 0xbf800000u);
static_assert(bits(math::expm1(value(0x7f800000u))) == 0x7f800000u);
static_assert(nan(bits(math::expm1(value(0x7f800001u)))));
static_assert(bits(math::damping_gain(-0.f)) == 0x80000000u);
static_assert(bits(math::damping_gain(value(0x7f800000u))) == 0x3f800000u);
static_assert(std::same_as<decltype(math::expm1(1.f)), float>);
static_assert(std::same_as<decltype(math::expm1(std::array<float, 3>{})), std::array<float, 3>>);
static_assert(std::same_as<decltype(math::expm1(vec<full_lanes>{})), vec<full_lanes>>);
static_assert(std::same_as<decltype(math::expm1(std::array<vec<full_lanes>, 3>{})), std::array<vec<full_lanes>, 3>>);
static_assert(std::same_as<decltype(math::expm1(native::wide<vec<full_lanes>, 3>{})), native::wide<vec<full_lanes>, 3>>);
static_assert(math::expm1(std::array<float, 0>{}).empty());
static_assert(math::damping_gain(std::array<float, 0>{}).empty());

template<class V> consteval bool constant_packet() {
  constexpr std::array words{0u, 0x80000000u, 1u, 0x80000001u,
    0x33000000u, 0xb3000000u, 0x3f800000u, 0xbf800000u, 0x3f317218u,
    0x7f800000u, 0xff800000u, 0x7f800001u, 0xffc12345u};
  for (auto word : words) {
    auto result = math::expm1(V(value(word)));
    std::array<float, V::lanes> actual{};
    result.store(actual.data());
    auto const expected = bits(math::expm1(value(word)));
    for (auto x : actual)
      if (!(bits(x) == expected || (nan(bits(x)) && nan(expected)))) return false;
  }
  return true;
}
static_assert(constant_packet<scalar>());
#if NATIVE_TEST_PROFILE != 0
static_assert(constant_packet<vec<2>>());
static_assert(constant_packet<vec<3>>());
#endif
static_assert(constant_packet<vec<full_lanes>>());

std::uint32_t ordered(std::uint32_t word) { return word >> 31 ? ~word : word ^ 0x80000000u; }
std::uint32_t max_ulp = 0;
std::uint32_t worst_input = 0;
std::size_t checks = 0;
void check(float input, float actual, std::uint32_t e, bool gain) {
  auto const a = bits(actual);
  if (nan(e)) {
    if (!nan(a)) std::abort();
  } else if ((e & 0x7fffffffu) == 0u || (e & 0x7fffffffu) == 0x7f800000u) {
    if (a != e) {
      std::fprintf(stderr, "special gain=%d input=%08x got=%08x expected=%08x\n", gain,bits(input),a,e);
      std::abort();
    }
  } else {
    auto const oa = ordered(a), oe = ordered(e);
    auto const error = oa > oe ? oa - oe : oe - oa;
    if (error > max_ulp) { max_ulp = error; worst_input = bits(input); }
    if (error > 2) {
      std::fprintf(stderr, "accuracy gain=%d input=%08x got=%08x expected=%08x ulp=%u\n",gain,bits(input),a,e,error);
      std::abort();
    }
  }
  ++checks;
}
template<class V> [[gnu::noinline]] auto evaluate(std::array<V, 3> const & input) {
  return std::pair{math::expm1(input), math::damping_gain(input)};
}
template<class V> void shape(std::vector<std::uint32_t> const & words,
    std::vector<std::array<std::uint32_t, 2>> const & expected) {
  for (std::size_t offset = 0; offset < words.size(); offset += V::lanes * 3) {
    std::array<std::array<float, V::lanes>, 3> inputs{}, exps{}, gains{};
    std::array<V, 3> registers;
    for (std::size_t chain = 0; chain < 3; ++chain) {
      for (std::size_t lane = 0; lane < V::lanes; ++lane)
        inputs[chain][lane] = value(words[(offset + chain * V::lanes + lane) % words.size()]);
      registers[chain] = V::load(inputs[chain].data());
    }
    auto [exp, gain] = evaluate(registers);
    auto legacy = math::expm1(native::wide<V, 3>{registers});
    for (std::size_t chain = 0; chain < 3; ++chain) {
      exp[chain].store(exps[chain].data());
      gain[chain].store(gains[chain].data());
      std::array<float, V::lanes> legacy_result{};
      legacy.registers[chain].store(legacy_result.data());
      for (std::size_t lane = 0; lane < V::lanes; ++lane) {
        auto const i = (offset + chain * V::lanes + lane) % words.size();
        check(inputs[chain][lane], exps[chain][lane], expected[i][0], false);
        check(inputs[chain][lane], gains[chain][lane], expected[i][1], true);
        auto a = bits(legacy_result[lane]), b = bits(exps[chain][lane]);
        if (!(a == b || (nan(a) && nan(b)))) std::abort();
      }
    }
  }
}
int main() {
  std::vector<std::uint32_t> words{0u,0x80000000u,1u,0x80000001u,0x7f800000u,
    0xff800000u,0x7f800001u,0xff800001u,0x7fc12345u,0xffc12345u};
  for (auto boundary : {0x33000000u,0x3eb17218u,0x3f800000u,0x418aa123u,0x42b0c0a6u})
    for (int offset = -64; offset <= 64; ++offset)
      for (auto sign : {0u,0x80000000u}) words.push_back((boundary + std::uint32_t(offset)) | sign);
  // Exercise both sides of reconstruction and round-even reduction boundaries.
  for (int n = -26; n <= 127; ++n)
    for (double residual : {-0.5, 0., 0.5}) {
      auto const center = bits(float((double(n) + residual) * std::log(2.)));
      for (int offset = -32; offset <= 32; ++offset)
        words.push_back(center + std::uint32_t(offset));
    }
  for (auto boundary : {0x007fffffu, 0x00800000u, 0x33800000u})
    for (int offset = -64; offset <= 64; ++offset)
      for (auto sign : {0u, 0x80000000u})
        words.push_back((boundary + std::uint32_t(offset)) | sign);
  std::uint32_t state = 0x138fc297u;
  for (unsigned i = 0; i < 100000; ++i) {
    state ^= state << 13;state ^= state >> 17;state ^= state << 5;
    words.push_back(state);
    words.push_back(bits(float(int(state & 0xffffu) - 32768) * 0x1p-9f));
  }
  auto const saved = native::test::read_fp_state();
  std::vector<std::array<std::uint32_t, 2>> expected;
  expected.reserve(words.size());
  {
    // Reference values are computed once with gradual underflow, then checked
    // by integer bits even when the caller enables FTZ/DAZ.
    native::test::fp_scope scope(native::test::fp_mode::gradual);
    for (auto word : words) {
      std::array<std::uint32_t, 2> results{};
      for (unsigned gain = 0; gain < 2; ++gain) {
        auto const x = gain ? -double(value(word)) : double(value(word));
        auto reference = x > double(value(0x42b0c0a5u)) ? value(0x7f800000u) : float(std::expm1(x));
        if (gain) reference = -reference;
        results[gain] = bits(reference);
      }
      expected.push_back(results);
    }
  }
  for (auto mode : {native::test::fp_mode::gradual, native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);
    shape<scalar>(words, expected);
#if NATIVE_TEST_PROFILE != 0
    shape<vec<2>>(words, expected);
    shape<vec<3>>(words, expected);
    shape<vec<full_lanes>>(words, expected);
#endif
    if (!scope.controls_match()) std::abort();
  }
  if (native::test::read_fp_state() != saved) std::abort();
  std::printf("checks=%zu words=%zu max_ulp=%u worst=%08x full_lanes=%zu; shape/constexpr/gradual+flush/controls passed\n", checks,words.size(),max_ulp,worst_input,full_lanes);
}
