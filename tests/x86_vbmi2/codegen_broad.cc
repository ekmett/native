// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/x86/vbmi2.h>

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512VBMI2__)
#error Optional instructions must come from function targets
#endif
constexpr auto broad = native::target_features<native::x86>("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2");

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressb<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressb<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressb_128_store(
  std::uint8_t * destination,
  __mmask16 mask,
  __m128i value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressb<broad>(destination, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandb<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_load_merge(
  __m128i source,
  __mmask16 mask,
  std::uint8_t const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<broad>(source, mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandb_128_load_zero(
  __mmask16 mask,
  std::uint8_t const * memory) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandb_128<broad>(mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressw<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpcompressw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressw<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressw_128_store(
  std::uint16_t * destination,
  __mmask8 mask,
  __m128i value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressw<broad>(destination, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandw<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_load_merge(
  __m128i source,
  __mmask8 mask,
  std::uint16_t const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<broad>(source, mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpexpandw_128_load_zero(
  __mmask8 mask,
  std::uint16_t const * memory) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandw_128<broad>(mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldw_128_plain(
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::vpshldw<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldw<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldw<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvw_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvw<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvw_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvw<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvw<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdw_128_plain(
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdw<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdw<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdw<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvw_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvw<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvw_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvw<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvw_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvw<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldd_128_plain(
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::vpshldd<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldd_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldd<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldd<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvd_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvd<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvd_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvd<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvd<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdd_128_plain(
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdd<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdd_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdd<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdd<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvd_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvd<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvd_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvd<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvd_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvd<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldq_128_plain(
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::vpshldq<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldq_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldq<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldq<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvq_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvq<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvq_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvq<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshldvq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvq<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdq_128_plain(
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdq<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdq_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdq<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdq<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvq_128_plain(
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvq<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvq_128_merge(
  __m128i a,
  __mmask8 mask,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvq<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m128i native_vpshrdvq_128_zero(
  __mmask8 mask,
  __m128i a,
  __m128i b,
  __m128i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvq<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressb<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressb<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressb_256_store(
  std::uint8_t * destination,
  __mmask32 mask,
  __m256i value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressb<broad>(destination, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandb<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_load_merge(
  __m256i source,
  __mmask32 mask,
  std::uint8_t const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<broad>(source, mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandb_256_load_zero(
  __mmask32 mask,
  std::uint8_t const * memory) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandb_256<broad>(mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressw<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpcompressw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressw<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressw_256_store(
  std::uint16_t * destination,
  __mmask16 mask,
  __m256i value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressw<broad>(destination, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandw<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_load_merge(
  __m256i source,
  __mmask16 mask,
  std::uint16_t const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<broad>(source, mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpexpandw_256_load_zero(
  __mmask16 mask,
  std::uint16_t const * memory) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandw_256<broad>(mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldw_256_plain(
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::vpshldw<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldw<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldw<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvw_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvw<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvw_256_merge(
  __m256i a,
  __mmask16 mask,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvw<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvw<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdw_256_plain(
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdw<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdw<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdw<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvw_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvw<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvw_256_merge(
  __m256i a,
  __mmask16 mask,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvw<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvw_256_zero(
  __mmask16 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvw<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldd_256_plain(
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::vpshldd<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldd_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldd<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldd<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvd_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvd<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvd_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvd<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvd<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdd_256_plain(
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdd<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdd_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdd<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdd<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvd_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvd<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvd_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvd<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvd_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvd<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldq_256_plain(
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::vpshldq<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldq_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldq<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldq<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvq_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvq<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvq_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvq<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshldvq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvq<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdq_256_plain(
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdq<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdq_256_merge(
  __m256i source,
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdq<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdq<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvq_256_plain(
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvq<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvq_256_merge(
  __m256i a,
  __mmask8 mask,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvq<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m256i native_vpshrdvq_256_zero(
  __mmask8 mask,
  __m256i a,
  __m256i b,
  __m256i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvq<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressb<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressb<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressb_512_store(
  std::uint8_t * destination,
  __mmask64 mask,
  __m512i value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressb<broad>(destination, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandb<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_load_merge(
  __m512i source,
  __mmask64 mask,
  std::uint8_t const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<broad>(source, mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandb_512_load_zero(
  __mmask64 mask,
  std::uint8_t const * memory) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandb_512<broad>(mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressw<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpcompressw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressw<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void native_vpcompressw_512_store(
  std::uint16_t * destination,
  __mmask32 mask,
  __m512i value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressw<broad>(destination, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<broad>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandw<broad>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_load_merge(
  __m512i source,
  __mmask32 mask,
  std::uint16_t const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<broad>(source, mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpexpandw_512_load_zero(
  __mmask32 mask,
  std::uint16_t const * memory) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandw_512<broad>(mask, memory);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldw_512_plain(
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::vpshldw<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldw<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldw<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvw_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvw<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvw_512_merge(
  __m512i a,
  __mmask32 mask,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvw<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvw<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdw_512_plain(
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdw<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdw<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdw<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvw_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvw<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvw_512_merge(
  __m512i a,
  __mmask32 mask,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvw<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvw_512_zero(
  __mmask32 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvw<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldd_512_plain(
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::vpshldd<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldd_512_merge(
  __m512i source,
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldd<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldd<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvd_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvd<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvd_512_merge(
  __m512i a,
  __mmask16 mask,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvd<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvd<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdd_512_plain(
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdd<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdd_512_merge(
  __m512i source,
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdd<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdd<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvd_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvd<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvd_512_merge(
  __m512i a,
  __mmask16 mask,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvd<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvd_512_zero(
  __mmask16 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvd<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldq_512_plain(
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::vpshldq<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldq_512_merge(
  __m512i source,
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldq<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldq<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvq_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvq<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvq_512_merge(
  __m512i a,
  __mmask8 mask,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvq<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshldvq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvq<broad>(mask, a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdq_512_plain(
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::vpshrdq<broad, 7>(a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdq_512_merge(
  __m512i source,
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdq<broad, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdq<broad, 7>(mask, a, b);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvq_512_plain(
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvq<broad>(a, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvq_512_merge(
  __m512i a,
  __mmask8 mask,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvq<broad>(a, mask, b, counts);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
__m512i native_vpshrdvq_512_zero(
  __mmask8 mask,
  __m512i a,
  __m512i b,
  __m512i counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvq<broad>(mask, a, b, counts);
}

extern "C" native_noinline unsigned long long
native_vbmi2_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
