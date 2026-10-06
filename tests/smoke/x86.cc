// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <native/attributes.h>
#include <native/targets.h>
import native.x86;

namespace {
  template<class T, std::size_t N, auto A>
  native_inline constexpr auto lanes(native::simd<T,N,A> const & value) noexcept {
    if consteval {
      std::array<T,N> result{};
      value.store_memory(result.data());
      return result;
    } else {
      // Full-register fixtures have no padding; bit transport needs no ISA leaf.
      static_assert(sizeof(value) == sizeof(std::array<T,N>));
      return std::bit_cast<std::array<T,N>>(value);
    }
  }

  template<class T, std::size_t N, auto A>
  native_inline constexpr bool all(native::simd<T,N,A> const & value, T expected) noexcept {
    for (auto actual : lanes(value))
      if (actual != expected) return false;
    return true;
  }

  constexpr auto bmi1 = native::target_features<native::x86>("bmi");
  constexpr auto bmi2 = native::target_features<native::x86>("bmi2");
  constexpr auto popcnt = native::target_features<native::x86>("popcnt");
  constexpr auto lzcnt = native::target_features<native::x86>("lzcnt");
  constexpr auto crc = native::target_features<native::x86>("sse4.2");
  constexpr auto adx = native::target_features<native::x86>("adx");
  constexpr auto aes = native::target_features<native::x86>("aes");
  constexpr auto vaes = native::target_features<native::x86>("avx,vaes");
  constexpr auto pclmul = native::target_features<native::x86>("pclmul");
  constexpr auto vpclmul = native::target_features<native::x86>("avx,vpclmulqdq");
  constexpr auto gfni = native::target_features<native::x86>("gfni");
  constexpr auto sha = native::target_features<native::x86>("sha");
  constexpr auto sha512 = native::target_features<native::x86>("sha512");
  constexpr auto sm3 = native::target_features<native::x86>("sm3");
  constexpr auto sm4 = native::target_features<native::x86>("sm4");
  constexpr auto ifma = native::target_features<native::x86>("avx512ifma,avx512vl");
  constexpr auto vpopcnt = native::target_features<native::x86>("avx512vpopcntdq,avx512vl");
  constexpr auto bitalg = native::target_features<native::x86>("avx512bitalg,avx512vl");
  constexpr auto cd = native::target_features<native::x86>("avx512cd,avx512vl");
  constexpr auto vbmi = native::target_features<native::x86>("avx512vbmi,avx512vl");
  constexpr auto vbmi2 = native::target_features<native::x86>("avx512vbmi2,avx512vl");
  constexpr auto vnni = native::target_features<native::x86>("avxvnni");
  constexpr auto f16c = native::target_features<native::x86>("f16c");
  constexpr auto neconvert = native::target_features<native::x86>("avxneconvert");
  constexpr auto gather = native::target_features<native::x86>("avx2");
  constexpr auto scatter = native::target_features<native::x86>("avx512f,avx512vl");

  // Small known cases run through both constant evaluation and admitted hardware.
  // The volatile caller input prevents runtime operands from folding to constants.
  __attribute__((target("bmi"), noinline))
  constexpr bool check_bmi1(int one) noexcept {
    return native::blsi<bmi1>(std::uint32_t(12 * one)) == 4
      && native::blsr<bmi1>(std::uint32_t(12 * one)) == 8
      && native::tzcnt<bmi1>(std::uint32_t(one - 1)) == 32;
  }

  __attribute__((target("bmi2"), noinline))
  constexpr bool check_bmi2(int one) noexcept {
    std::uint64_t high{};
    auto low = native::mulx<bmi2>(~std::uint64_t{}, std::uint64_t(2 * one), &high);
    return native::pdep<bmi2>(std::uint32_t(3 * one), std::uint32_t{0x55}) == 5
      && native::pext<bmi2>(std::uint32_t(0x41 * one), std::uint32_t{0x55}) == 9
      && low == 0xfffffffffffffffe && high == 1;
  }

  __attribute__((target("popcnt"), noinline))
  constexpr bool check_popcnt(int one) noexcept {
    return native::popcnt<popcnt>(std::uint64_t(one - 1)) == 0
      && native::popcnt<popcnt>(~std::uint64_t(one - 1)) == 64;
  }

  __attribute__((target("lzcnt"), noinline))
  constexpr bool check_lzcnt(int one) noexcept {
    return native::lzcnt<lzcnt>(std::uint64_t(one - 1)) == 64
      && native::lzcnt<lzcnt>(std::uint64_t(one)) == 63;
  }

  __attribute__((target("sse4.2"), noinline))
  constexpr bool check_crc(int one) noexcept {
    return native::crc32c<crc>(std::uint32_t{}, std::uint8_t(one)) == 0xf26b8303;
  }

