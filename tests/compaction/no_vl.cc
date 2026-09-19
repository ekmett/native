// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/vec.h>
#include <array>
#include <cstdint>
#include <cstdio>
constexpr auto partial_arch = simd::feature_closure(simd::avx2 & simd::feature::avx512f & simd::feature::avx512dq);
static_assert(!partial_arch.has(simd::feature::avx512vl));
static_assert(!partial_arch.has(simd::feature::avx512bw));
template<std::size_t N> bool check() {
  using V=simd::vec<std::uint32_t,N,partial_arch>;
  std::array<std::uint32_t,N> input{},out{},expected{};
  for(std::size_t lane=0;lane<N;++lane) input[lane]=std::uint32_t(lane+100);
  auto value=V::load(input.data());
  for(std::uint32_t bits=0;bits<(1u<<N);++bits) {
    auto mask=V::mask::from_bitset(bits);expected.fill(77);std::size_t count=0;
    for(std::size_t lane=0;lane<N;++lane) if(bits&(1u<<lane)) expected[count++]=input[lane];
    auto packed=simd::compress(mask,value,77u);packed.value.store(out.data());
    if(packed.count!=count || out!=expected) return false;
    simd::expand(mask,packed.value,V(99u)).store(out.data());
    for(std::size_t lane=0;lane<N;++lane) if(out[lane]!=((bits&(1u<<lane))?input[lane]:99u)) return false;
    out.fill(55);auto written=simd::compress_store(out.data(),3,mask,value);
    if(written!=(count<3?count:3)) return false;
    for(std::size_t lane=0;lane<N;++lane) if(out[lane]!=(lane<written?expected[lane]:55)) return false;
  }
  return true;
}
int compaction_entry(int,char**) {
  // The tag requests F/DQ explicitly; smaller registers cannot assume VL.
  if(!check<4>() || !check<8>() || !check<16>()) return 1;
  std::puts("Explicit F/DQ without VL/BW compaction passed");return 0;
}
