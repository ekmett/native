// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <native/targets.h>
import native.arm;

namespace {
  template<class T, std::size_t N, auto A>
  constexpr auto lanes(native::simd<T,N,A> value) noexcept {
    std::array<T,N> result{};
    value.store_memory(result.data());
    return result;
  }

  template<class T, std::size_t N, auto A>
  constexpr bool all(native::simd<T,N,A> value, T expected) noexcept {
    for (auto actual : lanes(value))
      if (actual != expected) return false;
    return true;
  }

  constexpr auto neon = native::target_features<native::arm>("neon");
  constexpr auto crc = native::target_features<native::arm>("crc");
  constexpr auto aes = native::target_features<native::arm>("aes");
  constexpr auto sha2 = native::target_features<native::arm>("sha2");
  constexpr auto sha3 = native::target_features<native::arm>("sha3");
  constexpr auto sm4 = native::target_features<native::arm>("sm4");
  constexpr auto dot = native::target_features<native::arm>("dotprod");
  constexpr auto rdm = native::target_features<native::arm>("rdm");
  constexpr auto fhm = native::target_features<native::arm>("fp16fml");
  constexpr auto bf16 = native::target_features<native::arm>("bf16");
  constexpr auto fcma = native::target_features<native::arm>("complxnum");
  constexpr auto i8mm = native::target_features<native::arm>("i8mm");
  constexpr auto jscvt = native::target_features<native::arm>("jsconv");

  // The same tiny cases validate the constant evaluator and admitted hardware.
  // A volatile caller input keeps runtime instruction operands from folding.
  __attribute__((target("neon"), noinline))
  constexpr bool check_neon(int one) noexcept {
    using S = native::simd<std::int16_t,8,neon>;
    using B = native::simd<std::int8_t,8,neon>;
    return all(native::sqadd(S(32767), S(std::int16_t(one))), std::int16_t{32767})
      && lanes(native::sqxtn(S(std::array<std::int16_t,8>{-129,-128,-1,0,1,127,128,256})))
        == std::array<std::int8_t,8>{-128,-128,-1,0,1,127,127,127}
      && all(native::sshl(B(std::int8_t(-2 * one)), B(-1)), std::int8_t{-1});
  }

  __attribute__((target("crc"), noinline))
  constexpr bool check_crc(int one) noexcept {
    return native::crc32<crc>(std::uint32_t{}, std::uint8_t(one)) == 0x77073096
      && native::crc32c<crc>(std::uint32_t{}, std::uint8_t(one)) == 0xf26b8303;
  }

  __attribute__((target("aes"), noinline))
  constexpr bool check_aes(int one) noexcept {
    using B = native::simd<std::uint8_t,16,aes>;
    auto zero = B(std::uint8_t(one - 1));
    return all(native::aese(zero, zero), std::uint8_t{0x63})
      && all(native::aesd(zero, zero), std::uint8_t{0x52})
      && all(native::aesmc(B(std::uint8_t(one))), std::uint8_t{1});
  }

  __attribute__((target("aes"), noinline))
  constexpr bool check_pmull(int one) noexcept {
    return lanes(native::pmull<aes>(std::uint64_t(3 * one), std::uint64_t{3}))
        == std::array<std::uint64_t,2>{5,0}
      && lanes(native::pmull<aes>(std::uint64_t{1} << 63, std::uint64_t(2 * one)))
        == std::array<std::uint64_t,2>{0,1};
  }

  __attribute__((target("sha2"), noinline))
  constexpr bool check_sha2(int one) noexcept {
    using W = native::simd<std::uint32_t,4,sha2>;
    auto zero = W(std::uint32_t(one - 1));
    return native::sha1h<sha2>(std::uint32_t(3 * one)) == 0xc0000000
      && all(native::sha1c(zero, std::uint32_t{}, zero), std::uint32_t{})
      && all(native::sha256su0(zero, zero), std::uint32_t{});
  }

  __attribute__((target("sha3"), noinline))
  constexpr bool check_sha3(int one) noexcept {
    using W = native::simd<std::uint64_t,2,sha3>;
    auto zero = W(std::uint64_t(one - 1));
    return all(native::sha512su1(zero, zero, zero), std::uint64_t{})
      && all(native::eor3(W(std::uint64_t(one)), W(2), W(4)), std::uint64_t{7})
      && all(native::xar<sha3,1>(W(std::uint64_t(one)), W(3)), std::uint64_t{1});
  }

  __attribute__((target("sm4"), noinline))
  constexpr bool check_sm3(int one) noexcept {
    using W = native::simd<std::uint32_t,4,sm4>;
    auto ones = W(std::uint32_t(one)), zero = W(std::uint32_t(one - 1));
    return lanes(native::sm3ss1(ones, ones, ones))
        == std::array<std::uint32_t,4>{0,0,0,0x80100}
      && lanes(native::sm3tt1a<sm4,0>(zero, zero, ones))
        == std::array<std::uint32_t,4>{0,0,0,1};
  }

  __attribute__((target("sm4"), noinline))
  constexpr bool check_sm4(int one) noexcept {
    using W = native::simd<std::uint32_t,4,sm4>;
    auto original = W(std::array<std::uint32_t,4>{
      0x01234567 ^ std::uint32_t(one - 1),0x89abcdef,0xfedcba98,0x76543210});
    auto keys = W(std::array<std::uint32_t,4>{0xf12186f9,0x41662b61,0x5a6ab19a,0x7ba92077});
    return lanes(native::sm4e(original, keys))
      == std::array<std::uint32_t,4>{0x27fad345,0xa18b4cb2,0x11c1e22a,0xcc13e2ee};
  }

