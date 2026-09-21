// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
import native;
static_assert(NATIVE_TARGET_MINIMUM<=native::avx2,
  "The code-generation fixture requires a project minimum no stronger than AVX2/FMA");
#define NATIVE_TARGET_kernel_base "avx2,fma,avx512f,avx512dq"
#define NATIVE_TARGET_kernel_vl "avx2,fma,avx512f,avx512dq,avx512vl"
#define ENTRY(name,width) \
  NATIVE_TARGET_PUSH(name) \
  extern "C" __attribute__((noinline)) void refined_codegen_kernel_##name( \
      float const * a,float const * b,float const * c,float * output) { \
    using V=native::simd<float,width,NATIVE_TARGET_ISA(name)>; \
    native::wide<V,2> x{V::loadu(a),V::loadu(a+width)}; \
    native::wide<V,2> y{V::loadu(b),V::loadu(b+width)}; \
    native::wide<V,2> z{V::loadu(c),V::loadu(c+width)}; \
    auto result=sqrt(abs(fma(x,y,z))); \
    result.registers[0].storeu(output);result.registers[1].storeu(output+width); \
  } \
  NATIVE_TARGET_POP()
ENTRY(avx2,8)
ENTRY(kernel_base,16)
ENTRY(kernel_vl,8)
#undef ENTRY
