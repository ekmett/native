// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>

#ifndef NATIVE_TEST_TARGET_0
#define NATIVE_TEST_TARGET_0 "avxifma"
#endif

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_madd52lo_vex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  return _mm_madd52lo_avx_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_madd52hi_vex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  return _mm_madd52hi_avx_epu64(accumulator, a, b);
}

#ifndef NATIVE_TEST_TARGET_1
#define NATIVE_TEST_TARGET_1 "avxifma"
#endif

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_madd52lo_vex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  return _mm256_madd52lo_avx_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_madd52hi_vex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  return _mm256_madd52hi_avx_epu64(accumulator, a, b);
}

#ifndef NATIVE_TEST_TARGET_2
#define NATIVE_TEST_TARGET_2 "avx512ifma,avx512vl"
#endif

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_madd52lo_evex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  return _mm_madd52lo_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_mask_madd52lo_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  return _mm_mask_madd52lo_epu64(accumulator, bits, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_maskz_madd52lo_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  return _mm_maskz_madd52lo_epu64(bits, accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_madd52hi_evex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  return _mm_madd52hi_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_mask_madd52hi_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  return _mm_mask_madd52hi_epu64(accumulator, bits, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_maskz_madd52hi_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  return _mm_maskz_madd52hi_epu64(bits, accumulator, a, b);
}

#ifndef NATIVE_TEST_TARGET_3
#define NATIVE_TEST_TARGET_3 "avx512ifma,avx512vl"
#endif

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_madd52lo_evex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  return _mm256_madd52lo_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_mask_madd52lo_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  return _mm256_mask_madd52lo_epu64(accumulator, bits, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_maskz_madd52lo_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  return _mm256_maskz_madd52lo_epu64(bits, accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_madd52hi_evex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  return _mm256_madd52hi_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_mask_madd52hi_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  return _mm256_mask_madd52hi_epu64(accumulator, bits, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_maskz_madd52hi_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  return _mm256_maskz_madd52hi_epu64(bits, accumulator, a, b);
}

#ifndef NATIVE_TEST_TARGET_4
#define NATIVE_TEST_TARGET_4 "avx512ifma"
#endif

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_madd52lo_evex512(__m512i accumulator, __m512i a, __m512i b) noexcept {
  return _mm512_madd52lo_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_mask_madd52lo_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  return _mm512_mask_madd52lo_epu64(accumulator, bits, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_maskz_madd52lo_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  return _mm512_maskz_madd52lo_epu64(bits, accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_madd52hi_evex512(__m512i accumulator, __m512i a, __m512i b) noexcept {
  return _mm512_madd52hi_epu64(accumulator, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_mask_madd52hi_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  return _mm512_mask_madd52hi_epu64(accumulator, bits, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_maskz_madd52hi_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  return _mm512_maskz_madd52hi_epu64(bits, accumulator, a, b);
}

extern "C" native_noinline unsigned native_ifma_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
