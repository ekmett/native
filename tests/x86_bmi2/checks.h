// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

namespace bmi2_fixture {
  constexpr native::isa<native::x86> arch{native::x86_feature::bmi2};
  constexpr native::isa<native::x86> unrelated{native::x86_feature::avx2};
  static_assert(!arch.has(native::x86_feature::bmi1));
  static_assert(!arch.has(native::x86_feature::avx2));

  template<native::isa<native::x86> A, class U> concept has_unsigned_ops = requires(U x, U* high, unsigned count) {
    { native::bzhi<A>(x, count) } noexcept -> std::same_as<U>;
    { native::mulx<A>(x, x, high) } noexcept -> std::same_as<U>;
    { native::pdep<A>(x, x) } noexcept -> std::same_as<U>;
    { native::pext<A>(x, x) } noexcept -> std::same_as<U>;
    { native::shlx<A>(x, count) } noexcept -> std::same_as<U>;
    { native::shrx<A>(x, count) } noexcept -> std::same_as<U>;
  };
  template<native::isa<native::x86> A, class S> concept has_signed_shift = requires(S x, unsigned count) {
    { native::sarx<A>(x, count) } noexcept -> std::same_as<S>;
  };
  template<native::isa<native::x86> A, unsigned Imm8, class U> concept has_rotate = requires(U x) {
    { native::rorx<A, Imm8>(x) } noexcept -> std::same_as<U>;
  };

  // Syntactic availability includes consteval fallbacks, not runtime support.
  static_assert(has_unsigned_ops<arch, std::uint32_t> && has_unsigned_ops<arch, std::uint64_t>);
  static_assert(has_unsigned_ops<native::isa<native::x86>{}, std::uint32_t> &&
                has_unsigned_ops<native::isa<native::x86>{}, std::uint64_t>);
  static_assert(has_unsigned_ops<unrelated, std::uint32_t> &&
                has_unsigned_ops<unrelated, std::uint64_t>);
  static_assert(has_signed_shift<arch, std::int32_t> && has_signed_shift<arch, std::int64_t>);
  static_assert(has_signed_shift<native::isa<native::x86>{}, std::int32_t> &&
                has_signed_shift<native::isa<native::x86>{}, std::int64_t>);
  static_assert(has_signed_shift<unrelated, std::int32_t> && has_signed_shift<unrelated, std::int64_t>);
  static_assert(has_rotate<arch, 0, std::uint32_t> && has_rotate<arch, 255, std::uint64_t>);
  static_assert(!has_rotate<arch, 256, std::uint32_t> && !has_rotate<arch, 256, std::uint64_t>);
  static_assert(!has_rotate<native::isa<native::x86>{}, 256, std::uint32_t> &&
                !has_rotate<native::isa<native::x86>{}, 256, std::uint64_t>);
  static_assert(has_rotate<native::isa<native::x86>{}, 0, std::uint32_t> &&
                has_rotate<native::isa<native::x86>{}, 0, std::uint64_t>);
  static_assert(has_rotate<unrelated, 7, std::uint32_t> && has_rotate<unrelated, 7, std::uint64_t>);

  template<class U> constexpr unsigned width = std::numeric_limits<U>::digits;

  template<class U>
  __attribute__((target("bmi2"))) bool check_width() {
    constexpr U all = ~U{0};
    constexpr U high_bit = U{1} << (width<U> - 1);
    struct permutation_case { U value, mask, deposited, extracted; };
    constexpr std::array<permutation_case, 7> permutations{{
      {0, all, 0, 0}, {all, 0, 0, 0}, {U{0xa5}, all, U{0xa5}, U{0xa5}},
      {5, 22, 18, 2}, {all, U(high_bit | 1), U(high_bit | 1), 3},
      {1, high_bit, high_bit, 0}, {high_bit, high_bit, 0, 1}
    }};
    for (auto const& c : permutations) {
      // Volatile loads keep these checks on the runtime intrinsic path.
      volatile U value = c.value, mask = c.mask;
      if (native::pdep<arch>(U(value), U(mask)) != c.deposited ||
          native::pext<arch>(U(value), U(mask)) != c.extracted) return false;
    }

    struct product_case { U a, b, low, high; };
    constexpr std::array<product_case, 3> products{{
      {0, all, 0, 0}, {all, all, 1, U(all - 1)}, {high_bit, 2, 0, 1}
    }};
    for (auto const& c : products) {
      volatile U a = c.a, b = c.b;
      U high = ~c.high;
      if (native::mulx<arch>(U(a), U(b), &high) != c.low || high != c.high) return false;
      // The high-half write remains observable when the low half is discarded.
      high = ~c.high;
      static_cast<void>(native::mulx<arch>(U(a), U(b), &high));
      if (high != c.high) return false;
    }

    struct zero_high_case { unsigned index; U expected; };
    constexpr std::array<zero_high_case, 7> indices{{
      {0, 0}, {1, 1}, {width<U> - 1, U(high_bit - 1)}, {width<U>, all},
      {255, all}, {256, 0}, {257, 1}
    }};
    volatile U input = all;
    for (auto const& c : indices) {
      volatile unsigned index = c.index;
      if (native::bzhi<arch>(U(input), unsigned(index)) != c.expected) return false;
    }

    input = U(high_bit | 3);
    using S = std::make_signed_t<U>;
    for (unsigned c : std::array<unsigned, 6>{0, 1, width<U> - 1, width<U>, width<U> + 1, 256}) {
      volatile unsigned count = c;
      U x = input;
      unsigned n = c % width<U>;
      U arithmetic = U(x >> n);
      if (n) arithmetic |= U(all << (width<U> - n));
      if (native::shlx<arch>(x, unsigned(count)) != U(x << n) ||
          native::shrx<arch>(x, unsigned(count)) != U(x >> n) ||
          std::bit_cast<U>(native::sarx<arch>(std::bit_cast<S>(x), unsigned(count))) != arithmetic)
        return false;
    }
    U x = input;
    return native::rorx<arch, 0>(x) == x &&
      native::rorx<arch, 1>(x) == U(high_bit | (high_bit >> 1) | 1) &&
      native::rorx<arch, width<U>>(x) == x && native::rorx<arch, 255>(x) == U{7};
  }

  __attribute__((target("bmi2"), noinline)) bool check() {
    return check_width<std::uint32_t>() && check_width<std::uint64_t>();
  }

  // This entry point stays at the compiler baseline; admission precedes every BMI2 call.
  int run() {
    auto cpu = native::observe_x86_capabilities();
    if (!native::classify_isa(cpu, arch).admitted()) {
      std::puts("BMI2 unavailable");
      return 77;
    }
    if (!check()) {
      std::fputs("BMI2 wrapper mismatch\n", stderr);
      return 1;
    }
    return 0;
  }
}
