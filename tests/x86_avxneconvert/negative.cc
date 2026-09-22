// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <immintrin.h>
import native.x86.avxneconvert;
using namespace native;
#if NATIVE_NE_NEGATIVE == 1
constexpr auto requirements = target_features<x86>("avx");
#elif NATIVE_NE_NEGATIVE == 3
constexpr auto requirements = [] {
  auto a = target_features<x86>("avxneconvert");
  a.avx = false;
  return a;
}();
#else
constexpr auto requirements = target_features<x86>("avxneconvert");
#endif
constexpr unsigned lanes = (NATIVE_NE_VARIANT % 2) ? 8 : 4;
#if NATIVE_NE_NEGATIVE == 2
[[gnu::target("avx,no-avxneconvert")]]
#else
[[gnu::target("avxneconvert")]]
#endif
auto probe(bf16 const * b, fp16 const * h, simd<float, lanes, requirements> f) {
#if NATIVE_NE_NEGATIVE == 4
  // Even lanes alone need seven elements, but the instruction requires eight.
  constexpr std::array<bf16, 7> too_short{};
  constexpr auto bad = cvtneebf16_ps<requirements, 4>(too_short.data());
  return bad;
#elif NATIVE_NE_NEGATIVE == 6
  return cvtneps_bf16(_mm_setzero_ps());
#elif NATIVE_NE_NEGATIVE == 7
  return cvtneps_bf16(_mm256_setzero_ps());
#elif NATIVE_NE_NEGATIVE == 5
  return cvtneebf16_ps<requirements, 2>(b);
#elif NATIVE_NE_VARIANT < 2
  return bcstnebf16_ps<requirements, lanes>(b);
#elif NATIVE_NE_VARIANT < 4
  return bcstnesh_ps<requirements, lanes>(h);
#elif NATIVE_NE_VARIANT < 6
  return cvtneebf16_ps<requirements, lanes>(b);
#elif NATIVE_NE_VARIANT < 8
  return cvtneeph_ps<requirements, lanes>(h);
#elif NATIVE_NE_VARIANT < 10
  return cvtneobf16_ps<requirements, lanes>(b);
#elif NATIVE_NE_VARIANT < 12
  return cvtneoph_ps<requirements, lanes>(h);
#else
  return cvtneps_bf16(f);
#endif
}
