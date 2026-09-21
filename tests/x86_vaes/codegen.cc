// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>

#ifndef NATIVE_TEST_TARGET_0
#define NATIVE_TEST_TARGET_0 "avx,aes"
#endif

#ifndef NATIVE_TEST_TARGET_1
#define NATIVE_TEST_TARGET_1 "avx,vaes"
#endif

#ifndef NATIVE_TEST_TARGET_2
#define NATIVE_TEST_TARGET_2 "avx512f,vaes"
#endif


extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesenc_128(__m128i a, __m128i b) noexcept {
  return _mm_aesenc_si128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesenclast_128(__m128i a, __m128i b) noexcept {
  return _mm_aesenclast_si128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesdec_128(__m128i a, __m128i b) noexcept {
  return _mm_aesdec_si128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesdeclast_128(__m128i a, __m128i b) noexcept {
  return _mm_aesdeclast_si128(a, b);
}


extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesenc_256(__m256i a, __m256i b) noexcept {
  return _mm256_aesenc_epi128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesenclast_256(__m256i a, __m256i b) noexcept {
  return _mm256_aesenclast_epi128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesdec_256(__m256i a, __m256i b) noexcept {
  return _mm256_aesdec_epi128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesdeclast_256(__m256i a, __m256i b) noexcept {
  return _mm256_aesdeclast_epi128(a, b);
}


extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesenc_512(__m512i a, __m512i b) noexcept {
  return _mm512_aesenc_epi128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesenclast_512(__m512i a, __m512i b) noexcept {
  return _mm512_aesenclast_epi128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesdec_512(__m512i a, __m512i b) noexcept {
  return _mm512_aesdec_epi128(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesdeclast_512(__m512i a, __m512i b) noexcept {
  return _mm512_aesdeclast_epi128(a, b);
}

extern "C" native_noinline unsigned native_vaes_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
