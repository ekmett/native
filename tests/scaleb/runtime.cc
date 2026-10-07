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
  constexpr auto storage_lanes=sizeof(typename V::native_type)/sizeof(float);
  using words=std::array<std::uint32_t,storage_lanes>;
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
  auto m=__builtin_bit_cast(words,merge.to_native());
  auto z=__builtin_bit_cast(words,zero.to_native());
  for(unsigned i=0;i<N;++i) {
    if(m[i] != ((active>>i)&1u?0x3f000000u:prior[i])) return false;
    if(z[i] != ((active>>i)&1u?0x3f000000u:0u)) return false;
  }
  for(unsigned i=N;i<storage_lanes;++i) if(m[i]!=0 || z[i]!=0) return false;
  auto all=native::scaleb(V(std::bit_cast<float>(std::uint32_t(one_bits))),V(-.5f));
  auto result=__builtin_bit_cast(words,all.to_native());
  for(unsigned i=0;i<storage_lanes;++i) if(result[i]!=(i<N?0x3f000000u:0u)) return false;
  return true;
}

[[gnu::target("avx2,fma,avx512f,avx512dq,avx512vl"),gnu::noinline]]
bool check_rounding(unsigned mode) {
  using V = native::simd<float,4,arch>;
  using words = std::array<std::uint32_t,4>;
  // A halfway subnormal and overflow, with both signs. MXCSR rounding order:
  // nearest-even, toward negative infinity, toward positive infinity, zero.
  words const input{0x00800001u,0x80800001u,0x7f7fffffu,0xff7fffffu};
  std::array<float,4> const exponents{-1.f,-1.f,1.f,1.f};
  auto result = native::scaleb(V::load_bits(input.data()),V::load(exponents.data()));
  words const expected{
    mode == 2 ? 0x00400001u : 0x00400000u,
    mode == 1 ? 0x80400001u : 0x80400000u,
    mode == 1 || mode == 3 ? 0x7f7fffffu : 0x7f800000u,
    mode == 2 || mode == 3 ? 0xff7fffffu : 0xff800000u
  };
  return __builtin_bit_cast(words,result.to_native()) == expected;
}

int main() {
  if(!native::classify_isa(native::observe_x86_capabilities(),arch).admitted()) {
    std::puts("AVX512F/VL scaling runtime skipped: CPU/OS requirements unavailable.");
    return 77;
  }
  auto saved=_mm_getcsr();
  // Mask traps, clear status, disable DAZ/FTZ, and start at nearest-even.
  auto const environment = (saved|0x1f80u)&~0xe07fu;
  _mm_setcsr(environment);
  bool valid=true;
  // Empty, alternating and full masks exercise inactive sNaNs alongside active
  // arithmetic. Sharing an unmasked calculation is allowed; inactive result
  // bits and logical padding still have to be preserved.
  for(unsigned mask : {0u,0x5555u,0xaaaau,0xffffu}) {
    valid &= check<1>(mask&1u) && check<2>(mask&3u) && check<3>(mask&7u) &&
      check<4>(mask&15u) && check<8>(mask&255u) && check<16>(mask);
  }
  for(unsigned mode=0;mode<4;++mode) {
    _mm_setcsr(environment|(mode<<13));
    valid &= check_rounding(mode);
  }
  _mm_setcsr(saved);
  if(!valid) std::puts("Scaling changed inactive bits, padding or rounding.");
  return valid?0:1;
}