  __attribute__((target("adx"), noinline))
  constexpr bool check_adx(int one) noexcept {
    std::uint64_t sum{};
    auto carry = native::addcarryx<adx>(std::uint8_t(one), ~std::uint64_t{}, std::uint64_t{}, &sum);
    return carry == 1 && sum == 0;
  }

  __attribute__((target("aes"), noinline))
  constexpr bool check_aes(int one) noexcept {
    using B = native::simd<std::uint8_t,16,aes>;
    auto zero = B(std::uint8_t(one - 1));
    return all(native::aesenc(zero, zero), std::uint8_t{0x63})
      && all(native::aesdec(zero, zero), std::uint8_t{0x52})
      && all(native::aesimc(B(std::uint8_t(one))), std::uint8_t{1});
  }

  __attribute__((target("avx,vaes"), noinline))
  constexpr bool check_vaes(int one) noexcept {
    using B = native::simd<std::uint8_t,32,vaes>;
    auto zero = B(std::uint8_t(one - 1));
    return all(native::vaesenc(zero, zero), std::uint8_t{0x63})
      && all(native::vaesdeclast(zero, zero), std::uint8_t{0x52});
  }

  __attribute__((target("pclmul"), noinline))
  constexpr bool check_pclmul(int one) noexcept {
    using W = native::simd<std::uint64_t,2,pclmul>;
    return lanes(native::pclmulqdq<pclmul,0>(W(std::uint64_t(3 * one)), W(3)))
        == std::array<std::uint64_t,2>{5,0}
      && lanes(native::pclmulqdq<pclmul,0>(W(std::uint64_t{1} << 63), W(std::uint64_t(2 * one))))
        == std::array<std::uint64_t,2>{0,1};
  }

  __attribute__((target("avx,vpclmulqdq"), noinline))
  constexpr bool check_vpclmul(int one) noexcept {
    using W = native::simd<std::uint64_t,4,vpclmul>;
    return lanes(native::vpclmulqdq<vpclmul,0>(W(std::uint64_t(3 * one)), W(3)))
      == std::array<std::uint64_t,4>{5,0,5,0};
  }

  __attribute__((target("gfni"), noinline))
  constexpr bool check_gfni(int one) noexcept {
    using B = native::simd<std::uint8_t,16,gfni>;
    using W = native::simd<std::uint64_t,2,gfni>;
    return all(native::gf2p8mulb(B(std::uint8_t(0x57 * one)), B(0x83)), std::uint8_t{0xc1})
      && all(native::gf2p8affineinvqb<gfni,0>(B(std::uint8_t(one)), W(0x0102040810204080)), std::uint8_t{1});
  }

  __attribute__((target("sha"), noinline))
  constexpr bool check_sha(int one) noexcept {
    using W = native::simd<std::uint32_t,4,sha>;
    auto zero = W(std::uint32_t(one - 1));
    return all(native::sha1msg1(zero, zero), std::uint32_t{})
      && all(native::sha256msg1(W(std::uint32_t(one)), W(1)), std::uint32_t{0x02004001});
  }

  __attribute__((target("sha512"), noinline))
  constexpr bool check_sha512(int one) noexcept {
    using W = native::simd<std::uint64_t,4,sha512>;
    using H = native::simd<std::uint64_t,2,sha512>;
    return all(native::sha512msg1(W(std::uint64_t(one)), H(1)), std::uint64_t{0x8100000000000001});
  }

  __attribute__((target("sm3"), noinline))
  constexpr bool check_sm3(int one) noexcept {
    using W = native::simd<std::uint32_t,4,sm3>;
    auto zero = W(std::uint32_t(one - 1));
    return lanes(native::sm3msg1(zero, W(std::uint32_t(one)), zero))
      == std::array<std::uint32_t,4>{0x40008040,0x40008040,0x40008040,0};
  }

  __attribute__((target("sm4"), noinline))
  constexpr bool check_sm4(int one) noexcept {
    using W = native::simd<std::uint32_t,4,sm4>;
    auto original = W(std::array<std::uint32_t,4>{
      0x01234567 ^ std::uint32_t(one - 1),0x89abcdef,0xfedcba98,0x76543210});
    auto keys = W(std::array<std::uint32_t,4>{0xf12186f9,0x41662b61,0x5a6ab19a,0x7ba92077});
    return lanes(native::sm4rnds4(original, keys))
      == std::array<std::uint32_t,4>{0x27fad345,0xa18b4cb2,0x11c1e22a,0xcc13e2ee};
  }

