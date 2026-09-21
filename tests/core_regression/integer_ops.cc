// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "support/profile.h"
#if !NATIVE_TEST_IMPORT
#include <native/integer.h>
#endif
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

namespace {
  void check(bool value) { if (!value) std::abort(); }

  template <class T, std::size_t N> void test() {
    using V = native::vec<T,N,test_arch>;
    std::array<T,N> source{}, actual{};
    for (unsigned round = 0; round < 259; ++round) {
      for (unsigned i = 0; i < N; ++i)
        source[i] = round == 0 ? 0 : round == 1 ? std::numeric_limits<T>::max() :
          T(std::uint64_t(round) * 0x9e3779b97f4a7c15ull + std::uint64_t(i) * 0xd1b54a32d192ed03ull);
      auto value = V::loadu(source.data());
      popcount(value).storeu(actual.data());
      for (unsigned i = 0; i < N; ++i) check(actual[i] == T(std::popcount(source[i])));
      using signed_type = std::make_signed_t<T>;
      auto signed_value = reinterpret_bits<signed_type>(value);
      reinterpret_bits<T>(signed_value).storeu(actual.data());
      check(actual == source);
      if constexpr (N > 1) {
        std::array<std::uint8_t,sizeof(T)*N> bytes{};
        reinterpret_bits<std::uint8_t>(value).storeu(bytes.data());
        check(std::memcmp(bytes.data(),source.data(),bytes.size()) == 0);
        if constexpr (sizeof(T) <= 4) {
          using W = decltype(pairwise_add_widened(value));
          std::array<typename W::value_type,N/2> pairs{};
          pairwise_add_widened(value).storeu(pairs.data());
          for (unsigned i = 0; i < N/2; ++i)
            check(pairs[i] == std::uint64_t(source[2*i]) + source[2*i+1]);
        }
      }
      if constexpr (sizeof(T) <= 4) {
        std::uint64_t expected = 0;
        for (auto element : source) expected += element;
        check(reduce_add_widened(value) == expected);
        // Every prefix, with the other lanes zero, exercises reduction tails
        // and UINT32_MAX lanes whose complete sum exceeds 32 bits.
        for (unsigned count = 0; count <= N; ++count) {
          auto prefix = V::load_partial(source.data(),count);
          std::uint64_t sum = 0;
          for (unsigned i = 0; i < count; ++i) sum += source[i];
          check(reduce_add_widened(prefix) == sum);
        }
      }
    }
  }

  template <unsigned Bytes> void register_test() {
    test<std::uint8_t,Bytes>();
    test<std::uint16_t,Bytes/2>();
    test<std::uint32_t,Bytes/4>();
    test<std::uint64_t,Bytes/8>();
  }
}

int main() {
  test<std::uint8_t,1>();
  test<std::uint16_t,1>();
  test<std::uint32_t,1>();
  test<std::uint64_t,1>();
#if NATIVE_TEST_PROFILE != 0
  register_test<16>();
#endif
#if NATIVE_TEST_PROFILE >= 256
  register_test<32>();
#endif
#if NATIVE_TEST_PROFILE == 512
  register_test<64>();
#endif
  std::puts("Integer bit order, lane populations, pair sums and widened reductions passed");
}
