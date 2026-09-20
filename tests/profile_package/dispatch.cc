#include "support/fp_environment.h"
#include <cstdint>
import native.x86.features;
#include <cstdio>
#if (!NATIVE_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)))
#error Common consumer must not inherit AVX-512 ISA flags
#endif
extern "C" int kernel_avx2(float const*,float*);
extern "C" int kernel_avx512(float const*,float*);
extern "C" int pair_avx2(float);
extern "C" int pair_avx512(float);
extern "C" int static_string_check();
int main() {
  if (static_string_check()) return 5;
  auto admission = native::classify_isa(
    native::observe_x86_capabilities(), native::avx512);
  if (!admission.admitted()) { std::puts(admission.reason()); return 77; }
  native::test::fp_scope region(native::test::fp_mode::gradual);
  if(!region.controls_match()) return 3;
  float in[16],a[16],z[16];for(int i=0;i<16;++i)in[i]=float(i);
  if(kernel_avx2(in,a)!=8||kernel_avx512(in,z)!=16)return 1;
  for(int i=0;i<16;++i)if(a[i]!=2.f*in[i]+1.f||z[i]!=a[i])return 2;
  if(pair_avx2(in[0]) || pair_avx512(in[0])) return 4;
  std::puts("Both native profiles from the installed hub: distinct types, FMA values exact, baseline dispatch admitted.");
}
