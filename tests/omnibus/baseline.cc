// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstdio>
#include <type_traits>
import simd.scalar;
import simd.numerics;
import simd.wide;
#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__FMA__)
#error The baseline omnibus consumer inherited native ISA options
#endif
static_assert(sizeof(simd::vec<float,1,simd::scalar>)==sizeof(float));
static_assert(std::same_as<decltype(simd::wide{1,2}),simd::wide<int,2>>);
int main() {
  simd::vec<float,1,simd::scalar> x(2.f);
  float value=0.f;
  simd::store_simd(&value,x+x);
  if(value!=4.f) return 1;
  simd::fp16 half(1.5f);
  if(float(half)!=1.5f) return 2;
  std::puts("Baseline archive consumer: scalar and numerics available without AVX flags.");
}