  __attribute__((target("dotprod"), noinline))
  constexpr bool check_dot(int one) noexcept {
    using S = native::simd<std::int8_t,16,dot>;
    using U = native::simd<std::uint8_t,16,dot>;
    using I = native::simd<std::int32_t,4,dot>;
    using W = native::simd<std::uint32_t,4,dot>;
    return all(native::sdot(I(1), S(std::int8_t(-one)), S(2)), std::int32_t{-7})
      && all(native::udot(W(0xffffffff), U(std::uint8_t(one)), U(1)), std::uint32_t{3});
  }

  __attribute__((target("rdm"), noinline))
  constexpr bool check_rdm(int one) noexcept {
    using S = native::simd<std::int16_t,4,rdm>;
    // Minimum-times-minimum cancels before saturation: this is not mulh + add.
    return all(native::sqrdmlah(S(-32768), S(-32768), S(std::int16_t(-32768 * one))), std::int16_t{})
      && all(native::sqrdmlah(S(32767), S(16384), S(std::int16_t(16384 * one))), std::int16_t{32767})
      && native::sqrdmlsh<rdm>(std::int16_t(one), std::int16_t{16384}, std::int16_t{16384}) == -8191;
  }

  __attribute__((target("fp16fml"), noinline))
  constexpr bool check_fhm(int one) noexcept {
    using H = native::simd<native::fp16,4,fhm>;
    using F = native::simd<float,2,fhm>;
    auto a = H(std::array<native::fp16,4>{native::fp16::from_bits(0x3c00),
      native::fp16::from_bits(0x3c00),native::fp16::from_bits(0x4000),native::fp16::from_bits(0x4000)});
    auto acc = F(float(one));
    return all(native::fmlal(acc, a, a), 2.f)
      && all(native::fmlal2(acc, a, a), 5.f)
      && all(native::fmlsl2(acc, a, a), -3.f);
  }

  __attribute__((target("bf16"), noinline))
  constexpr bool check_bf16(int one) noexcept {
    using B = native::simd<native::bf16,8,bf16>;
    using F = native::simd<float,4,bf16>;
    auto ones = B(native::bf16::from_bits(0x3f80));
    auto acc = F(float(one));
    return all(native::bfdot(acc, ones, ones), 3.f)
      && all(native::bfmmla(acc, ones, ones), 5.f)
      && all(native::bfmlalt(acc, ones, ones), 2.f);
  }

  __attribute__((target("complxnum"), noinline))
  constexpr bool check_fcma(int one) noexcept {
    using F = native::simd<float,2,fcma>;
    auto a = F(std::array<float,2>{float(one),2.f}), b = F(std::array<float,2>{3.f,4.f});
    return lanes(native::fcadd<fcma,90>(a, b)) == std::array<float,2>{-3.f,5.f}
      && lanes(native::fcmla<fcma,90>(a, a, b)) == std::array<float,2>{-7.f,8.f};
  }

  __attribute__((target("i8mm"), noinline))
  constexpr bool check_i8mm(int one) noexcept {
    using S = native::simd<std::int8_t,16,i8mm>;
    using U = native::simd<std::uint8_t,16,i8mm>;
    using I = native::simd<std::int32_t,4,i8mm>;
    using W = native::simd<std::uint32_t,4,i8mm>;
    return all(native::smmla(I(1), S(std::int8_t(-one)), S(1)), std::int32_t{-7})
      && all(native::ummla(W(1), U(std::uint8_t(one)), U(1)), std::uint32_t{9})
      && all(native::usdot(I(1), U(std::uint8_t(one)), S(-1)), std::int32_t{-3});
  }

  __attribute__((target("jsconv"), noinline))
  constexpr bool check_jscvt(int one) noexcept {
    return native::jcvt<jscvt>(4294967296.0 + one) == 1
      && native::jcvt<jscvt>(-1.9 * one) == -1;
  }

  static_assert(check_neon(1) && check_crc(1) && check_aes(1) && check_pmull(1));
  static_assert(check_sha2(1) && check_sha3(1) && check_sm3(1) && check_sm4(1));
  static_assert(check_dot(1) && check_rdm(1) && check_fhm(1) && check_bf16(1));
  static_assert(check_fcma(1) && check_i8mm(1) && check_jscvt(1));
}

int instruction_smoke() noexcept {
  auto cpu = native::observe_arm_capabilities();
  volatile int input = 1;
  int one = input;
  struct family {
    char const * name;
    native::isa<native::arm> requirement;
    bool (*check)(int) noexcept;
  };
  family const families[]{
    {"NEON",neon,check_neon}, {"CRC",crc,check_crc}, {"AES",aes,check_aes},
    {"PMULL",aes,check_pmull}, {"SHA-1/SHA-256",sha2,check_sha2},
    {"SHA-512/SHA-3",sha3,check_sha3}, {"SM3",sm4,check_sm3}, {"SM4",sm4,check_sm4},
    {"DotProd",dot,check_dot}, {"RDM",rdm,check_rdm}, {"FHM",fhm,check_fhm},
    {"BF16",bf16,check_bf16}, {"FCMA",fcma,check_fcma}, {"I8MM",i8mm,check_i8mm},
    {"JSCVT",jscvt,check_jscvt}
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
