// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vbmi2;

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512VBMI2__)
#error Optional instructions must come from function targets
#endif
constexpr auto broad = native::target_features<native::x86>("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2");

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::mask_vpcompressb<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::maskz_vpcompressb<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressb_128_store(
  std::uint8_t * destination,
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  native::mask_vpcompressb<broad>(
    destination,
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::mask_vpexpandb<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::maskz_vpexpandb<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_load_merge(
  __m128i source,
  __mmask16 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::mask_vpexpandb<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_load_zero(
  __mmask16 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::maskz_vpexpandb<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::mask_vpcompressw<broad>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::maskz_vpcompressw<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressw_128_store(
  std::uint16_t * destination,
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  native::mask_vpcompressw<broad>(
    destination,
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::mask_vpexpandw<broad>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::maskz_vpexpandw<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_load_merge(
  __m128i source,
  __mmask8 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::mask_vpexpandw<broad>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_load_zero(
  __mmask8 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::maskz_vpexpandw<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldw_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::vpshldw<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::mask_vpshldw<broad, 7>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::maskz_vpshldw<broad, 7>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvw_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::vpshldvw<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvw_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::mask_vpshldvw<broad>(
    vector_type::from_native(a),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::maskz_vpshldvw<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdw_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::vpshrdw<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::mask_vpshrdw<broad, 7>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::maskz_vpshrdw<broad, 7>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvw_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::vpshrdvw<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvw_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::mask_vpshrdvw<broad>(
    vector_type::from_native(a),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 8, broad>;
  return native::maskz_vpshrdvw<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldd_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::vpshldd<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldd_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::mask_vpshldd<broad, 7>(
    vector_type::from_native(source),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::maskz_vpshldd<broad, 7>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvd_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::vpshldvd<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvd_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::mask_vpshldvd<broad>(
    vector_type::from_native(a),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::maskz_vpshldvd<broad>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdd_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::vpshrdd<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdd_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::mask_vpshrdd<broad, 7>(
    vector_type::from_native(source),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::maskz_vpshrdd<broad, 7>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvd_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::vpshrdvd<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvd_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::mask_vpshrdvd<broad>(
    vector_type::from_native(a),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 4, broad>;
  return native::maskz_vpshrdvd<broad>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldq_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::vpshldq<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldq_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::mask_vpshldq<broad, 7>(
    vector_type::from_native(source),
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::maskz_vpshldq<broad, 7>(
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvq_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::vpshldvq<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvq_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::mask_vpshldvq<broad>(
    vector_type::from_native(a),
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::maskz_vpshldvq<broad>(
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdq_128_plain(
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::vpshrdq<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdq_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::mask_vpshrdq<broad, 7>(
    vector_type::from_native(source),
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::maskz_vpshrdq<broad, 7>(
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvq_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::vpshrdvq<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvq_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::mask_vpshrdvq<broad>(
    vector_type::from_native(a),
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 2, broad>;
  return native::maskz_vpshrdvq<broad>(
    native::predicate<2, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::mask_vpcompressb<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::maskz_vpcompressb<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressb_256_store(
  std::uint8_t * destination,
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  native::mask_vpcompressb<broad>(
    destination,
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::mask_vpexpandb<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::maskz_vpexpandb<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_load_merge(
  __m256i source,
  __mmask32 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::mask_vpexpandb<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_load_zero(
  __mmask32 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::maskz_vpexpandb<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::mask_vpcompressw<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::maskz_vpcompressw<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressw_256_store(
  std::uint16_t * destination,
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  native::mask_vpcompressw<broad>(
    destination,
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::mask_vpexpandw<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::maskz_vpexpandw<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_load_merge(
  __m256i source,
  __mmask16 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::mask_vpexpandw<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_load_zero(
  __mmask16 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::maskz_vpexpandw<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldw_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::vpshldw<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::mask_vpshldw<broad, 7>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::maskz_vpshldw<broad, 7>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvw_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::vpshldvw<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvw_256_merge(
  __m256i a,
  __mmask16 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::mask_vpshldvw<broad>(
    vector_type::from_native(a),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::maskz_vpshldvw<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdw_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::vpshrdw<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::mask_vpshrdw<broad, 7>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::maskz_vpshrdw<broad, 7>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvw_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::vpshrdvw<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvw_256_merge(
  __m256i a,
  __mmask16 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::mask_vpshrdvw<broad>(
    vector_type::from_native(a),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 16, broad>;
  return native::maskz_vpshrdvw<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldd_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::vpshldd<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldd_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::mask_vpshldd<broad, 7>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::maskz_vpshldd<broad, 7>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvd_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::vpshldvd<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvd_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::mask_vpshldvd<broad>(
    vector_type::from_native(a),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::maskz_vpshldvd<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdd_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::vpshrdd<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdd_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::mask_vpshrdd<broad, 7>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::maskz_vpshrdd<broad, 7>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvd_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::vpshrdvd<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvd_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::mask_vpshrdvd<broad>(
    vector_type::from_native(a),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 8, broad>;
  return native::maskz_vpshrdvd<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldq_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::vpshldq<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldq_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::mask_vpshldq<broad, 7>(
    vector_type::from_native(source),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::maskz_vpshldq<broad, 7>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvq_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::vpshldvq<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvq_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::mask_vpshldvq<broad>(
    vector_type::from_native(a),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::maskz_vpshldvq<broad>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdq_256_plain(
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::vpshrdq<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdq_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::mask_vpshrdq<broad, 7>(
    vector_type::from_native(source),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::maskz_vpshrdq<broad, 7>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvq_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::vpshrdvq<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvq_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::mask_vpshrdvq<broad>(
    vector_type::from_native(a),
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 4, broad>;
  return native::maskz_vpshrdvq<broad>(
    native::predicate<4, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::mask_vpcompressb<broad>(
    vector_type::from_native(source),
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::maskz_vpcompressb<broad>(
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressb_512_store(
  std::uint8_t * destination,
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  native::mask_vpcompressb<broad>(
    destination,
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::mask_vpexpandb<broad>(
    vector_type::from_native(source),
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::maskz_vpexpandb<broad>(
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_load_merge(
  __m512i source,
  __mmask64 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::mask_vpexpandb<broad>(
    vector_type::from_native(source),
    native::predicate<64, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_load_zero(
  __mmask64 mask,
  std::uint8_t const * memory) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::maskz_vpexpandb<broad>(
    native::predicate<64, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::mask_vpcompressw<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::maskz_vpcompressw<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressw_512_store(
  std::uint16_t * destination,
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  native::mask_vpcompressw<broad>(
    destination,
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value));
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::mask_vpexpandw<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::maskz_vpexpandw<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_load_merge(
  __m512i source,
  __mmask32 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::mask_vpexpandw<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_load_zero(
  __mmask32 mask,
  std::uint16_t const * memory) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::maskz_vpexpandw<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    memory).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldw_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::vpshldw<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::mask_vpshldw<broad, 7>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::maskz_vpshldw<broad, 7>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvw_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::vpshldvw<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvw_512_merge(
  __m512i a,
  __mmask32 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::mask_vpshldvw<broad>(
    vector_type::from_native(a),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::maskz_vpshldvw<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdw_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::vpshrdw<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::mask_vpshrdw<broad, 7>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::maskz_vpshrdw<broad, 7>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvw_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::vpshrdvw<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvw_512_merge(
  __m512i a,
  __mmask32 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::mask_vpshrdvw<broad>(
    vector_type::from_native(a),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint16_t, 32, broad>;
  return native::maskz_vpshrdvw<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldd_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::vpshldd<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldd_512_merge(
  __m512i source,
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::mask_vpshldd<broad, 7>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::maskz_vpshldd<broad, 7>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvd_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::vpshldvd<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvd_512_merge(
  __m512i a,
  __mmask16 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::mask_vpshldvd<broad>(
    vector_type::from_native(a),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::maskz_vpshldvd<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdd_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::vpshrdd<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdd_512_merge(
  __m512i source,
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::mask_vpshrdd<broad, 7>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::maskz_vpshrdd<broad, 7>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvd_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::vpshrdvd<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvd_512_merge(
  __m512i a,
  __mmask16 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::mask_vpshrdvd<broad>(
    vector_type::from_native(a),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint32_t, 16, broad>;
  return native::maskz_vpshrdvd<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldq_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::vpshldq<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldq_512_merge(
  __m512i source,
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::mask_vpshldq<broad, 7>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::maskz_vpshldq<broad, 7>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvq_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::vpshldvq<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvq_512_merge(
  __m512i a,
  __mmask8 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::mask_vpshldvq<broad>(
    vector_type::from_native(a),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::maskz_vpshldvq<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdq_512_plain(
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::vpshrdq<broad, 7>(
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdq_512_merge(
  __m512i source,
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::mask_vpshrdq<broad, 7>(
    vector_type::from_native(source),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::maskz_vpshrdq<broad, 7>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvq_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::vpshrdvq<broad>(
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvq_512_merge(
  __m512i a,
  __mmask8 mask,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::mask_vpshrdvq<broad>(
    vector_type::from_native(a),
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  using vector_type = native::simd<std::uint64_t, 8, broad>;
  return native::maskz_vpshrdvq<broad>(
    native::predicate<8, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b),
    vector_type::from_native(counts)).to_native();
}

extern "C" native_noinline unsigned long long
native_vbmi2_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
