// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.avx512cd;

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512CD__) || defined(__AVX512VL__)
#error Optional instructions must come from function targets
#endif
constexpr auto cd512 = native::target_features<native::x86>(
  "avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd");
constexpr auto cdvl = cd512;

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictd_128_plain(__m128i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, cdvl>;
  return native::vpconflictd<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictd_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, cdvl>;
  return native::mask_vpconflictd<cdvl>(
    vector_type::from_native(source),
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictd_128_zero(__mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, cdvl>;
  return native::maskz_vpconflictd<cdvl>(
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntd_128_plain(__m128i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, cdvl>;
  return native::vplzcntd<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntd_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, cdvl>;
  return native::mask_vplzcntd<cdvl>(
    vector_type::from_native(source),
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntd_128_zero(__mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, cdvl>;
  return native::maskz_vplzcntd<cdvl>(
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictq_128_plain(__m128i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, cdvl>;
  return native::vpconflictq<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictq_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, cdvl>;
  return native::mask_vpconflictq<cdvl>(
    vector_type::from_native(source),
    native::predicate<2, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictq_128_zero(__mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, cdvl>;
  return native::maskz_vpconflictq<cdvl>(
    native::predicate<2, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntq_128_plain(__m128i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, cdvl>;
  return native::vplzcntq<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntq_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, cdvl>;
  return native::mask_vplzcntq<cdvl>(
    vector_type::from_native(source),
    native::predicate<2, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntq_128_zero(__mmask8 mask, __m128i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, cdvl>;
  return native::maskz_vplzcntq<cdvl>(
    native::predicate<2, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictd_256_plain(__m256i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, cdvl>;
  return native::vpconflictd<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictd_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, cdvl>;
  return native::mask_vpconflictd<cdvl>(
    vector_type::from_native(source),
    native::predicate<8, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictd_256_zero(__mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, cdvl>;
  return native::maskz_vpconflictd<cdvl>(
    native::predicate<8, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntd_256_plain(__m256i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, cdvl>;
  return native::vplzcntd<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntd_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, cdvl>;
  return native::mask_vplzcntd<cdvl>(
    vector_type::from_native(source),
    native::predicate<8, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntd_256_zero(__mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, cdvl>;
  return native::maskz_vplzcntd<cdvl>(
    native::predicate<8, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictq_256_plain(__m256i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, cdvl>;
  return native::vpconflictq<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictq_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, cdvl>;
  return native::mask_vpconflictq<cdvl>(
    vector_type::from_native(source),
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictq_256_zero(__mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, cdvl>;
  return native::maskz_vpconflictq<cdvl>(
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntq_256_plain(__m256i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, cdvl>;
  return native::vplzcntq<cdvl>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntq_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, cdvl>;
  return native::mask_vplzcntq<cdvl>(
    vector_type::from_native(source),
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntq_256_zero(__mmask8 mask, __m256i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, cdvl>;
  return native::maskz_vplzcntq<cdvl>(
    native::predicate<4, cdvl>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictd_512_plain(__m512i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, cd512>;
  return native::vpconflictd<cd512>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictd_512_merge(__m512i source, __mmask16 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, cd512>;
  return native::mask_vpconflictd<cd512>(
    vector_type::from_native(source),
    native::predicate<16, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictd_512_zero(__mmask16 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, cd512>;
  return native::maskz_vpconflictd<cd512>(
    native::predicate<16, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntd_512_plain(__m512i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, cd512>;
  return native::vplzcntd<cd512>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntd_512_merge(__m512i source, __mmask16 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, cd512>;
  return native::mask_vplzcntd<cd512>(
    vector_type::from_native(source),
    native::predicate<16, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntd_512_zero(__mmask16 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, cd512>;
  return native::maskz_vplzcntd<cd512>(
    native::predicate<16, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictq_512_plain(__m512i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, cd512>;
  return native::vpconflictq<cd512>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictq_512_merge(__m512i source, __mmask8 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, cd512>;
  return native::mask_vpconflictq<cd512>(
    vector_type::from_native(source),
    native::predicate<8, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictq_512_zero(__mmask8 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, cd512>;
  return native::maskz_vpconflictq<cd512>(
    native::predicate<8, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntq_512_plain(__m512i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, cd512>;
  return native::vplzcntq<cd512>(vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntq_512_merge(__m512i source, __mmask8 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, cd512>;
  return native::mask_vplzcntq<cd512>(
    vector_type::from_native(source),
    native::predicate<8, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntq_512_zero(__mmask8 mask, __m512i value) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, cd512>;
  return native::maskz_vplzcntq<cd512>(
    native::predicate<8, cd512>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline unsigned long long
native_avx512cd_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
