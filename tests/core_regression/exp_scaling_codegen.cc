// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/attributes.h>
import native.math;

extern "C" native_noinline native_target("avx2,fma")
void exp_avx2_4(float const * input,float * output) {
  using V=native::simd<float,4,native::avx2>;
  math::exp(V::load(input)).store(output);
}
extern "C" native_noinline native_target("avx2,fma")
void exp_avx2_8(float const * input,float * output) {
  using V=native::simd<float,8,native::avx2>;
  math::exp(V::load(input)).store(output);
}
extern "C" native_noinline native_target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl")
void exp_avx512_16(float const * input,float * output) {
  using V=native::simd<float,16,native::avx512>;
  math::exp(V::load(input)).store(output);
}
