// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/bitalg.h>

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512BITALG__)
#error Optional instructions must come from function targets
#endif
constexpr auto full_width = native::target_features<native::x86>("avx512f,avx512bw,avx512bitalg");
constexpr auto short_width = native::target_features<native::x86>("avx512f,avx512bw,avx512bitalg,avx512vl");

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntb_128_plain(
  __m128i value) noexcept {
  return native::detail::x86_bitalg::vpopcntb<short_width>(value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntb<short_width>(source, mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntb_128_zero(
  __mmask16 mask,
  __m128i value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntb<short_width>(mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntw_128_plain(
  __m128i value) noexcept {
  return native::detail::x86_bitalg::vpopcntw<short_width>(value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntw_128_merge(
  __m128i source,
  __mmask8 mask,
  __m128i value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntw<short_width>(source, mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m128i native_vpopcntw_128_zero(
  __mmask8 mask,
  __m128i value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntw<short_width>(mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask16 native_vpshufbitqmb_128_plain(
  __m128i value,
  __m128i control) noexcept {
  return native::detail::x86_bitalg::vpshufbitqmb<short_width>(value, control);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask16 native_vpshufbitqmb_128_zero(
  __mmask16 mask,
  __m128i value,
  __m128i control) noexcept {
  return native::detail::x86_bitalg::mask_vpshufbitqmb<short_width>(mask, value, control);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntb_256_plain(
  __m256i value) noexcept {
  return native::detail::x86_bitalg::vpopcntb<short_width>(value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntb<short_width>(source, mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntb_256_zero(
  __mmask32 mask,
  __m256i value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntb<short_width>(mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntw_256_plain(
  __m256i value) noexcept {
  return native::detail::x86_bitalg::vpopcntw<short_width>(value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntw_256_merge(
  __m256i source,
  __mmask16 mask,
  __m256i value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntw<short_width>(source, mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__m256i native_vpopcntw_256_zero(
  __mmask16 mask,
  __m256i value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntw<short_width>(mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask32 native_vpshufbitqmb_256_plain(
  __m256i value,
  __m256i control) noexcept {
  return native::detail::x86_bitalg::vpshufbitqmb<short_width>(value, control);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
__mmask32 native_vpshufbitqmb_256_zero(
  __mmask32 mask,
  __m256i value,
  __m256i control) noexcept {
  return native::detail::x86_bitalg::mask_vpshufbitqmb<short_width>(mask, value, control);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntb_512_plain(
  __m512i value) noexcept {
  return native::detail::x86_bitalg::vpopcntb<full_width>(value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntb<full_width>(source, mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntb_512_zero(
  __mmask64 mask,
  __m512i value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntb<full_width>(mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntw_512_plain(
  __m512i value) noexcept {
  return native::detail::x86_bitalg::vpopcntw<full_width>(value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntw_512_merge(
  __m512i source,
  __mmask32 mask,
  __m512i value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntw<full_width>(source, mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__m512i native_vpopcntw_512_zero(
  __mmask32 mask,
  __m512i value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntw<full_width>(mask, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__mmask64 native_vpshufbitqmb_512_plain(
  __m512i value,
  __m512i control) noexcept {
  return native::detail::x86_bitalg::vpshufbitqmb<full_width>(value, control);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512bitalg")
__mmask64 native_vpshufbitqmb_512_zero(
  __mmask64 mask,
  __m512i value,
  __m512i control) noexcept {
  return native::detail::x86_bitalg::mask_vpshufbitqmb<full_width>(mask, value, control);
}

extern "C" native_noinline unsigned long long
native_bitalg_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
