#include "support/fp_environment.h"
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <type_traits>
#include <utility>
#include "support/profile.h"
import simd.wide;

static_assert(std::same_as<decltype(simd::vec{test_arch{},1.0f}), test_vec<float, 1>>);
static_assert(sizeof(test_vec<float,1>) == sizeof(float));
static_assert(std::is_trivially_copyable_v<test_vec<float,1>>);

void emit(std::ostream & out, std::uint32_t bits) {
  // NaN payload and sign are explicitly outside the reproducibility contract.
  if ((bits & 0x7fffffffu) > 0x7f800000u) bits = 0x7fc00000u;
  out.write(reinterpret_cast<char const *>(&bits), sizeof(bits));
}

template<std::size_t N> void capture(std::ostream & out) {
  using V = test_vec<float, N>;
  using F = test_vec<float, N>;
  using I = test_vec<simd::uint32_t, N>;
  std::uint32_t seed = 0x893bfd12u;
  for (std::size_t step = 0; step < 512; ++step) {
    std::array<float, N> input{};
    for (auto & x : input) {
      seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
      x = float(int(seed & 0xffffu) - 32768) * 0x1p-10f;
    }
    auto a = V::loadu(input.data());
    simd::wide<V, 3> values{a, a * V(.5f), a * V(.25f)};
    auto [...lanes] = values;
    auto reconstructed = simd::wide{lanes...};
    auto result = simd::exp(reconstructed);
    auto sum = reconstructed + reconstructed;
    auto product = reconstructed * reconstructed;
    auto fused = simd::fma(reconstructed, reconstructed, reconstructed);
    for (std::size_t i = 0; i < 3; ++i) {
      for (auto v : {result.registers[i], sum.registers[i], product.registers[i], fused.registers[i]}) {
        std::array<std::uint32_t, N> words{};
        v.store_bits(words.data());
        for (auto word : words) emit(out, word);
      }
    }
    auto raw = simd::exp(simd::wide<F, 2>{F::loadu(input.data())});
    for (auto v : raw.registers) {
      std::array<std::uint32_t, N> words{};
      v.store_bits(words.data());
      for (auto word : words) emit(out, word);
    }
    auto mask = a > V(0);
    auto chosen = select(mask, a, -a);
    std::array<std::uint32_t, N> words{};
    chosen.store_bits(words.data());
    for (auto word : words) emit(out, word);
    auto integer = I::loadu(words.data());
    auto incremented = integer + I(3u);
    incremented.storeu(words.data());
    for (auto word : words) out.write(reinterpret_cast<char const *>(&word), sizeof(word));
  }
}

int main(int argc, char ** argv) {
  if (argc != 2) return 2;
  std::ofstream out(argv[1], std::ios::binary);
  if (!out) return 3;
  auto before = simd::test::read_fp_state();
  {
    simd::test::fp_scope controls(simd::test::fp_mode::flush);
    capture<1>(out);
#if defined(__AVX2__) || defined(__ARM_NEON)
    capture<4>(out);
#endif
#if defined(__AVX2__)
    capture<8>(out);
#endif
#if defined(__AVX512F__) && defined(__AVX512DQ__)
    capture<16>(out);
#endif
    if (!controls.controls_match()) return 5;
  }
  if (before != simd::test::read_fp_state()) return 6;
  out.close();
  return out ? 0 : 7;
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
