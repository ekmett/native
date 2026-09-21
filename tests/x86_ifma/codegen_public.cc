// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.ifma;

#ifndef NATIVE_TEST_TARGET_0
#define NATIVE_TEST_TARGET_0 "avxifma"
#endif
constexpr auto arch_vex128 = native::target_features<native::x86>(NATIVE_TEST_TARGET_0);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_madd52lo_vex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_vex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52lo<arch_vex128>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_0)
__m128i native_madd52hi_vex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_vex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52hi<arch_vex128>(sum, x, y).to_native();
}

#ifndef NATIVE_TEST_TARGET_1
#define NATIVE_TEST_TARGET_1 "avxifma"
#endif
constexpr auto arch_vex256 = native::target_features<native::x86>(NATIVE_TEST_TARGET_1);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_madd52lo_vex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_vex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52lo<arch_vex256>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_1)
__m256i native_madd52hi_vex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_vex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52hi<arch_vex256>(sum, x, y).to_native();
}

#ifndef NATIVE_TEST_TARGET_2
#define NATIVE_TEST_TARGET_2 "avx512ifma,avx512vl"
#endif
constexpr auto arch_evex128 = native::target_features<native::x86>(NATIVE_TEST_TARGET_2);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_madd52lo_evex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_evex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52lo<arch_evex128>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_mask_madd52lo_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_evex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<2, arch_evex128>::from_bitset(bits);
  return native::mask_madd52lo<arch_evex128>(sum, mask, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_maskz_madd52lo_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_evex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<2, arch_evex128>::from_bitset(bits);
  return native::maskz_madd52lo<arch_evex128>(mask, sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_madd52hi_evex128(__m128i accumulator, __m128i a, __m128i b) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_evex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52hi<arch_evex128>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_mask_madd52hi_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_evex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<2, arch_evex128>::from_bitset(bits);
  return native::mask_madd52hi<arch_evex128>(sum, mask, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_2)
__m128i native_maskz_madd52hi_evex128(__m128i accumulator, __m128i a, __m128i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 2, arch_evex128>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<2, arch_evex128>::from_bitset(bits);
  return native::maskz_madd52hi<arch_evex128>(mask, sum, x, y).to_native();
}

#ifndef NATIVE_TEST_TARGET_3
#define NATIVE_TEST_TARGET_3 "avx512ifma,avx512vl"
#endif
constexpr auto arch_evex256 = native::target_features<native::x86>(NATIVE_TEST_TARGET_3);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_madd52lo_evex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_evex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52lo<arch_evex256>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_mask_madd52lo_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_evex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<4, arch_evex256>::from_bitset(bits);
  return native::mask_madd52lo<arch_evex256>(sum, mask, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_maskz_madd52lo_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_evex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<4, arch_evex256>::from_bitset(bits);
  return native::maskz_madd52lo<arch_evex256>(mask, sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_madd52hi_evex256(__m256i accumulator, __m256i a, __m256i b) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_evex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52hi<arch_evex256>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_mask_madd52hi_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_evex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<4, arch_evex256>::from_bitset(bits);
  return native::mask_madd52hi<arch_evex256>(sum, mask, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_3)
__m256i native_maskz_madd52hi_evex256(__m256i accumulator, __m256i a, __m256i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 4, arch_evex256>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<4, arch_evex256>::from_bitset(bits);
  return native::maskz_madd52hi<arch_evex256>(mask, sum, x, y).to_native();
}

#ifndef NATIVE_TEST_TARGET_4
#define NATIVE_TEST_TARGET_4 "avx512ifma"
#endif
constexpr auto arch_evex512 = native::target_features<native::x86>(NATIVE_TEST_TARGET_4);

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_madd52lo_evex512(__m512i accumulator, __m512i a, __m512i b) noexcept {
  using vector = native::simd<std::uint64_t, 8, arch_evex512>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52lo<arch_evex512>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_mask_madd52lo_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 8, arch_evex512>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<8, arch_evex512>::from_bitset(bits);
  return native::mask_madd52lo<arch_evex512>(sum, mask, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_maskz_madd52lo_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 8, arch_evex512>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<8, arch_evex512>::from_bitset(bits);
  return native::maskz_madd52lo<arch_evex512>(mask, sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_madd52hi_evex512(__m512i accumulator, __m512i a, __m512i b) noexcept {
  using vector = native::simd<std::uint64_t, 8, arch_evex512>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  return native::madd52hi<arch_evex512>(sum, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_mask_madd52hi_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 8, arch_evex512>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<8, arch_evex512>::from_bitset(bits);
  return native::mask_madd52hi<arch_evex512>(sum, mask, x, y).to_native();
}

extern "C" native_noinline native_target(NATIVE_TEST_TARGET_4)
__m512i native_maskz_madd52hi_evex512(__m512i accumulator, __m512i a, __m512i b, __mmask8 bits) noexcept {
  using vector = native::simd<std::uint64_t, 8, arch_evex512>;
  auto sum = vector::from_native(accumulator);
  auto x = vector::from_native(a);
  auto y = vector::from_native(b);
  auto mask = native::predicate<8, arch_evex512>::from_bitset(bits);
  return native::maskz_madd52hi<arch_evex512>(mask, sum, x, y).to_native();
}

extern "C" native_noinline unsigned native_ifma_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
