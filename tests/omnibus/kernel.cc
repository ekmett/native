// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
import simd;
#if TEST_AVX2
using Arch=simd::avx2;
#elif TEST_AVX512
using Arch=simd::avx512;
#elif TEST_NEON_FP16
using Arch=simd::neon_fp16;
#else
using Arch=simd::neon;
#endif
using V=simd::vec<float,4,Arch>;
static_assert(std::same_as<decltype(simd::vec(Arch{},1.f,2.f,3.f,4.f)),V>);
static_assert(std::same_as<decltype(V{}<V{}),V::mask>);
static_assert(sizeof(V)==16);
#if TEST_AVX512 && TEST_BOTH_X86
static_assert(!std::same_as<simd::vec<float,4,simd::avx2>,V>);
static_assert(!simd::vec<float,4,simd::avx2>::mask::compact);
static_assert(V::mask::compact);
#else
static_assert(!V::mask::compact);
#endif
extern "C" int omnibus_kernel(float const *input,float *output) {
  auto x=simd::load_simd<V>(input);
  auto y=fma(x,V(2.f),V(1.f));
  simd::store_simd(output,y);
  simd::wide<V,2> packed(x,y);
  auto sum=packed+packed;
  auto const &[a,b]=sum;
  simd::store_simd(output+4,a);
  simd::store_simd(output+8,b);
  auto reversed=x.wzyx;
  simd::store_simd(output+12,reversed);
#if TEST_AVX512 && TEST_BOTH_X86
  using A=simd::vec<float,4,simd::avx2>;
  auto other=fma(simd::load_simd<A>(input),A(2.f),A(1.f));
  simd::store_simd(output+16,other);
  return 20;
#else
  return 16;
#endif
}
