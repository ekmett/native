#include "support/profile.h"
import simd.wide;
#include <array>
#include <cstdio>
#include <cstdlib>
#include <type_traits>
using namespace test_simd;
static void require(bool b,char const * why) { if(!b){std::fprintf(stderr,"%s\n",why);std::exit(1);} }
template<std::size_t N> void integer_bridge() {
  using F=test_vec<float,N>;using U=test_vec<uint32_t,N>;
  static_assert(std::same_as<typename F::bits_type,U>);
  std::array<uint32_t,N> words{};
  for(std::size_t i=0;i<N;++i)words[i]=(i%2)?0xffc12345u:0x80000000u;
  auto u=U::load(words.data());auto f=F::from_bits(u);
  std::array<uint32_t,N> actual{};f.bits().store(actual.data());
  require(words==actual,"real integer float bits roundtrip");
  for(std::size_t n=0;n<=N;++n) {
    auto v=F::load_bits_partial(words.data(),n,0x12345678u);v.bits().store(actual.data());
    for(std::size_t i=0;i<N;++i)require(actual[i]==(i<n?words[i]:0x12345678u),"real integer partial fill");
  }
}

int main() {
  integer_bridge<1>();
#if defined(__AVX2__)
  integer_bridge<4>();integer_bridge<8>();
#endif
#if defined(__AVX512F__)
  integer_bridge<16>();
#endif
#if defined(__ARM_NEON)
  integer_bridge<4>();
#endif
  return 0;
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Checks exact floating and real integer SIMD bit bridges and partial fills.
 */
