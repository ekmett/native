// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/targets.h>
import simd;
static_assert((SIMD_TARGET_MINIMUM&~simd::avx2::features)==0,
  "The code-generation fixture requires a project minimum no stronger than AVX2/FMA/BMI2");
#define SIMD_TARGET_kernel_base "avx2,fma,bmi2,avx512f,avx512dq"
#define SIMD_TARGET_kernel_vl "avx2,fma,bmi2,avx512f,avx512dq,avx512vl"
#define ENTRY(name,width) \
  SIMD_TARGET_PUSH(name) \
  extern "C" __attribute__((noinline)) void refined_codegen_kernel_##name( \
      float const * a,float const * b,float const * c,float * output) { \
    using V=simd::vec<float,width,SIMD_TARGET_TYPE(name)>; \
    simd::wide<V,2> x{V::loadu(a),V::loadu(a+width)}; \
    simd::wide<V,2> y{V::loadu(b),V::loadu(b+width)}; \
    simd::wide<V,2> z{V::loadu(c),V::loadu(c+width)}; \
    auto result=sqrt(abs(fma(x,y,z))); \
    result.registers[0].storeu(output);result.registers[1].storeu(output+width); \
  } \
  SIMD_TARGET_POP()
ENTRY(avx2,8)
ENTRY(kernel_base,16)
ENTRY(kernel_vl,8)
#undef ENTRY
