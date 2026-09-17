// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include "engine.h"
#include <concepts>
#include <cstdio>
#include <type_traits>
#if defined(USE_AVX2)
import simd.avx2;
#endif
#if defined(USE_AVX512)
import simd.avx512;
#endif
#if defined(USE_AVX2)
static_assert(std::same_as<decltype(simd::vec{simd::avx2{},1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f}),simd::vec<float,8,simd::avx2>>);
extern "C" __declspec(dllexport) __declspec(noinline) void add8(float const*a,float const*b,float*out){
  using V=simd::vec<float,8,simd::avx2>;auto result=V::load(a)+V::load(b);result.store(out);
}
#endif
#if defined(USE_AVX512)
static_assert(std::same_as<decltype(simd::vec{simd::avx512{},1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f,12.f,13.f,14.f,15.f,16.f}),simd::vec<float,16,simd::avx512>>);
extern "C" __declspec(dllexport) __declspec(noinline) void add16(float const*a,float const*b,float*out){
  using V=simd::vec<float,16,simd::avx512>;auto result=V::load(a)+V::load(b);result.store(out);
}
#endif
#if defined(USE_AVX2)&&defined(USE_AVX512)
static_assert(!std::same_as<simd::vec<float,8,simd::avx2>,simd::vec<float,8,simd::avx512>>);
static_assert(!std::same_as<fixture::engine<simd::avx2,8>,fixture::engine<simd::avx512,16>>);
static_assert(std::same_as<fixture::engine<simd::avx2,8>::value_type,simd::vec<float,8,simd::avx2>>);
static_assert(std::same_as<fixture::engine<simd::avx512,16>::value_type,simd::vec<float,16,simd::avx512>>);
#endif
int main(){
 float a[48],b[48],out[48];for(int i=0;i<48;++i){a[i]=float(i);b[i]=float(i)*2.f;}
#if defined(USE_AVX2)
 add8(a,b,out);for(int i=0;i<8;++i)if(out[i]!=3.f*float(i))return 1;
 using A=simd::vec<float,8,simd::avx2>;fixture::wide<A,3>aw{{A::load(a),A::load(a+8),A::load(a+16)}};
 auto ac=aw+aw;ac.registers[2].store(out);if(out[0]!=32.f||simd::avx2_lanes()!=8)return 2;
#endif
#if defined(USE_AVX512)
 add16(a,b,out);for(int i=0;i<16;++i)if(out[i]!=3.f*float(i))return 3;
 using B=simd::vec<float,16,simd::avx512>;fixture::wide<B,3>bw{{B::load(b),B::load(b+16),B::load(b+32)}};
 auto bc=bw+bw;bc.registers[2].store(out);if(out[0]!=128.f||simd::avx512_lanes()!=16)return 4;
#endif
 std::puts("native module arithmetic, CTAD, wide and archive calls passed");
}
