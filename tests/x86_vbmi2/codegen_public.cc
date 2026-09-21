// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vbmi2;

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512VBMI2__)
#error Optional instructions must come from function targets
#endif
constexpr auto full_width = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi2");
constexpr auto short_width = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi2,avx512vl");

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpcompressb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::mask_vpcompressb<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpcompressb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::maskz_vpcompressb<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
void native_vpcompressb_128_store(
  std::uint8_t * destination,
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  native::mask_vpcompressb<short_width>(
    destination,
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::mask_vpexpandb<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::maskz_vpexpandb<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandb_128_load_merge(
  __m128i source,
  __mmask16 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::mask_vpexpandb<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandb_128_load_zero(
  __mmask16 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, short_width>;
  return native::maskz_vpexpandb<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpcompressw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpcompressw<short_width>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpcompressw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpcompressw<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
void native_vpcompressw_128_store(
  std::uint16_t * destination,
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  native::mask_vpcompressw<short_width>(
    destination,
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpexpandw<short_width>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpexpandw<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandw_128_load_merge(
  __m128i source,
  __mmask8 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpexpandw<short_width>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpexpandw_128_load_zero(
  __mmask8 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpexpandw<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldw_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::vpshldw<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpshldw<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpshldw<short_width, 7>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvw_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::vpshldvw<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvw_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpshldvw<short_width>(
    vector_type::from_native(a),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpshldvw<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdw_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::vpshrdw<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpshrdw<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpshrdw<short_width, 7>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvw_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::vpshrdvw<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvw_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::mask_vpshrdvw<short_width>(
    vector_type::from_native(a),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, short_width>;
  return native::maskz_vpshrdvw<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldd_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::vpshldd<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldd_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::mask_vpshldd<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::maskz_vpshldd<short_width, 7>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvd_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::vpshldvd<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvd_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::mask_vpshldvd<short_width>(
    vector_type::from_native(a),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::maskz_vpshldvd<short_width>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdd_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::vpshrdd<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdd_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::mask_vpshrdd<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::maskz_vpshrdd<short_width, 7>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvd_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::vpshrdvd<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvd_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::mask_vpshrdvd<short_width>(
    vector_type::from_native(a),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, short_width>;
  return native::maskz_vpshrdvd<short_width>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldq_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::vpshldq<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldq_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::mask_vpshldq<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::maskz_vpshldq<short_width, 7>(
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvq_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::vpshldvq<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvq_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::mask_vpshldvq<short_width>(
    vector_type::from_native(a),
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshldvq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::maskz_vpshldvq<short_width>(
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdq_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::vpshrdq<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdq_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::mask_vpshrdq<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::maskz_vpshrdq<short_width, 7>(
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvq_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::vpshrdvq<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvq_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::mask_vpshrdvq<short_width>(
    vector_type::from_native(a),
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m128i native_vpshrdvq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, short_width>;
  return native::maskz_vpshrdvq<short_width>(
    native::predicate<2, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpcompressb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::mask_vpcompressb<short_width>(
    vector_type::from_native(source),
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpcompressb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::maskz_vpcompressb<short_width>(
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
void native_vpcompressb_256_store(
  std::uint8_t * destination,
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  native::mask_vpcompressb<short_width>(
    destination,
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::mask_vpexpandb<short_width>(
    vector_type::from_native(source),
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::maskz_vpexpandb<short_width>(
    native::predicate<32, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandb_256_load_merge(
  __m256i source,
  __mmask32 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::mask_vpexpandb<short_width>(
    vector_type::from_native(source),
    native::predicate<32, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandb_256_load_zero(
  __mmask32 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, short_width>;
  return native::maskz_vpexpandb<short_width>(
    native::predicate<32, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpcompressw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpcompressw<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpcompressw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpcompressw<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
void native_vpcompressw_256_store(
  std::uint16_t * destination,
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  native::mask_vpcompressw<short_width>(
    destination,
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpexpandw<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpexpandw<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandw_256_load_merge(
  __m256i source,
  __mmask16 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpexpandw<short_width>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpexpandw_256_load_zero(
  __mmask16 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpexpandw<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldw_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::vpshldw<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpshldw<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpshldw<short_width, 7>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvw_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::vpshldvw<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvw_256_merge(
  __m256i a,
  __mmask16 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpshldvw<short_width>(
    vector_type::from_native(a),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpshldvw<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdw_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::vpshrdw<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpshrdw<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpshrdw<short_width, 7>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvw_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::vpshrdvw<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvw_256_merge(
  __m256i a,
  __mmask16 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::mask_vpshrdvw<short_width>(
    vector_type::from_native(a),
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, short_width>;
  return native::maskz_vpshrdvw<short_width>(
    native::predicate<16, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldd_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::vpshldd<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldd_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::mask_vpshldd<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::maskz_vpshldd<short_width, 7>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvd_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::vpshldvd<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvd_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::mask_vpshldvd<short_width>(
    vector_type::from_native(a),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::maskz_vpshldvd<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdd_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::vpshrdd<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdd_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::mask_vpshrdd<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::maskz_vpshrdd<short_width, 7>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvd_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::vpshrdvd<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvd_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::mask_vpshrdvd<short_width>(
    vector_type::from_native(a),
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, short_width>;
  return native::maskz_vpshrdvd<short_width>(
    native::predicate<8, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldq_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::vpshldq<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldq_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::mask_vpshldq<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::maskz_vpshldq<short_width, 7>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvq_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::vpshldvq<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvq_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::mask_vpshldvq<short_width>(
    vector_type::from_native(a),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshldvq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::maskz_vpshldvq<short_width>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdq_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::vpshrdq<short_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdq_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::mask_vpshrdq<short_width, 7>(
    vector_type::from_native(source),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::maskz_vpshrdq<short_width, 7>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvq_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::vpshrdvq<short_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvq_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::mask_vpshrdvq<short_width>(
    vector_type::from_native(a),
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
__m256i native_vpshrdvq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, short_width>;
  return native::maskz_vpshrdvq<short_width>(
    native::predicate<4, short_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpcompressb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::mask_vpcompressb<full_width>(
    vector_type::from_native(source),
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpcompressb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::maskz_vpcompressb<full_width>(
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
void native_vpcompressb_512_store(
  std::uint8_t * destination,
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  native::mask_vpcompressb<full_width>(
    destination,
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::mask_vpexpandb<full_width>(
    vector_type::from_native(source),
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::maskz_vpexpandb<full_width>(
    native::predicate<64, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandb_512_load_merge(
  __m512i source,
  __mmask64 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::mask_vpexpandb<full_width>(
    vector_type::from_native(source),
    native::predicate<64, full_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandb_512_load_zero(
  __mmask64 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, full_width>;
  return native::maskz_vpexpandb<full_width>(
    native::predicate<64, full_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpcompressw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpcompressw<full_width>(
    vector_type::from_native(source),
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpcompressw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpcompressw<full_width>(
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
void native_vpcompressw_512_store(
  std::uint16_t * destination,
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  native::mask_vpcompressw<full_width>(
    destination,
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpexpandw<full_width>(
    vector_type::from_native(source),
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpexpandw<full_width>(
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandw_512_load_merge(
  __m512i source,
  __mmask32 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpexpandw<full_width>(
    vector_type::from_native(source),
    native::predicate<32, full_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpexpandw_512_load_zero(
  __mmask32 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpexpandw<full_width>(
    native::predicate<32, full_width>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldw_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::vpshldw<full_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpshldw<full_width, 7>(
    vector_type::from_native(source),
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpshldw<full_width, 7>(
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvw_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::vpshldvw<full_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvw_512_merge(
  __m512i a,
  __mmask32 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpshldvw<full_width>(
    vector_type::from_native(a),
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpshldvw<full_width>(
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdw_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::vpshrdw<full_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpshrdw<full_width, 7>(
    vector_type::from_native(source),
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpshrdw<full_width, 7>(
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvw_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::vpshrdvw<full_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvw_512_merge(
  __m512i a,
  __mmask32 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::mask_vpshrdvw<full_width>(
    vector_type::from_native(a),
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, full_width>;
  return native::maskz_vpshrdvw<full_width>(
    native::predicate<32, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldd_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::vpshldd<full_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldd_512_merge(
  __m512i source,
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::mask_vpshldd<full_width, 7>(
    vector_type::from_native(source),
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::maskz_vpshldd<full_width, 7>(
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvd_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::vpshldvd<full_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvd_512_merge(
  __m512i a,
  __mmask16 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::mask_vpshldvd<full_width>(
    vector_type::from_native(a),
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::maskz_vpshldvd<full_width>(
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdd_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::vpshrdd<full_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdd_512_merge(
  __m512i source,
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::mask_vpshrdd<full_width, 7>(
    vector_type::from_native(source),
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::maskz_vpshrdd<full_width, 7>(
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvd_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::vpshrdvd<full_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvd_512_merge(
  __m512i a,
  __mmask16 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::mask_vpshrdvd<full_width>(
    vector_type::from_native(a),
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, full_width>;
  return native::maskz_vpshrdvd<full_width>(
    native::predicate<16, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldq_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::vpshldq<full_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldq_512_merge(
  __m512i source,
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::mask_vpshldq<full_width, 7>(
    vector_type::from_native(source),
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::maskz_vpshldq<full_width, 7>(
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvq_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::vpshldvq<full_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvq_512_merge(
  __m512i a,
  __mmask8 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::mask_vpshldvq<full_width>(
    vector_type::from_native(a),
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshldvq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::maskz_vpshldvq<full_width>(
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdq_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::vpshrdq<full_width, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdq_512_merge(
  __m512i source,
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::mask_vpshrdq<full_width, 7>(
    vector_type::from_native(source),
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::maskz_vpshrdq<full_width, 7>(
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvq_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::vpshrdvq<full_width>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvq_512_merge(
  __m512i a,
  __mmask8 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::mask_vpshrdvq<full_width>(
    vector_type::from_native(a),
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
__m512i native_vpshrdvq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, full_width>;
  return native::maskz_vpshrdvq<full_width>(
    native::predicate<8, full_width>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline unsigned long long
native_vbmi2_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
