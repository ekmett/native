// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <immintrin.h>
import native.math;
#if NATIVE_SCALEB_NEGATIVE == 1
constexpr auto a=native::avx2;
#elif NATIVE_SCALEB_NEGATIVE == 2
constexpr auto a=native::target_features<native::x86>("avx2,fma,avx512f,avx512dq");
#else
constexpr auto a=native::avx512;
#endif
#if NATIVE_SCALEB_NEGATIVE == 3
[[gnu::target("avx2,fma,no-avx512f,no-avx512vl")]]
#else
[[gnu::target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl")]]
#endif
auto probe(native::simd<float,4,a> x) {
#if NATIVE_SCALEB_NEGATIVE == 4
  return native::scaleb(_mm_setzero_ps(),_mm_setzero_ps());
#else
  return native::scaleb(x,x);
#endif
}
