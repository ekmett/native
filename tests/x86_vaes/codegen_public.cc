// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vaes;

#ifndef NATIVE_TEST_TARGET_0
#define NATIVE_TEST_TARGET_0 "avx,aes"
#endif

#ifndef NATIVE_TEST_TARGET_1
#define NATIVE_TEST_TARGET_1 "avx,vaes"
#endif

#ifndef NATIVE_TEST_TARGET_2
#define NATIVE_TEST_TARGET_2 "avx512f,vaes"
#endif

constexpr auto arch128 = native::target_features<native::x86>(NATIVE_TEST_TARGET_0);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesenc_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint8_t, 16, arch128>;
  return native::vaesenc<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesenclast_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint8_t, 16, arch128>;
  return native::vaesenclast<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesdec_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint8_t, 16, arch128>;
  return native::vaesdec<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_vaesdeclast_128(__m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint8_t, 16, arch128>;
  return native::vaesdeclast<arch128>(vector::from_native(a), vector::from_native(b)).to_native();
}

constexpr auto arch256 = native::target_features<native::x86>(NATIVE_TEST_TARGET_1);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesenc_256(__m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint8_t, 32, arch256>;
  return native::vaesenc<arch256>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesenclast_256(__m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint8_t, 32, arch256>;
  return native::vaesenclast<arch256>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesdec_256(__m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint8_t, 32, arch256>;
  return native::vaesdec<arch256>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_vaesdeclast_256(__m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint8_t, 32, arch256>;
  return native::vaesdeclast<arch256>(vector::from_native(a), vector::from_native(b)).to_native();
}

constexpr auto arch512 = native::target_features<native::x86>(NATIVE_TEST_TARGET_2);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesenc_512(__m512i a, __m512i b) noexcept {
  using vector = native::simd<std::uint8_t, 64, arch512>;
  return native::vaesenc<arch512>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesenclast_512(__m512i a, __m512i b) noexcept {
  using vector = native::simd<std::uint8_t, 64, arch512>;
  return native::vaesenclast<arch512>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesdec_512(__m512i a, __m512i b) noexcept {
  using vector = native::simd<std::uint8_t, 64, arch512>;
  return native::vaesdec<arch512>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m512i native_vaesdeclast_512(__m512i a, __m512i b) noexcept {
  using vector = native::simd<std::uint8_t, 64, arch512>;
  return native::vaesdeclast<arch512>(vector::from_native(a), vector::from_native(b)).to_native();
}

extern "C" native_noinline unsigned native_vaes_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
