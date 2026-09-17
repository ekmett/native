// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include "support/fp_environment.h"
#include "support/profile.h"

static void require(bool condition,char const * message) {
  if(!condition){std::fprintf(stderr,"%s\n",message);std::abort();}
}
constexpr std::array<std::uint32_t,20> bank{
  0u,0x80000000u,1u,0x80000001u,0x007fffffu,0x807fffffu,
  0x00800000u,0x80800000u,0x00800001u,0x80800001u,
  0x3f800000u,0xbf800000u,0x7f7fffffu,0xff7fffffu,
  0x7f800000u,0xff800000u,0x7fc12345u,0xffc12345u,0x7f800001u,0xff800001u};
constexpr std::uint32_t flushed(std::uint32_t word) {
  return (word&0x7fffffffu)<0x00800000u ? word&0x80000000u : word;
}
template<std::size_t N> void check_short() {
  using F=test_vec<float,N>;
  using U=typename F::bits_type;
  static_assert(std::same_as<decltype(simd::flush_to_zero(F{})),F>);
  static_assert(noexcept(simd::flush_to_zero(F{})));
  for(std::size_t start=0;start<bank.size();++start) {
    std::array<std::uint32_t,N> words{},actual{};
    for(std::size_t lane=0;lane<N;++lane)words[lane]=bank[(start+lane)%bank.size()];
    F input=F::from_bits(U::load(words.data()));
    simd::flush_to_zero(input).bits().store(actual.data());
    for(std::size_t lane=0;lane<N;++lane)require(actual[lane]==flushed(words[lane]),"short scalar-register flush changed a word");
    auto output=simd::flush_to_zero(std::array{input,input});
    for(auto result:output) {
      result.bits().store(actual.data());
      for(std::size_t lane=0;lane<N;++lane)require(actual[lane]==flushed(words[lane]),"short array flush changed a word");
    }
  }
  auto empty=simd::flush_to_zero(std::array<F,0>{});
  static_assert(std::same_as<decltype(empty),std::array<F,0>>);
}
int main() {
  auto saved=simd::test::read_fp_state();
  for(auto mode:{simd::test::fp_mode::gradual,simd::test::fp_mode::flush}) {
    simd::test::fp_scope scope(mode);
    auto before=simd::test::read_fp_state();
    check_short<2>();check_short<3>();
    require(simd::test::read_fp_state()==before,"integer short flush modified FP state");
  }
  require(simd::test::read_fp_state()==saved,"short flush fixture failed to restore FP state");
}
