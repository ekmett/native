// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include "engine.h"
#include <concepts>
#include <cstdio>
#include <type_traits>
#if defined(USE_AVX2)
import native.avx2;
#endif
#if defined(USE_AVX512)
import native.avx512;
#endif
#if defined(USE_AVX2)
static_assert(std::same_as<decltype(native::vec<float,8,native::avx2>{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f}),native::vec<float,8,native::avx2>>);
extern "C" __declspec(dllexport) __declspec(noinline) void add8(float const*a,float const*b,float*out){
  using V=native::vec<float,8,native::avx2>;auto result=V::load(a)+V::load(b);result.store(out);
}
#endif
#if defined(USE_AVX512)
static_assert(std::same_as<decltype(native::vec<float,16,native::avx512>{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f,12.f,13.f,14.f,15.f,16.f}),native::vec<float,16,native::avx512>>);
extern "C" __declspec(dllexport) __declspec(noinline) void add16(float const*a,float const*b,float*out){
  using V=native::vec<float,16,native::avx512>;auto result=V::load(a)+V::load(b);result.store(out);
}
#endif
#if defined(USE_AVX2)&&defined(USE_AVX512)
static_assert(!std::same_as<native::vec<float,8,native::avx2>,native::vec<float,8,native::avx512>>);
static_assert(!std::same_as<fixture::engine<native::avx2,8>,fixture::engine<native::avx512,16>>);
static_assert(std::same_as<fixture::engine<native::avx2,8>::value_type,native::vec<float,8,native::avx2>>);
static_assert(std::same_as<fixture::engine<native::avx512,16>::value_type,native::vec<float,16,native::avx512>>);
#endif
int main(){
 float a[48],b[48],out[48];for(int i=0;i<48;++i){a[i]=float(i);b[i]=float(i)*2.f;}
#if defined(USE_AVX2)
 add8(a,b,out);for(int i=0;i<8;++i)if(out[i]!=3.f*float(i))return 1;
 using A=native::vec<float,8,native::avx2>;fixture::wide<A,3>aw{{A::load(a),A::load(a+8),A::load(a+16)}};
 auto ac=aw+aw;ac.registers[2].store(out);if(out[0]!=32.f||native::avx2_lanes()!=8)return 2;
#endif
#if defined(USE_AVX512)
 add16(a,b,out);for(int i=0;i<16;++i)if(out[i]!=3.f*float(i))return 3;
 using B=native::vec<float,16,native::avx512>;fixture::wide<B,3>bw{{B::load(b),B::load(b+16),B::load(b+32)}};
 auto bc=bw+bw;bc.registers[2].store(out);if(out[0]!=128.f||native::avx512_lanes()!=16)return 4;
#endif
 std::puts("native module arithmetic, CTAD, wide and archive calls passed");
}
