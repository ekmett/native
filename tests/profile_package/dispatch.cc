#include "support/fp_environment.h"
#include <intrin.h>
#include <cstdio>
#if defined(__AVX2__) || defined(__AVX512F__) || defined(__FMA__)
#error Baseline dispatcher inherited ISA flags
#endif
extern "C" int kernel_avx2(float const*,float*);
extern "C" int kernel_avx512(float const*,float*);
extern "C" int pair_avx2(float);
extern "C" int pair_avx512(float);
extern "C" int static_string_check();
int main() {
  if (static_string_check()) return 5;
  int r[4]; __cpuidex(r,0,0);if(r[0]<7)return 77;
  __cpuidex(r,1,0);unsigned c=unsigned(r[2]);
  constexpr unsigned required=(1u<<27)|(1u<<28)|(1u<<12);
  if((c&required)!=required)return 77;
  auto state=_xgetbv(0);if((state&0xe6)!=0xe6)return 77;
  __cpuidex(r,7,0);unsigned b=unsigned(r[1]);
  constexpr unsigned mask=(1u<<5)|(1u<<8)|(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31);
  if((b&mask)!=mask)return 77;
  simd::test::fp_scope region(simd::test::fp_mode::gradual);
  if(!region.controls_match()) return 3;
  float in[16],a[16],z[16];for(int i=0;i<16;++i)in[i]=float(i);
  if(kernel_avx2(in,a)!=8||kernel_avx512(in,z)!=16)return 1;
  for(int i=0;i<16;++i)if(a[i]!=2.f*in[i]+1.f||z[i]!=a[i])return 2;
  if(pair_avx2(in[0]) || pair_avx512(in[0])) return 4;
  std::puts("Both installed profile modules: distinct types, FMA values exact, baseline dispatch admitted.");
}
