// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vbmi;

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512VBMI__)
#error Optional instructions must come from function targets
#endif
// Keep the two tables separated by indices in the probe ABI. This prevents
// Clang from concatenating adjacent stack arguments into one wider VPERMB.
constexpr auto broad = native::target_features<native::x86>("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi");

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermb_128_plain(
  __m128i indices,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::vpermb<broad>(
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i indices,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::mask_vpermb<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermb_128_zero(
  __mmask16 mask,
  __m128i indices,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::maskz_vpermb<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermt2b_128_plain(
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::vpermt2b<broad>(
    vector_type::from_native(a),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermt2b_128_merge(
  __m128i a,
  __mmask16 mask,
  __m128i indices,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::mask_vpermt2b<broad>(
    vector_type::from_native(a),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermt2b_128_zero(
  __mmask16 mask,
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::maskz_vpermt2b<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermi2b_128_plain(
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::vpermi2b<broad>(
    vector_type::from_native(indices),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermi2b_128_merge(
  __m128i a,
  __mmask16 mask,
  __m128i indices,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::mask_vpermi2b<broad>(
    vector_type::from_native(indices),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpermi2b_128_zero(
  __mmask16 mask,
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  return native::maskz_vpermi2b<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpmultishiftqb_128_plain(
  __m128i control,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  using word_type = native::simd<std::uint64_t, 2, broad>;
  return native::vpmultishiftqb<broad>(
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpmultishiftqb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i control,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  using word_type = native::simd<std::uint64_t, 2, broad>;
  return native::mask_vpmultishiftqb<broad>(
    vector_type::from_native(source),
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m128i native_vpmultishiftqb_128_zero(
  __mmask16 mask,
  __m128i control,
  __m128i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 16, broad>;
  using word_type = native::simd<std::uint64_t, 2, broad>;
  return native::maskz_vpmultishiftqb<broad>(
    native::predicate<16, broad>::from_bitset(mask),
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermb_256_plain(
  __m256i indices,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::vpermb<broad>(
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i indices,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::mask_vpermb<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermb_256_zero(
  __mmask32 mask,
  __m256i indices,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::maskz_vpermb<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermt2b_256_plain(
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::vpermt2b<broad>(
    vector_type::from_native(a),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermt2b_256_merge(
  __m256i a,
  __mmask32 mask,
  __m256i indices,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::mask_vpermt2b<broad>(
    vector_type::from_native(a),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermt2b_256_zero(
  __mmask32 mask,
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::maskz_vpermt2b<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermi2b_256_plain(
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::vpermi2b<broad>(
    vector_type::from_native(indices),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermi2b_256_merge(
  __m256i a,
  __mmask32 mask,
  __m256i indices,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::mask_vpermi2b<broad>(
    vector_type::from_native(indices),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpermi2b_256_zero(
  __mmask32 mask,
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  return native::maskz_vpermi2b<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpmultishiftqb_256_plain(
  __m256i control,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  using word_type = native::simd<std::uint64_t, 4, broad>;
  return native::vpmultishiftqb<broad>(
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpmultishiftqb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i control,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  using word_type = native::simd<std::uint64_t, 4, broad>;
  return native::mask_vpmultishiftqb<broad>(
    vector_type::from_native(source),
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m256i native_vpmultishiftqb_256_zero(
  __mmask32 mask,
  __m256i control,
  __m256i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 32, broad>;
  using word_type = native::simd<std::uint64_t, 4, broad>;
  return native::maskz_vpmultishiftqb<broad>(
    native::predicate<32, broad>::from_bitset(mask),
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermb_512_plain(
  __m512i indices,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::vpermb<broad>(
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i indices,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::mask_vpermb<broad>(
    vector_type::from_native(source),
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermb_512_zero(
  __mmask64 mask,
  __m512i indices,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::maskz_vpermb<broad>(
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermt2b_512_plain(
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::vpermt2b<broad>(
    vector_type::from_native(a),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermt2b_512_merge(
  __m512i a,
  __mmask64 mask,
  __m512i indices,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::mask_vpermt2b<broad>(
    vector_type::from_native(a),
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermt2b_512_zero(
  __mmask64 mask,
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::maskz_vpermt2b<broad>(
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(indices),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermi2b_512_plain(
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::vpermi2b<broad>(
    vector_type::from_native(indices),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermi2b_512_merge(
  __m512i a,
  __mmask64 mask,
  __m512i indices,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::mask_vpermi2b<broad>(
    vector_type::from_native(indices),
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpermi2b_512_zero(
  __mmask64 mask,
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  return native::maskz_vpermi2b<broad>(
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(indices),
    vector_type::from_native(a),
    vector_type::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpmultishiftqb_512_plain(
  __m512i control,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  using word_type = native::simd<std::uint64_t, 8, broad>;
  return native::vpmultishiftqb<broad>(
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpmultishiftqb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i control,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  using word_type = native::simd<std::uint64_t, 8, broad>;
  return native::mask_vpmultishiftqb<broad>(
    vector_type::from_native(source),
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
__m512i native_vpmultishiftqb_512_zero(
  __mmask64 mask,
  __m512i control,
  __m512i value) noexcept {
  using vector_type = native::simd<std::uint8_t, 64, broad>;
  using word_type = native::simd<std::uint64_t, 8, broad>;
  return native::maskz_vpmultishiftqb<broad>(
    native::predicate<64, broad>::from_bitset(mask),
    vector_type::from_native(control),
    word_type::from_native(value)).to_native();
}

extern "C" native_noinline unsigned long long
native_vbmi_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
