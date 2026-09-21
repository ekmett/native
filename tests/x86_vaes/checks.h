// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "prelude.h"
#include "oracle.h"

namespace vaes_fixture {
  constexpr auto vex128 = native::target_features<native::x86>("avx,aes");
  constexpr auto vex256 = native::target_features<native::x86>("avx,vaes");
  constexpr auto evex512 = native::target_features<native::x86>("avx512f,vaes");
  constexpr auto weak128 = native::target_features<native::x86>("sse2");
  constexpr auto weak256 = native::target_features<native::x86>("avx");
  constexpr auto weak512 = native::target_features<native::x86>("avx512f");
  constexpr auto broad = native::avx512 & native::x86_feature::aes & native::x86_feature::vaes;

  template<unsigned N>
  using block = std::array<std::uint8_t, N>;
  template<unsigned N>
  using results = std::array<block<N>, 4>;

  template<unsigned N>
  constexpr results<N> oracle(block<N> const & a, block<N> const & key) {
    results<N> out{};
    for (unsigned base = 0; base < N; base += 16) {
      bytes state{}, round_key{};
      for (unsigned i = 0; i < 16; ++i) {
        state[i] = a[base + i];
        round_key[i] = key[base + i];
      }
      for (unsigned operation = 0; operation < 4; ++operation) {
        auto value = round(state, round_key, operation >= 2, operation % 2 != 0);
        for (unsigned i = 0; i < 16; ++i) {
          out[operation][base + i] = value[i];
        }
      }
    }
    return out;
  }

  template<native::isa<native::x86> A, unsigned N>
  consteval results<N> constant(block<N> const & a, block<N> const & key) {
    using vector = native::simd<std::uint8_t, N, A>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    results<N> out{};
    native::vaesenc<A>(x, y).store(out[0].data());
    native::vaesenclast<A>(x, y).store(out[1].data());
    native::vaesdec<A>(x, y).store(out[2].data());
    native::vaesdeclast<A>(x, y).store(out[3].data());
    return out;
  }

  template<native::isa<native::x86> Strong, native::isa<native::x86> Weak, unsigned N>
  consteval bool constant_bank() {
    for (unsigned trial = 0; trial < 16; ++trial) {
      block<N> state{}, key{};
      for (unsigned i = 0; i < N; ++i) {
        state[i] = static_cast<std::uint8_t>(trial * 16 + i);
        key[i] = static_cast<std::uint8_t>(255 - i * 7 + trial);
      }
      auto expected = oracle<N>(state, key);
      if (constant<Strong, N>(state, key) != expected || constant<Weak, N>(state, key) != expected ||
          constant<broad, N>(state, key) != expected) {
        return false;
      }
    }
    return true;
  }
  static_assert(constant_bank<vex128, weak128, 16>());
  static_assert(constant_bank<vex256, weak256, 32>());
  static_assert(constant_bank<evex512, weak512, 64>());

  // FIPS 197 Appendix B's first round, repeated with independently keyed lanes
  // above. These fixed bytes anchor the table oracle to a published AES vector.
  constexpr bytes initial{0x19, 0x3d, 0xe3, 0xbe, 0xa0, 0xf4, 0xe2, 0x2b,
    0x9a, 0xc6, 0x8d, 0x2a, 0xe9, 0xf8, 0x48, 0x08};
  constexpr bytes round_key{0xa0, 0xfa, 0xfe, 0x17, 0x88, 0x54, 0x2c, 0xb1,
    0x23, 0xa3, 0x39, 0x39, 0x2a, 0x6c, 0x76, 0x05};
  constexpr bytes first{0xa4, 0x9c, 0x7f, 0xf2, 0x68, 0x9f, 0x35, 0x2b,
    0x6b, 0x5b, 0xea, 0x43, 0x02, 0x6a, 0x50, 0x49};
  static_assert(constant<vex128, 16>(initial, round_key)[0] == first);

  native_noinline native_target("avx,aes")
  void evaluate128(block<16> const & a, block<16> const & key, results<16> & out) noexcept {
    using vector = native::simd<std::uint8_t, 16, vex128>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    native::vaesenc<vex128>(x, y).store(out[0].data());
    native::vaesenclast<vex128>(x, y).store(out[1].data());
    native::vaesdec<vex128>(x, y).store(out[2].data());
    native::vaesdeclast<vex128>(x, y).store(out[3].data());
  }

  native_noinline native_target("avx,vaes")
  void evaluate256(block<32> const & a, block<32> const & key, results<32> & out) noexcept {
    using vector = native::simd<std::uint8_t, 32, vex256>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    native::vaesenc<vex256>(x, y).store(out[0].data());
    native::vaesenclast<vex256>(x, y).store(out[1].data());
    native::vaesdec<vex256>(x, y).store(out[2].data());
    native::vaesdeclast<vex256>(x, y).store(out[3].data());
  }

  native_noinline native_target("avx512f,vaes")
  void evaluate512(block<64> const & a, block<64> const & key, results<64> & out) noexcept {
    using vector = native::simd<std::uint8_t, 64, evex512>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    native::vaesenc<evex512>(x, y).store(out[0].data());
    native::vaesenclast<evex512>(x, y).store(out[1].data());
    native::vaesdec<evex512>(x, y).store(out[2].data());
    native::vaesdeclast<evex512>(x, y).store(out[3].data());
  }

  template<unsigned N>
  int exercise(native::isa<native::x86> requirements, void (*evaluate)(block<N> const &, block<N> const &, results<N> &) noexcept) {
    auto admission = native::classify_isa(native::observe_x86_capabilities(), requirements);
    if (!admission.admitted()) {
      std::printf("SKIP VAES %u-bit runtime: %s\n", N * 8, admission.reason());
      return 77;
    }
    std::uint32_t random = 0x9e3779b9;
    for (unsigned trial = 0; trial < 1024; ++trial) {
      block<N> a{}, key{};
      for (auto * input : {&a, &key}) {
        for (auto & lane : *input) {
          random ^= random << 13;
          random ^= random >> 17;
          random ^= random << 5;
          lane = static_cast<std::uint8_t>(trial == 0 ? 0 : trial == 1 ? 255 : random);
        }
      }
      results<N> out{};
      evaluate(a, key, out);
      auto expected = oracle<N>(a, key);
      for (unsigned operation = 0; operation < out.size(); ++operation) {
        if (out[operation] != expected[operation]) {
          std::printf("VAES %u-bit operation %u failed trial %u\n", N * 8, operation, trial);
          return 1;
        }
      }
    }
    return 0;
  }

  int run() {
    int a = exercise<16>(vex128, evaluate128);
    int b = exercise<32>(vex256, evaluate256);
    int c = exercise<64>(evex512, evaluate512);
    return a == 1 || b == 1 || c == 1 ? 1 : a == 77 && b == 77 && c == 77 ? 77 : 0;
  }
}
