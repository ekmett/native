// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>

#ifndef NATIVE_TEST_TARGET_0
#define NATIVE_TEST_TARGET_0 "sha"
#endif


extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_0_128(__m128i a, __m128i b) noexcept {
  return _mm_sha1rnds4_epu32(a, b, 0);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_1_128(__m128i a, __m128i b) noexcept {
  return _mm_sha1rnds4_epu32(a, b, 1);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_2_128(__m128i a, __m128i b) noexcept {
  return _mm_sha1rnds4_epu32(a, b, 2);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_3_128(__m128i a, __m128i b) noexcept {
  return _mm_sha1rnds4_epu32(a, b, 3);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1nexte_128(__m128i a, __m128i b) noexcept {
  return _mm_sha1nexte_epu32(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1msg1_128(__m128i a, __m128i b) noexcept {
  return _mm_sha1msg1_epu32(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1msg2_128(__m128i a, __m128i b) noexcept {
  return _mm_sha1msg2_epu32(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha256rnds2_128(__m128i a, __m128i b, __m128i c) noexcept {
  return _mm_sha256rnds2_epu32(a, b, c);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha256msg1_128(__m128i a, __m128i b) noexcept {
  return _mm_sha256msg1_epu32(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha256msg2_128(__m128i a, __m128i b) noexcept {
  return _mm_sha256msg2_epu32(a, b);
}

extern "C" native_noinline unsigned native_sha_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
