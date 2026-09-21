// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.bitalg;

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512BITALG__)
#error Optional instructions must come from function targets
#endif
constexpr auto full_width = native::target_features<native::x86>("avx512f,avx512bw,avx512bitalg");
constexpr auto short_width = native::target_features<native::x86>("avx512f,avx512bw,avx512bitalg,avx512vl");

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntb_128_plain(
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::vpopcntb<short_width>(
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::mask_vpopcntb<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::maskz_vpopcntb<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntw_128_plain(
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::vpopcntw<short_width>(
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpopcntw<short_width>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpopcntw<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask16 native_vpshufbitqmb_128_plain(
  __m128i value,
  __m128i control) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  using control_type = native::simd<std::uint8_t, 16, short_width>;
  return native::vpshufbitqmb<short_width>(
    vector_type::from_native(value),
    control_type::from_native(control)).to_bitset();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask16 native_vpshufbitqmb_128_zero(
  __mmask16 mask,
  __m128i value,
  __m128i control) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  using control_type = native::simd<std::uint8_t, 16, short_width>;
  return native::mask_vpshufbitqmb<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value),
    control_type::from_native(control)).to_bitset();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntb_256_plain(
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::vpopcntb<short_width>(
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::mask_vpopcntb<short_width>(
    vector_type::from_native(source),
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::maskz_vpopcntb<short_width>(
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntw_256_plain(
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::vpopcntw<short_width>(
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpopcntw<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpopcntw<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask32 native_vpshufbitqmb_256_plain(
  __m256i value,
  __m256i control) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  using control_type = native::simd<std::uint8_t, 32, short_width>;
  return native::vpshufbitqmb<short_width>(
    vector_type::from_native(value),
    control_type::from_native(control)).to_bitset();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask32 native_vpshufbitqmb_256_zero(
  __mmask32 mask,
  __m256i value,
  __m256i control) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  using control_type = native::simd<std::uint8_t, 32, short_width>;
  return native::mask_vpshufbitqmb<short_width>(
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value),
    control_type::from_native(control)).to_bitset();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntb_512_plain(
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::vpopcntb<full_width>(
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::mask_vpopcntb<full_width>(
    vector_type::from_native(source),
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::maskz_vpopcntb<full_width>(
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntw_512_plain(
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::vpopcntw<full_width>(
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpopcntw<full_width>(
    vector_type::from_native(source),
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpopcntw<full_width>(
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__mmask64 native_vpshufbitqmb_512_plain(
  __m512i value,
  __m512i control) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  using control_type = native::simd<std::uint8_t, 64, full_width>;
  return native::vpshufbitqmb<full_width>(
    vector_type::from_native(value),
    control_type::from_native(control)).to_bitset();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__mmask64 native_vpshufbitqmb_512_zero(
  __mmask64 mask,
  __m512i value,
  __m512i control) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  using control_type = native::simd<std::uint8_t, 64, full_width>;
  return native::mask_vpshufbitqmb<full_width>(
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value),
    control_type::from_native(control)).to_bitset();
}

extern "C" native_noinline unsigned long long
native_bitalg_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
