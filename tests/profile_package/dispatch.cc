#include "support/fp_environment.h"
#include <cstdint>
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif
#include <cstdio>
#if defined(__AVX2__) || defined(__AVX512F__) || defined(__FMA__)
#error Baseline dispatcher inherited ISA flags
#endif
extern "C" int kernel_avx2(float const*,float*);
extern "C" int kernel_avx512(float const*,float*);
extern "C" int pair_avx2(float);
extern "C" int pair_avx512(float);
extern "C" int static_string_check();
namespace {
  void cpuid(int (&result)[4], unsigned leaf, unsigned subleaf) {
#if defined(_MSC_VER)
    __cpuidex(result, int(leaf), int(subleaf));
#else
    unsigned a, b, c, d;
    __cpuid_count(leaf, subleaf, a, b, c, d);
    result[0] = int(a); result[1] = int(b);
    result[2] = int(c); result[3] = int(d);
#endif
  }

  std::uint64_t vector_state() {
#if defined(_MSC_VER)
    return _xgetbv(0);
#else
    std::uint32_t low, high;
    __asm__("xgetbv" : "=a"(low), "=d"(high) : "c"(0));
    return (std::uint64_t(high) << 32) | low;
#endif
  }
}

int main() {
  if (static_string_check()) return 5;
  int r[4]; cpuid(r,0,0);if(r[0]<7)return 77;
  cpuid(r,1,0);unsigned c=unsigned(r[2]);
  constexpr unsigned required=(1u<<27)|(1u<<28)|(1u<<12);
  if((c&required)!=required)return 77;
  auto state=vector_state();if((state&0xe6)!=0xe6)return 77;
  cpuid(r,7,0);unsigned b=unsigned(r[1]);
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
