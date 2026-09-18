// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/targets.h>
#ifdef SIMD_TARGETS_METADATA_ONLY
import simd_target_metadata;
#else
import simd;
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_TARGET_custom "neon,fullfp16,bf16"
#define SELECTED_TARGETS(X,...) X(neon,__VA_ARGS__) X(neon_fp16,__VA_ARGS__) X(custom,__VA_ARGS__)
#else
#define SIMD_TARGET_custom "avx2,fma,bmi2,f16c"
#define SELECTED_TARGETS(X,...) X(avx512,__VA_ARGS__) X(avx2,__VA_ARGS__) X(custom,__VA_ARGS__)
#endif

// The metadata-only developer probe also checks the compiler's raw-vector escape.
// The installed test uses the real hub's wrappers and two-register wide addition.
#ifdef SIMD_TARGETS_METADATA_ONLY
#define DOUBLE_STEP(tag,lanes) \
  using native=float __attribute__((ext_vector_type(lanes))); \
  for(unsigned i=0;i<32;i+=lanes) { \
    native value;__builtin_memcpy(&value,input+i,sizeof(value)); \
    value+=value;__builtin_memcpy(output+i,&value,sizeof(value)); \
  }
#else
#define DOUBLE_STEP(tag,lanes) \
  using V=simd::vec<float,lanes,tag>; \
  for(unsigned i=0;i<32;i+=2*lanes) { \
    simd::wide<V,2> value{V::load(input+i),V::load(input+i+lanes)}; \
    value=value+value; \
    value.registers[0].store(output+i);value.registers[1].store(output+i+lanes); \
  }
#endif
#define DOUBLE_BODY(name,tag) \
  __attribute__((noinline)) void name(tag,float * output,float const * input) { \
    constexpr unsigned lanes=simd::has_feature<tag,simd::feature::avx512f>?16: \
      simd::has_feature<tag,simd::feature::neon>?4:8; \
    DOUBLE_STEP(tag,lanes) \
  }
SIMD_TARGET_VARIANTS(source_kernel,SELECTED_TARGETS,DOUBLE_BODY)
#undef DOUBLE_BODY
#undef DOUBLE_STEP

// The target scope must end at the body, leaving later declarations ordinary.
extern "C" __attribute__((noinline)) float source_after_scope(float x) { return x+x; }

#ifndef SIMD_TARGETS_METADATA_ONLY
int main() {
#if defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=simd::observe_arm_capabilities();
#else
  auto cpu=simd::observe_x86_capabilities();
#endif
  float input[32],output[32]{};
  for(unsigned i=0;i<32;++i) input[i]=float(i)-17;
  auto selected=simd::with_isa(SIMD_TARGET_LIST(SELECTED_TARGETS),cpu,[&](auto tag) {
    source_kernel(tag,output,input);
  });
  if(!selected) return 77;
  for(unsigned i=0;i<32;++i) if(output[i]!=input[i]*2) return 1;
  return 0;
}
#endif