  __attribute__((target("avx512ifma,avx512vl"), noinline))
  constexpr bool check_ifma(int one) noexcept {
    using W = native::simd<std::uint64_t,2,ifma>;
    return all(native::madd52lo(W(1), W(3), W(std::uint64_t(2 * one))), std::uint64_t{7})
      && all(native::madd52hi(W(0), W(std::uint64_t{1} << 51), W(std::uint64_t(2 * one))), std::uint64_t{1});
  }

  __attribute__((target("avx512vpopcntdq,avx512vl"), noinline))
  constexpr bool check_vpopcnt(int one) noexcept {
    using W = native::simd<std::uint32_t,4,vpopcnt>;
    return all(native::vpopcntd(W(~std::uint32_t(one - 1))), std::uint32_t{32})
      && all(native::vpopcntd(W(std::uint32_t(one - 1))), std::uint32_t{});
  }

  __attribute__((target("avx512bitalg,avx512vl"), noinline))
  constexpr bool check_bitalg(int one) noexcept {
    using B = native::simd<std::uint8_t,16,bitalg>;
    using W = native::simd<std::uint64_t,2,bitalg>;
    return all(native::vpopcntb(B(std::uint8_t(255 * one))), std::uint8_t{8})
      && native::vpshufbitqmb(W(std::uint64_t{1} << 63), B(63)).to_bitset() == 0xffff;
  }

  __attribute__((target("avx512cd,avx512vl"), noinline))
  constexpr bool check_cd(int one) noexcept {
    using W = native::simd<std::uint32_t,4,cd>;
    return lanes(native::vpconflictd(W(std::array<std::uint32_t,4>{std::uint32_t(one),2,1,1})))
        == std::array<std::uint32_t,4>{0,0,1,5}
      && all(native::vplzcntd(W(std::uint32_t(one - 1))), std::uint32_t{32});
  }

  __attribute__((target("avx512vbmi,avx512vl"), noinline))
  constexpr bool check_vbmi(int one) noexcept {
    using B = native::simd<std::uint8_t,16,vbmi>;
    using W = native::simd<std::uint64_t,2,vbmi>;
    return all(native::vpermt2b(B(7), B(16), B(std::uint8_t(one))), std::uint8_t{1})
      && all(native::vpmultishiftqb(B(63), W(std::uint64_t(one) << 63)), std::uint8_t{1});
  }

  __attribute__((target("avx512vbmi2,avx512vl"), noinline))
  constexpr bool check_vbmi2(int one) noexcept {
    using W = native::simd<std::uint16_t,8,vbmi2>;
    using B = native::simd<std::uint8_t,16,vbmi2>;
    auto mask = native::predicate<16,vbmi2>::from_bitset(0);
    return all(native::vpshldw<vbmi2,16>(W(7), W(std::uint16_t(one))), std::uint16_t{7})
      && all(native::maskz_vpcompressb(mask, B(std::uint8_t(one))), std::uint8_t{});
  }

  __attribute__((target("avxvnni"), noinline))
  constexpr bool check_vnni(int one) noexcept {
    using U = native::simd<std::uint8_t,16,vnni>;
    using S = native::simd<std::int8_t,16,vnni>;
    using I = native::simd<std::int32_t,4,vnni>;
    auto a = U(std::uint8_t(one));
    auto b = S(-1);
    return all(native::dpbusd(I(1), a, b), std::int32_t{-3})
      && all(native::dpbusds(I(-2147483647 - 1), a, b), std::int32_t{-2147483647 - 1});
  }

  __attribute__((target("f16c"), noinline))
  constexpr bool check_f16c(int one) noexcept {
    return native::cvtss_sh<f16c,0>(float(one)) == 0x3c00
      && native::cvtss_sh<f16c,0>(-0.f * one) == 0x8000
      && native::cvtsh_ss<f16c>(std::uint16_t(0x4000)) == 2.f;
  }

  __attribute__((target("avxneconvert"), noinline))
  constexpr bool check_neconvert(int one) noexcept {
    using F = native::simd<float,8,neconvert>;
    auto small = native::bf16::from_bits(std::uint16_t(one));
    return std::bit_cast<std::uint32_t>(lanes(native::bcstnebf16_ps<neconvert,4>(&small))[0]) == 0x00010000
      && lanes(native::cvtneps_bf16(F(float(one))))[0].to_bits() == 0x3f80;
  }

  __attribute__((target("avx2"), noinline))
  constexpr bool check_gather(int one) noexcept {
    using I = native::simd<std::int32_t,4,gather>;
    std::array<std::int32_t,4> source{11 * one,22,33,44};
    return lanes(native::vpgatherdd<4,4>(source.data(), I(std::array<std::int32_t,4>{3,2,1,0})))
        == std::array<std::int32_t,4>{44,33,22,11}
      && all(native::mask_vpgatherdd<4>(I(-one), I(0), static_cast<std::int32_t const *>(nullptr), I(0)), std::int32_t{-1});
  }

