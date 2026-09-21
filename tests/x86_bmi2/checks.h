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

  template<class U> U rotate_oracle(U x, unsigned count) {
    for (unsigned i = 0; i != count % width<U>; ++i)
      x = U((x / 2) | ((x & 1) << (width<U> - 1)));
    return x;
  }

  template<class U> U bzhi_oracle(U x, unsigned index) {
    U result = 0;
    for (unsigned bit = 0; bit != width<U>; ++bit)
      if (bit < (index & 255u)) result |= x & (U{1} << bit);
    return result;
  }

  // Schoolbook multiplication in two words; no intrinsic or wider-integer dependency.
  template<class U> std::array<U, 2> product_oracle(U a, U b) {
    std::array<U, 2> result{};
    for (unsigned bit = 0; bit != width<U>; ++bit) {
      if (((b >> bit) & 1) == 0) continue;
      U low = U(a << bit);
      U high = bit ? U(a >> (width<U> - bit)) : U{0};
      U previous = result[0];
      result[0] += low;
      result[1] += U(high + U(result[0] < previous));
    }
    return result;
  }

  template<class U> U shift_oracle(U x, unsigned count, bool left, bool arithmetic) {
    U sign = arithmetic ? x & (U{1} << (width<U> - 1)) : 0;
    for (unsigned i = 0; i != count % width<U>; ++i)
      x = left ? U(x * 2) : U((x / 2) | sign);
    return x;
  }

  template<unsigned Imm8, class U>
  __attribute__((target("bmi2"))) bool check_rotate(U x) {
    return native::rorx<arch, Imm8>(x) == rotate_oracle(x, Imm8);
  }

  template<class U>
  __attribute__((target("bmi2"))) bool check_pair(U x, U y, unsigned random_count) {
    auto product = product_oracle(x, y);
    U high = ~product[1];
    U low = native::mulx<arch>(x, y, &high);
    if (low != product[0] || high != product[1]) return false;
    // The high-half write remains observable when the low half is discarded.
    high = ~product[1];
    static_cast<void>(native::mulx<arch>(x, y, &high));
    if (high != product[1]) return false;

    for (unsigned count : std::array<unsigned, 18>{0, 1, 7, 31, 32, 33, 63, 64, 65,
                                                  127, 128, 255, 256, 257, 511, 512,
                                                  ~0u, random_count}) {
      if (native::bzhi<arch>(x, count) != bzhi_oracle(x, count)) return false;
      if (native::shlx<arch>(x, count) != shift_oracle(x, count, true, false)) return false;
      if (native::shrx<arch>(x, count) != shift_oracle(x, count, false, false)) return false;
      using S = std::make_signed_t<U>;
      auto shifted = native::sarx<arch>(std::bit_cast<S>(x), count);
      if (std::bit_cast<U>(shifted) != shift_oracle(x, count, false, true)) return false;
    }
    return check_rotate<0>(x) && check_rotate<1>(x) && check_rotate<7>(x) &&
           check_rotate<31>(x) && check_rotate<32>(x) && check_rotate<63>(x) &&
           check_rotate<64>(x) && check_rotate<127>(x) && check_rotate<255>(x);
  }

  template<class U>
  __attribute__((target("bmi2"))) bool check_width(std::uint64_t seed) {
    constexpr U all = ~U{0};
    constexpr U sign = U{1} << (width<U> - 1);
    constexpr std::array<U, 8> edges{0, 1, 2, all, U(all - 1), sign,
                                    U(sign - 1), U(0xaaaaaaaaaaaaaaaaull)};
    for (U x : edges)
      for (U y : edges)
        if (!check_pair(x, y, unsigned(seed))) return false;
    auto next = [&] {
      seed ^= seed << 13;
      seed ^= seed >> 7;
      seed ^= seed << 17;
      return seed;
    };
    for (unsigned i = 0; i != 2048; ++i) {
      U x = U(next()), y = U(next());
      if (!check_pair(x, y, unsigned(next()))) return false;
    }
    return true;
  }

  __attribute__((target("bmi2"), noinline)) bool check(std::uint64_t seed) {
    return check_width<std::uint32_t>(seed) && check_width<std::uint64_t>(seed);
  }

  // This entry point stays at the compiler baseline; admission precedes every BMI2 call.
  int run(int argc) {
    auto cpu = native::observe_x86_capabilities();
    if (!native::classify_isa(cpu, arch).admitted()) {
      std::puts("BMI2 unavailable");
      return 77;
    }
    if (!check(0x6a09e667f3bcc909ull ^ unsigned(argc))) {
      std::fputs("BMI2 scalar oracle mismatch\n", stderr);
      return 1;
    }
    return 0;
  }
}
