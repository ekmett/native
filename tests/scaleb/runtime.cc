// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
import native.math;
import native.x86.features;

constexpr auto arch=native::target_features<native::x86>("avx2,fma,avx512f,avx512dq,avx512vl");
volatile std::uint32_t one_bits=0x3f800000u;
template<std::size_t N>
[[gnu::target("avx2,fma,avx512f,avx512dq,avx512vl"),gnu::noinline]]
bool check(unsigned active) {
  using V=native::simd<float,N,arch>;
  std::array<std::uint32_t,N> value{},exponent{},prior{};
  for(unsigned i=0;i<N;++i) {
    value[i]=(active>>i)&1u?one_bits:0x7f812345u;
    exponent[i]=(active>>i)&1u?0xbf000000u:0x7f800000u;
    prior[i]=0xff812345u+i;
  }
  auto a=V::load_bits(value.data()),b=V::load_bits(exponent.data()),p=V::load_bits(prior.data());
  auto mask=V::mask::from_bitset(active);
  auto merge=native::masked_scaleb(mask,p,a,b);
  auto zero=native::masked_scaleb_zero(mask,a,b);
  auto m=std::bit_cast<std::array<std::uint32_t,4>>(merge.to_native());
  auto z=std::bit_cast<std::array<std::uint32_t,4>>(zero.to_native());
  for(unsigned i=0;i<N;++i) {
    if(m[i] != ((active>>i)&1u?0x3f000000u:prior[i])) return false;
    if(z[i] != ((active>>i)&1u?0x3f000000u:0u)) return false;
  }
  for(unsigned i=N;i<4;++i) if(m[i]!=0 || z[i]!=0) return false;
  auto all=native::scaleb(V(std::bit_cast<float>(std::uint32_t(one_bits))),V(-.5f));
  auto result=std::bit_cast<std::array<std::uint32_t,4>>(all.to_native());
  for(unsigned i=0;i<4;++i) if(result[i]!=(i<N?0x3f000000u:0u)) return false;
  return true;
}
int main() {
  if(!native::classify_isa(native::observe_x86_capabilities(),arch).admitted()) {
    std::puts("AVX512F/VL short scaling runtime skipped: CPU/OS requirements unavailable.");
    return 77;
  }
  auto saved=_mm_getcsr();
  _mm_setcsr((saved|0x1f80u)&~0x3fu);
  bool valid=true;
  for(unsigned mask=0;mask<8 && valid;++mask) valid=check<2>(mask&3u) && check<3>(mask);
  valid &= (_mm_getcsr()&0x3fu)==0;
  _mm_setcsr(saved);
  if(!valid) std::puts("Short scaling changed inactive bits, padding or exception status.");
  return valid?0:1;
}
