// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "support/profile.h"
#if !SIMD_TEST_IMPORT
#include <simd/packing.h>
#endif
#include <array>
#include <bit>
#include <cstdint>
#include <cstdlib>
#include <limits>

namespace {
  void check(bool value) { if (!value) std::abort(); }

  std::uint64_t deposit_oracle(std::uint64_t value, std::uint64_t mask) {
    std::uint64_t result = 0;
    unsigned source = 0;
    for (unsigned bit = 0; bit != 64; ++bit)
      if ((mask >> bit) & 1) result |= ((value >> source++) & 1) << bit;
    return result;
  }

#if SIMD_TEST_PROFILE != 0
  template <class To, class From, unsigned Bytes> void narrow_test() {
    constexpr auto n = Bytes / sizeof(From);
    using V = simd::vec<From,n,test_arch>;
    std::array<From,n> a{}, b{};
    std::array<To,2*n> actual{};
    for (unsigned round = 0; round != 257; ++round) {
      for (unsigned i = 0; i != n; ++i) {
        a[i] = round == 0 ? std::numeric_limits<From>::max() :
          From(std::uint64_t(round) * 0x9e3779b97f4a7c15ull + i * 0xd1b54a32d192ed03ull);
        b[i] = From(~a[i] + i);
      }
      narrow_concat<To>(V::loadu(a.data()), V::loadu(b.data())).storeu(actual.data());
      for (unsigned i = 0; i != n; ++i) {
        check(actual[i] == To(a[i]));
        check(actual[n+i] == To(b[i]));
      }
    }
  }
  template <unsigned Bytes> void register_test() {
    narrow_test<std::uint8_t,std::uint16_t,Bytes>();
    narrow_test<std::uint16_t,std::uint32_t,Bytes>();
    narrow_test<std::uint32_t,std::uint64_t,Bytes>();
  }
#endif
}

int main() {
  std::uint64_t state = 0x123456789abcdef0ull;
  for (unsigned round = 0; round != 8192; ++round) {
    state ^= state << 13; state ^= state >> 7; state ^= state << 17;
    auto value = state;
    state ^= state << 13; state ^= state >> 7; state ^= state << 17;
    auto mask = round == 0 ? 0ull : round == 1 ? ~0ull : state;
    check(simd::deposit_bits<test_arch>(value,mask) == deposit_oracle(value,mask));
    for (unsigned j = 0; j != unsigned(std::popcount(mask)); ++j) {
      auto bit = simd::deposit_bits<test_arch>(std::uint64_t{1} << j,mask);
      check(std::has_single_bit(bit) && (bit & mask) != 0);
      check(unsigned(std::popcount(mask & (bit-1))) == j);
    }
  }
#if SIMD_TEST_PROFILE != 0
  register_test<16>();
#endif
#if SIMD_TEST_PROFILE >= 256
  register_test<32>();
#endif
#if SIMD_TEST_PROFILE == 512
  register_test<64>();
#endif
}
