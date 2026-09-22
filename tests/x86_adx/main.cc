// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
#include <native/attributes.h>

#if defined(__ADX__) || defined(__AVXIFMA__) || defined(__AVX512IFMA__) || defined(__AVX__)
#error IFMA/ADX admission must run at the provider baseline
#endif
#if NATIVE_TEST_INTERFACE == 1
import native.x86.adx;
import native.features;
#else
import native;
#endif
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0


namespace adx_fixture {
  constexpr auto strong = native::target_features<native::x86>("adx");
  constexpr native::isa<native::x86> weak{};

  template<native::isa<native::x86> A, class U>
  concept carry_shape = requires(std::uint8_t carry, U value, U * output) {
    { native::addcarryx<A>(carry, value, value, output) } noexcept -> std::same_as<std::uint8_t>;
  };
  static_assert(carry_shape<strong, std::uint32_t> && carry_shape<strong, std::uint64_t>);
  static_assert(carry_shape<weak, std::uint32_t> && carry_shape<weak, std::uint64_t>);
  static_assert(!carry_shape<strong, std::int32_t> && !carry_shape<strong, std::int64_t>);

  template<class U>
  using result = std::array<U, 2>;

  // Bitwise full-adder reference, independent of the implementation's two
  // unsigned additions and overflow comparisons. Every nonzero carry means 1.
  template<class U>
  constexpr result<U> oracle(std::uint8_t carry, U a, U b) {
    U sum = 0;
    unsigned next = carry != 0;
    for (unsigned bit = 0; bit < sizeof(U) * 8; ++bit) {
      auto value = unsigned((a >> bit) & 1) + unsigned((b >> bit) & 1) + next;
      sum |= U(value & 1) << bit;
      next = value >> 1;
    }
    return {sum, U(next)};
  }

  template<native::isa<native::x86> A, class U>
  consteval result<U> constant(std::uint8_t carry, U a, U b) {
    U sum{};
    auto next = native::addcarryx<A>(carry, a, b, &sum);
    return {sum, U(next)};
  }

  template<class U>
  consteval bool constant_bank() {
    for (unsigned carry = 0; carry < 256; ++carry) {
      U a = carry % 3 == 0 ? ~U{0} : U{1} << (carry % (sizeof(U) * 8));
      U b = carry % 3 == 1 ? ~a : static_cast<U>(carry * 0x9e3779b9u);
      auto expected = oracle<U>(static_cast<std::uint8_t>(carry), a, b);
      if (constant<strong>(static_cast<std::uint8_t>(carry), a, b) != expected ||
          constant<weak>(static_cast<std::uint8_t>(carry), a, b) != expected) {
        return false;
      }
      U sum{};
      auto next = native::addcarryx(static_cast<std::uint8_t>(carry), a, b, &sum);
      if (result<U>{sum, U(next)} != expected) {
        return false;
      }
    }
    return true;
  }
  static_assert(constant_bank<std::uint32_t>() && constant_bank<std::uint64_t>());
  static_assert(constant<strong>(1, ~std::uint32_t{0}, std::uint32_t{0}) == result<std::uint32_t>{0, 1});
  static_assert(constant<weak>(255, ~std::uint64_t{0}, ~std::uint64_t{0}) ==
    result<std::uint64_t>{~std::uint64_t{0}, 1});

  native_noinline native_target("adx")
  void evaluate(std::uint8_t carry, std::uint64_t a, std::uint64_t b,
    result<std::uint32_t> & narrow, result<std::uint64_t> & wide) noexcept {
    narrow[1] = native::addcarryx<strong>(carry, std::uint32_t(a), std::uint32_t(b), &narrow[0]);
    wide[1] = native::addcarryx<strong>(carry, a, b, &wide[0]);
  }

  native_noinline native_target("adx")
  std::uint8_t chain(std::array<std::uint64_t, 4> const & a,
    std::array<std::uint64_t, 4> const & b, std::array<std::uint64_t, 4> & sum) noexcept {
    std::uint8_t carry = 0;
    for (unsigned lane = 0; lane < 4; ++lane) {
      carry = native::addcarryx<strong>(carry, a[lane], b[lane], &sum[lane]);
    }
    return carry;
  }

  int run() {
    auto admission = native::classify_isa(native::observe_x86_capabilities(), strong);
    if (!admission.admitted()) {
      std::printf("SKIP ADX runtime: %s\n", admission.reason());
      return 77;
    }
    std::uint64_t random = 0x243f6a8885a308d3;
    for (unsigned trial = 0; trial < 4096; ++trial) {
      random ^= random << 13;
      random ^= random >> 7;
      random ^= random << 17;
      auto a = trial < 256 ? ~std::uint64_t{0} : random;
      auto b = std::rotl(random, 29);
      auto carry = static_cast<std::uint8_t>(trial);
      result<std::uint32_t> narrow{};
      result<std::uint64_t> wide{};
      evaluate(carry, a, b, narrow, wide);
      if (narrow != oracle(carry, std::uint32_t(a), std::uint32_t(b)) || wide != oracle(carry, a, b)) {
        std::printf("ADX scalar case %u failed\n", trial);
        return 1;
      }
      std::array<std::uint64_t, 4> left{a, ~std::uint64_t{0}, 0, b};
      std::array<std::uint64_t, 4> right{b, 0, ~std::uint64_t{0}, a};
      std::array<std::uint64_t, 4> sum{};
      auto next = chain(left, right, sum);
      std::uint8_t expected_carry = 0;
      for (unsigned lane = 0; lane < 4; ++lane) {
        auto expected = oracle(expected_carry, left[lane], right[lane]);
        if (sum[lane] != expected[0]) {
          return 1;
        }
        expected_carry = static_cast<std::uint8_t>(expected[1]);
      }
      if (next != expected_carry) {
        return 1;
      }
    }
    return 0;
  }
}

extern "C" native_noinline unsigned native_adx_baseline_import(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}

int main() {
  return adx_fixture::run();
}
