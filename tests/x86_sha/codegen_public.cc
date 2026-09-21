// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.sha;

#ifndef NATIVE_TEST_TARGET_0
#define NATIVE_TEST_TARGET_0 "sha"
#endif

constexpr auto arch128 = native::target_features<native::x86>(NATIVE_TEST_TARGET_0);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_0_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha1rnds4<arch128, 0>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_1_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha1rnds4<arch128, 1>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_2_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha1rnds4<arch128, 2>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1rnds4_3_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha1rnds4<arch128, 3>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1nexte_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha1nexte<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1msg1_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha1msg1<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha1msg2_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha1msg2<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha256rnds2_128(__m128i a, __m128i b, __m128i c) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha256rnds2<arch128>(vector::from_native(a), vector::from_native(b), vector::from_native(c)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha256msg1_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha256msg1<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_sha256msg2_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint32_t, 4, arch128>;
  return native::sha256msg2<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline unsigned native_sha_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
