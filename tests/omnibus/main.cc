// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#include <cstdint>
#if !TEST_NEON && !TEST_NEON_FP16 && !TEST_NEON_BF16
import native.x86.features;
#elif TEST_REQUIRED_NEON_FP16 || TEST_REQUIRED_NEON_BF16
import native.arm.features;
#endif
#if (!NATIVE_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)))
#error Common consumer must not inherit AVX-512 ISA flags
#endif
extern "C" int omnibus_kernel(float const *,float *);
extern "C" void granular_kernel(float const *,float *);
int main() {
#if !TEST_NEON && !TEST_NEON_FP16 && !TEST_NEON_BF16
  constexpr auto profile =
#if TEST_REQUIRED_AVX512_BF16
    native::avx512_bf16;
#elif TEST_REQUIRED_AVX512
    native::avx512;
#else
    native::avx2;
#endif
  auto admission = native::classify_isa(native::observe_x86_capabilities(), profile);
  if (!admission.admitted()) { std::puts(admission.reason()); return 77; }
#if TEST_REQUIRED_AVX512_FP16
  auto half_admission=native::classify_isa(native::observe_x86_capabilities(),native::avx512_fp16);
  if(!half_admission.admitted()) {std::puts(half_admission.reason());return 77;}
#endif
#endif
#if TEST_REQUIRED_NEON_FP16
  {
    auto admission=native::classify_isa(native::observe_arm_capabilities(),native::neon_fp16);
    if(!admission.admitted()) { std::puts(admission.reason());return 77; }
  }
#endif
#if TEST_REQUIRED_NEON_BF16
  {
    auto admission=native::classify_isa(native::observe_arm_capabilities(),native::neon_bf16);
    if(!admission.admitted()) {std::puts(admission.reason());return 77;}
  }
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
