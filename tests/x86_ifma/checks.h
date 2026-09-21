// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "prelude.h"
#include "oracle.h"

namespace ifma_fixture {
  constexpr auto vex = native::target_features<native::x86>("avxifma");
  constexpr auto evex = native::target_features<native::x86>("avx512ifma");
  constexpr auto evex_vl = native::target_features<native::x86>("avx512ifma,avx512vl");
  constexpr auto weak128 = native::target_features<native::x86>("sse2");
  constexpr auto weak256 = native::target_features<native::x86>("avx");
  constexpr auto weak512 = native::target_features<native::x86>("avx512f");
  constexpr auto broad = native::avx512 & native::x86_feature::avx512ifma;

  template<native::isa<native::x86> A, unsigned N, bool Masked>
  consteval results<N> constant(words<N> const & accumulator, words<N> const & a,
    words<N> const & b, std::uint64_t bits) {
    using vector = native::simd<std::uint64_t, N, A>;
    auto sum = vector::load(accumulator.data());
    auto x = vector::load(a.data());
    auto y = vector::load(b.data());
    results<N> out{};
    native::madd52lo<A>(sum, x, y).store(out[0].data());
    native::madd52hi<A>(sum, x, y).store(out[3].data());
    if constexpr (Masked) {
      auto mask = native::predicate<N, A>::from_bitset(bits);
      native::mask_madd52lo<A>(sum, mask, x, y).store(out[1].data());
      native::maskz_madd52lo<A>(mask, sum, x, y).store(out[2].data());
      native::mask_madd52hi<A>(sum, mask, x, y).store(out[4].data());
      native::maskz_madd52hi<A>(mask, sum, x, y).store(out[5].data());
    }
    return out;
  }

  template<native::isa<native::x86> A, native::isa<native::x86> Weak, unsigned N, bool Masked>
  consteval bool constant_bank() {
    for (unsigned trial = 0; trial < 32; ++trial) {
      words<N> accumulator{}, a{}, b{};
      for (unsigned lane = 0; lane < N; ++lane) {
        accumulator[lane] = ~std::uint64_t{0} - lane * 0x123456789ull;
        a[lane] = std::rotl(0xfedcba9876543210ull + trial, lane * 7 + trial);
        b[lane] = trial % 3 ? ~a[lane] : ~std::uint64_t{0};
      }
      auto mask = std::uint64_t(trial * 0x9b) | (std::uint64_t{0xffff} << N);
      auto expected = oracle<N>(accumulator, a, b, mask);
      auto out = constant<A, N, Masked>(accumulator, a, b, mask);
      auto weaker = constant<Weak, N, Masked>(accumulator, a, b, mask);
      for (unsigned operation = 0; operation < 6; ++operation) {
        if ((Masked || operation % 3 == 0) &&
            (out[operation] != expected[operation] || weaker[operation] != expected[operation])) {
          return false;
        }
      }
    }
    return true;
  }
  static_assert(constant_bank<vex, weak128, 2, false>());
  static_assert(constant_bank<vex, weak256, 4, false>());
  static_assert(constant_bank<evex_vl, weak128, 2, true>());
  static_assert(constant_bank<evex_vl, weak256, 4, true>());
  static_assert(constant_bank<evex, weak512, 8, true>());
  static_assert(constant_bank<broad, weak512, 8, true>());
  static_assert(constant_bank<broad & native::x86_feature::avxifma, weak256, 4, true>());

  consteval bool boundary_cases() {
    constexpr words<2> accumulator{~std::uint64_t{0}, 1};
    constexpr words<2> a{~std::uint64_t{0}, std::uint64_t{1} << 52};
    constexpr words<2> b{~std::uint64_t{0}, ~std::uint64_t{0}};
    auto out = constant<evex_vl, 2, true>(accumulator, a, b, 0);
    return out[0] == words<2>{0, 1} &&
      out[3] == words<2>{(std::uint64_t{1} << 52) - 3, 1} &&
      out[1] == accumulator && out[2] == words<2>{} &&
      out[4] == accumulator && out[5] == words<2>{};
  }
  static_assert(boundary_cases());

  native_noinline native_target("avxifma")
  void evaluate_vex128(words<2> const & accumulator, words<2> const & a, words<2> const & b,
    std::uint64_t bits, results<2> & out) noexcept {
    using vector = native::simd<std::uint64_t, 2, vex>;
    auto sum = vector::load(accumulator.data());
    auto x = vector::load(a.data());
    auto y = vector::load(b.data());
    native::madd52lo<vex>(sum, x, y).store(out[0].data());
    native::madd52hi<vex>(sum, x, y).store(out[3].data());
  }

  native_noinline native_target("avxifma")
  void evaluate_vex256(words<4> const & accumulator, words<4> const & a, words<4> const & b,
    std::uint64_t bits, results<4> & out) noexcept {
    using vector = native::simd<std::uint64_t, 4, vex>;
    auto sum = vector::load(accumulator.data());
    auto x = vector::load(a.data());
    auto y = vector::load(b.data());
    native::madd52lo<vex>(sum, x, y).store(out[0].data());
    native::madd52hi<vex>(sum, x, y).store(out[3].data());
  }