  __attribute__((target("avx512f,avx512vl"), noinline))
  constexpr bool check_scatter(int one) noexcept {
    using I = native::simd<std::int32_t,4,scatter>;
    std::array<std::int32_t,4> result{};
    native::mask_vpscatterdd<4>(result.data(), native::predicate<4,scatter>::from_bitset(3),
      I(std::array<std::int32_t,4>{0,0,1,1}), I(std::array<std::int32_t,4>{one,2,3,4}));
    return result == std::array<std::int32_t,4>{2,0,0,0};
  }

  __attribute__((target(NATIVE_TARGET_avx512_bf16), noinline))
  constexpr bool check_bf16(int one) noexcept {
    using B = native::simd<native::bf16,8,native::avx512_bf16>;
    using F = native::simd<float,4,native::avx512_bf16>;
    auto value = B(native::bf16::from_bits(0x3f80));
    return all(native::dot2(value, value, F(float(one))), 3.f);
  }

  __attribute__((target(NATIVE_TARGET_avx512_fp16), noinline))
  constexpr bool check_fp16(int one) noexcept {
    using H = native::simd<native::fp16,32,native::avx512_fp16>;
    auto value = H(native::fp16::from_bits(0x3c00));
    return lanes(native::fma(value, value, H(native::fp16::from_bits(std::uint16_t(0x3c00 * one)))))[0].to_bits() == 0x4000;
  }

  static_assert(check_bmi1(1) && check_bmi2(1) && check_popcnt(1) && check_lzcnt(1) && check_crc(1) && check_adx(1));
  static_assert(check_aes(1) && check_vaes(1) && check_pclmul(1) && check_vpclmul(1) && check_gfni(1));
  static_assert(check_sha(1) && check_sha512(1) && check_sm3(1) && check_sm4(1) && check_ifma(1));
  static_assert(check_vpopcnt(1) && check_bitalg(1) && check_cd(1) && check_vbmi(1) && check_vbmi2(1));
  static_assert(check_vnni(1) && check_f16c(1) && check_neconvert(1) && check_gather(1) && check_scatter(1));
  static_assert(check_bf16(1) && check_fp16(1));
  static_assert(native::waiter<native::spin> && native::waiter<native::mwaitx> && native::waiter<native::umwait>);
  static_assert(requires(std::uint64_t deadline) {
    { native::tpause<native::x86_feature::waitpkg>(deadline) } noexcept -> std::same_as<std::uint8_t>;
  });
}

int instruction_smoke() noexcept {
  auto cpu = native::observe_x86_capabilities();
  volatile int input = 1;
  int one = input;
  struct family {
    char const * name;
    native::isa<native::x86> requirement;
    bool (*check)(int) noexcept;
  };
  family const families[]{
    {"BMI1",bmi1,check_bmi1}, {"BMI2",bmi2,check_bmi2}, {"POPCNT",popcnt,check_popcnt},
    {"LZCNT",lzcnt,check_lzcnt}, {"CRC32C",crc,check_crc}, {"ADX",adx,check_adx},
    {"AES",aes,check_aes}, {"VAES",vaes,check_vaes}, {"PCLMUL",pclmul,check_pclmul},
    {"VPCLMUL",vpclmul,check_vpclmul}, {"GFNI",gfni,check_gfni}, {"SHA",sha,check_sha},
    {"SHA512",sha512,check_sha512}, {"SM3",sm3,check_sm3}, {"SM4",sm4,check_sm4},
    {"IFMA",ifma,check_ifma}, {"VPOPCNTDQ",vpopcnt,check_vpopcnt}, {"BITALG",bitalg,check_bitalg},
    {"AVX512CD",cd,check_cd}, {"VBMI",vbmi,check_vbmi}, {"VBMI2",vbmi2,check_vbmi2},
    {"VNNI",vnni,check_vnni}, {"F16C",f16c,check_f16c}, {"AVX-NE-CONVERT",neconvert,check_neconvert},
    {"Gather",gather,check_gather}, {"Scatter",scatter,check_scatter},
    {"BF16",native::avx512_bf16,check_bf16}, {"FP16",native::avx512_fp16,check_fp16}
  };
  for (auto const & item : families) {
    auto admitted = native::classify_isa(cpu, item.requirement, NATIVE_TARGET_MINIMUM);
    if (!admitted.admitted()) {
      std::printf("%s: constexpr passed; runtime unavailable (%s)\n", item.name, admitted.reason());
    } else if (!item.check(one)) {
      std::printf("%s instruction smoke failed\n", item.name);
      return 1;
    }
  }
  return 0;
}
