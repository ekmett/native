// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/vbmi.h>

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512VBMI__)
#error Optional instructions must come from function targets
#endif
// Keep the two tables separated by indices in the probe ABI. This prevents
// Clang from concatenating adjacent stack arguments into one wider VPERMB.
constexpr auto full_width = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi");
constexpr auto short_width = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi,avx512vl");

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermb_128_plain(
  __m128i indices,
  __m128i value) noexcept {
  return native::detail::x86_vbmi::vpermb<short_width>(indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i indices,
  __m128i value) noexcept {
  return native::detail::x86_vbmi::mask_vpermb<short_width>(source, mask, indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermb_128_zero(
  __mmask16 mask,
  __m128i indices,
  __m128i value) noexcept {
  return native::detail::x86_vbmi::maskz_vpermb<short_width>(mask, indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermt2b_128_plain(
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  return native::detail::x86_vbmi::vpermt2b<short_width>(a, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermt2b_128_merge(
  __m128i a,
  __mmask16 mask,
  __m128i indices,
  __m128i b) noexcept {
  return native::detail::x86_vbmi::mask_vpermt2b<short_width>(a, mask, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermt2b_128_zero(
  __mmask16 mask,
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermt2b<short_width>(mask, a, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermi2b_128_plain(
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  return native::detail::x86_vbmi::vpermi2b<short_width>(indices, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermi2b_128_merge(
  __m128i a,
  __mmask16 mask,
  __m128i indices,
  __m128i b) noexcept {
  return native::detail::x86_vbmi::mask_vpermi2b<short_width>(indices, mask, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpermi2b_128_zero(
  __mmask16 mask,
  __m128i a,
  __m128i indices,
  __m128i b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermi2b<short_width>(mask, indices, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpmultishiftqb_128_plain(
  __m128i control,
  __m128i value) noexcept {
  return native::detail::x86_vbmi::vpmultishiftqb<short_width>(control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpmultishiftqb_128_merge(
  __m128i source,
  __mmask16 mask,
  __m128i control,
  __m128i value) noexcept {
  return native::detail::x86_vbmi::mask_vpmultishiftqb<short_width>(source, mask, control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m128i native_vpmultishiftqb_128_zero(
  __mmask16 mask,
  __m128i control,
  __m128i value) noexcept {
  return native::detail::x86_vbmi::maskz_vpmultishiftqb<short_width>(mask, control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermb_256_plain(
  __m256i indices,
  __m256i value) noexcept {
  return native::detail::x86_vbmi::vpermb<short_width>(indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i indices,
  __m256i value) noexcept {
  return native::detail::x86_vbmi::mask_vpermb<short_width>(source, mask, indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermb_256_zero(
  __mmask32 mask,
  __m256i indices,
  __m256i value) noexcept {
  return native::detail::x86_vbmi::maskz_vpermb<short_width>(mask, indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermt2b_256_plain(
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  return native::detail::x86_vbmi::vpermt2b<short_width>(a, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermt2b_256_merge(
  __m256i a,
  __mmask32 mask,
  __m256i indices,
  __m256i b) noexcept {
  return native::detail::x86_vbmi::mask_vpermt2b<short_width>(a, mask, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermt2b_256_zero(
  __mmask32 mask,
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermt2b<short_width>(mask, a, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermi2b_256_plain(
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  return native::detail::x86_vbmi::vpermi2b<short_width>(indices, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermi2b_256_merge(
  __m256i a,
  __mmask32 mask,
  __m256i indices,
  __m256i b) noexcept {
  return native::detail::x86_vbmi::mask_vpermi2b<short_width>(indices, mask, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpermi2b_256_zero(
  __mmask32 mask,
  __m256i a,
  __m256i indices,
  __m256i b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermi2b<short_width>(mask, indices, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpmultishiftqb_256_plain(
  __m256i control,
  __m256i value) noexcept {
  return native::detail::x86_vbmi::vpmultishiftqb<short_width>(control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpmultishiftqb_256_merge(
  __m256i source,
  __mmask32 mask,
  __m256i control,
  __m256i value) noexcept {
  return native::detail::x86_vbmi::mask_vpmultishiftqb<short_width>(source, mask, control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
__m256i native_vpmultishiftqb_256_zero(
  __mmask32 mask,
  __m256i control,
  __m256i value) noexcept {
  return native::detail::x86_vbmi::maskz_vpmultishiftqb<short_width>(mask, control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermb_512_plain(
  __m512i indices,
  __m512i value) noexcept {
  return native::detail::x86_vbmi::vpermb<full_width>(indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i indices,
  __m512i value) noexcept {
  return native::detail::x86_vbmi::mask_vpermb<full_width>(source, mask, indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermb_512_zero(
  __mmask64 mask,
  __m512i indices,
  __m512i value) noexcept {
  return native::detail::x86_vbmi::maskz_vpermb<full_width>(mask, indices, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermt2b_512_plain(
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  return native::detail::x86_vbmi::vpermt2b<full_width>(a, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermt2b_512_merge(
  __m512i a,
  __mmask64 mask,
  __m512i indices,
  __m512i b) noexcept {
  return native::detail::x86_vbmi::mask_vpermt2b<full_width>(a, mask, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermt2b_512_zero(
  __mmask64 mask,
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermt2b<full_width>(mask, a, indices, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermi2b_512_plain(
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  return native::detail::x86_vbmi::vpermi2b<full_width>(indices, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermi2b_512_merge(
  __m512i a,
  __mmask64 mask,
  __m512i indices,
  __m512i b) noexcept {
  return native::detail::x86_vbmi::mask_vpermi2b<full_width>(indices, mask, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpermi2b_512_zero(
  __mmask64 mask,
  __m512i a,
  __m512i indices,
  __m512i b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermi2b<full_width>(mask, indices, a, b);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpmultishiftqb_512_plain(
  __m512i control,
  __m512i value) noexcept {
  return native::detail::x86_vbmi::vpmultishiftqb<full_width>(control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpmultishiftqb_512_merge(
  __m512i source,
  __mmask64 mask,
  __m512i control,
  __m512i value) noexcept {
  return native::detail::x86_vbmi::mask_vpmultishiftqb<full_width>(source, mask, control, value);
}

extern "C" native_noinline native_target("avx512f,avx512bw,avx512vbmi")
__m512i native_vpmultishiftqb_512_zero(
  __mmask64 mask,
  __m512i control,
  __m512i value) noexcept {
  return native::detail::x86_vbmi::maskz_vpmultishiftqb<full_width>(mask, control, value);
}

extern "C" native_noinline unsigned long long
native_vbmi_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
