// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
import native;
#if TEST_AVX2
constexpr auto Arch = native::avx2;
#elif TEST_AVX512
constexpr auto Arch = native::avx512;
#elif TEST_NEON_BF16
constexpr auto Arch = native::neon_bf16;
#elif TEST_NEON_FP16
constexpr auto Arch = native::neon_fp16;
#else
constexpr auto Arch = native::neon;
#endif
using V=native::simd<float,4,Arch>;
static_assert(std::same_as<decltype(native::simd<float,4,Arch>(1.f,2.f,3.f,4.f)),V>);
static_assert(std::same_as<decltype(V{}<V{}),V::mask>);
static_assert(sizeof(V)==16);
#if TEST_AVX512 && TEST_BOTH_X86
static_assert(!std::same_as<native::simd<float,4,native::avx2>,V>);
static_assert(!native::simd<float,4,native::avx2>::mask::compact);
static_assert(V::mask::compact);
#else
static_assert(!V::mask::compact);
#endif
extern "C" int omnibus_kernel(float const *input,float *output) {
  auto x=native::load_simd<V>(input);
  auto y=fma(x,V(2.f),V(1.f));
  native::store_simd(output,y);
  native::wide<V,2> packed(x,y);
  auto sum=packed+packed;
  auto const &[a,b]=sum;
  native::store_simd(output+4,a);
  native::store_simd(output+8,b);
  auto reversed=x.wzyx;
  native::store_simd(output+12,reversed);
#if TEST_AVX512 && TEST_BOTH_X86
  using A=native::simd<float,4,native::avx2>;
  auto other=fma(native::load_simd<A>(input),A(2.f),A(1.f));
  native::store_simd(output+16,other);
  return 20;
#else
  return 16;
#endif
}
