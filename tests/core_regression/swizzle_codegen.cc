// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "support/profile.h"
using V2=test_vec<float,2>;
using V3=test_vec<float,3>;
extern "C" simd_noinline void swizzle_reverse3(float * output,float const * input) {
  auto value=simd::load_simd<V3>(input);
  V3 reversed=value.zyx;
  simd::store_simd(output,reversed);
}
extern "C" simd_noinline void swizzle_overlap3(float * output,float const * input) {
  auto value=simd::load_simd<V3>(input);
  value.xyz=value.zyx;
  simd::store_simd(output,value);
}
extern "C" simd_noinline void swizzle_repeat4(float * output,float const * input) {
  auto value=simd::load_simd<V2>(input);
  test_vec<float,4> repeated=value.xxxx;
  simd::store_simd(output,repeated);
}
extern "C" simd_noinline void swizzle_copy_xy(float * output,float const * left,float const * right) {
  auto a=simd::load_simd<V3>(left),b=simd::load_simd<V3>(right);
  a.xy=b.xy;
  simd::store_simd(output,a);
}
extern "C" simd_noinline void short_copy3(float * output,float const * input) {
  V3::loadu(input).storeu(output);
}
extern "C" simd_noinline void short_integer_copy3(std::int32_t * output,std::int32_t const * input) {
  test_vec<std::int32_t,3>::loadu(input).storeu(output);
}