  native_noinline native_target("avx512ifma,avx512vl")
  void evaluate_evex128(words<2> const & accumulator, words<2> const & a, words<2> const & b,
    std::uint64_t bits, results<2> & out) noexcept {
    using vector = native::simd<std::uint64_t, 2, evex_vl>;
    auto sum = vector::load(accumulator.data());
    auto x = vector::load(a.data());
    auto y = vector::load(b.data());
    auto mask = native::predicate<2, evex_vl>::from_bitset(bits);
    native::madd52lo<evex_vl>(sum, x, y).store(out[0].data());
    native::mask_madd52lo<evex_vl>(sum, mask, x, y).store(out[1].data());
    native::maskz_madd52lo<evex_vl>(mask, sum, x, y).store(out[2].data());
    native::madd52hi<evex_vl>(sum, x, y).store(out[3].data());
    native::mask_madd52hi<evex_vl>(sum, mask, x, y).store(out[4].data());
    native::maskz_madd52hi<evex_vl>(mask, sum, x, y).store(out[5].data());
  }

  native_noinline native_target("avx512ifma,avx512vl")
  void evaluate_evex256(words<4> const & accumulator, words<4> const & a, words<4> const & b,
    std::uint64_t bits, results<4> & out) noexcept {
    using vector = native::simd<std::uint64_t, 4, evex_vl>;
    auto sum = vector::load(accumulator.data());
    auto x = vector::load(a.data());
    auto y = vector::load(b.data());
    auto mask = native::predicate<4, evex_vl>::from_bitset(bits);
    native::madd52lo<evex_vl>(sum, x, y).store(out[0].data());
    native::mask_madd52lo<evex_vl>(sum, mask, x, y).store(out[1].data());
    native::maskz_madd52lo<evex_vl>(mask, sum, x, y).store(out[2].data());
    native::madd52hi<evex_vl>(sum, x, y).store(out[3].data());
    native::mask_madd52hi<evex_vl>(sum, mask, x, y).store(out[4].data());
    native::maskz_madd52hi<evex_vl>(mask, sum, x, y).store(out[5].data());
  }

  native_noinline native_target("avx512ifma")
  void evaluate_evex512(words<8> const & accumulator, words<8> const & a, words<8> const & b,
    std::uint64_t bits, results<8> & out) noexcept {
    using vector = native::simd<std::uint64_t, 8, evex>;
    auto sum = vector::load(accumulator.data());
    auto x = vector::load(a.data());
    auto y = vector::load(b.data());
    auto mask = native::predicate<8, evex>::from_bitset(bits);
    native::madd52lo<evex>(sum, x, y).store(out[0].data());
    native::mask_madd52lo<evex>(sum, mask, x, y).store(out[1].data());
    native::maskz_madd52lo<evex>(mask, sum, x, y).store(out[2].data());
    native::madd52hi<evex>(sum, x, y).store(out[3].data());
    native::mask_madd52hi<evex>(sum, mask, x, y).store(out[4].data());
    native::maskz_madd52hi<evex>(mask, sum, x, y).store(out[5].data());
  }

  template<unsigned N, bool Masked>
  int exercise(char const * name, native::isa<native::x86> requirements,
    void (*evaluate)(words<N> const &, words<N> const &, words<N> const &, std::uint64_t, results<N> &) noexcept) {
    auto admission = native::classify_isa(native::observe_x86_capabilities(), requirements);
    if (!admission.admitted()) {
      std::printf("SKIP IFMA %s runtime: %s\n", name, admission.reason());
      return 77;
    }
    std::uint64_t random = 0x243f6a8885a308d3;
    for (unsigned trial = 0; trial < 1024; ++trial) {
      words<N> accumulator{}, a{}, b{};
      for (auto * input : {&accumulator, &a, &b}) {
        for (auto & lane : *input) {
          random ^= random << 13;
          random ^= random >> 7;
          random ^= random << 17;
          lane = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : random;
        }
      }
      auto mask = trial < 256 ? trial : random;
      results<N> out{};
      evaluate(accumulator, a, b, mask, out);
      auto expected = oracle<N>(accumulator, a, b, mask);
      for (unsigned operation = 0; operation < 6; ++operation) {
        if ((Masked || operation % 3 == 0) && out[operation] != expected[operation]) {
          std::printf("IFMA %s operation %u failed trial %u\n", name, operation, trial);
          return 1;
        }
      }
    }
    return 0;
  }

  int run() {
    int failures = 0;
    int admitted = 0;
    auto vex128 = exercise<2, false>("vex128", vex, evaluate_vex128);
    failures += vex128 == 1;
    admitted += vex128 != 77;
    auto vex256 = exercise<4, false>("vex256", vex, evaluate_vex256);
    failures += vex256 == 1;
    admitted += vex256 != 77;
    auto evex128 = exercise<2, true>("evex128", evex_vl, evaluate_evex128);
    failures += evex128 == 1;
    admitted += evex128 != 77;
    auto evex256 = exercise<4, true>("evex256", evex_vl, evaluate_evex256);
    failures += evex256 == 1;
    admitted += evex256 != 77;
    auto evex512 = exercise<8, true>("evex512", evex, evaluate_evex512);
    failures += evex512 == 1;
    admitted += evex512 != 77;
    return failures ? 1 : admitted ? 0 : 77;
  }
}
