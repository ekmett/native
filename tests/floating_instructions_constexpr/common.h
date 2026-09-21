// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "../property_check.h"
#include <concepts>

namespace floating_fixture {
  template<class T> constexpr T special_bits(native_test::property_rng & rng) {
    constexpr std::array<std::uint64_t,14> half{0,0x8000,1,0x8001,0x03ff,0x0400,
      0x3c00,0xbc00,0x7bff,0x7c00,0xfc00,0x7c01,0x7e35,0xfe65};
    constexpr std::array<std::uint64_t,14> single{0,0x80000000,1,0x80000001,0x007fffff,
      0x00800000,0x3f800000,0xbf800000,0x7f7fffff,0x7f800000,0xff800000,
      0x7f800001,0x7fc12345,0xffc32145};
    constexpr std::array<std::uint64_t,14> dual{0,0x8000000000000000ULL,1,0x8000000000000001ULL,
      0x000fffffffffffffULL,0x0010000000000000ULL,0x3ff0000000000000ULL,0xbff0000000000000ULL,
      0x7fefffffffffffffULL,0x7ff0000000000000ULL,0xfff0000000000000ULL,
      0x7ff0000000000001ULL,0x7ff8123456789abcULL,0xfff8abc987654321ULL};
    if(rng.next()&1) return rng.bits<T>();
    auto index=rng.index(14);
    if constexpr(sizeof(T)==2) return T(half[index]);
    else if constexpr(sizeof(T)==4) return T(single[index]);
    else return T(dual[index]);
  }

#if defined(__aarch64__) || defined(_M_ARM64)
  struct environment {
    std::uint64_t control,status;
    environment() {
      asm volatile("mrs %0, fpcr\n\tmrs %1, fpsr" : "=r"(control),"=r"(status));
      // RNE, gradual input/output, payload NaNs, ordinary half format, EBF=0.
      std::uint64_t zero=0;
      asm volatile("msr fpcr, %0\n\tmsr fpsr, %0" :: "r"(zero) : "memory");
    }
    ~environment() {
      asm volatile("msr fpcr, %0\n\tmsr fpsr, %1" :: "r"(control),"r"(status) : "memory");
    }
  };
#endif
}
