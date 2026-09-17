// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#include <cstdint>
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__x86_64__)
#include <immintrin.h>
#endif
#if !TEST_NEON
import simd.cpuid;
#endif
#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__FMA__)
#error The baseline dispatcher inherited native ISA options
#endif
extern "C" int omnibus_kernel(float const *,float *);
extern "C" void granular_kernel(float const *,float *);
int main() {
#if !TEST_NEON
  auto max=simd::cpuid(0,0).eax;
  if(max<7) return 77;
  auto c=std::uint32_t(simd::cpuid(1,0).ecx);
  constexpr auto required=(1u<<27)|(1u<<28)|(1u<<12);
  if((c&required)!=required) return 77;
#if defined(_MSC_VER)
  auto state=_xgetbv(0);
#else
  std::uint32_t low,high;
  __asm__("xgetbv" : "=a"(low),"=d"(high) : "c"(0));
  auto state=(std::uint64_t(high)<<32)|low;
#endif
  auto bits=std::uint32_t(simd::cpuid(7,0).ebx);
  constexpr auto mask=(1u<<5)|(1u<<8)
#if TEST_REQUIRED_AVX512
    |(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31)
#endif
    ;
  constexpr auto enabled=
#if TEST_REQUIRED_AVX512
    0xe6u;
#else
    0x6u;
#endif
  if((state&enabled)!=enabled || (bits&mask)!=mask) return 77;
#endif
  float input[4]={1.f,2.f,3.f,4.f},output[20]{},control[4]{};
  auto count=omnibus_kernel(input,output);
  granular_kernel(input,control);
  if(count!=16 && count!=20) return 1;
  for(int i=0;i<4;++i) {
    if(output[i]!=2.f*input[i]+1.f || control[i]!=output[i]) return 2;
    if(output[i+4]!=2.f*input[i] || output[i+8]!=2.f*output[i]) return 3;
    if(output[i+12]!=input[3-i]) return 4;
    if(count==20 && output[i+16]!=control[i]) return 5;
  }
  std::puts("Omnibus and granular imports agree; native entry admitted from baseline.");
}
