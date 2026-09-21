// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/avx512cd.h>

#if defined(__AVX__) || defined(__AVX512F__) || defined(__AVX512CD__) || defined(__AVX512VL__)
#error Optional instructions must come from function targets
#endif
constexpr auto cd512 = native::target_features<native::x86>(
  "avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd");
constexpr auto cdvl = cd512;

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictd_128_plain(__m128i value) noexcept {
  return native::detail::x86_avx512cd::vpconflictd<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictd_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictd<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictd_128_zero(__mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictd<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntd_128_plain(__m128i value) noexcept {
  return native::detail::x86_avx512cd::vplzcntd<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntd_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntd<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntd_128_zero(__mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntd<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictq_128_plain(__m128i value) noexcept {
  return native::detail::x86_avx512cd::vpconflictq<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictq_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictq<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vpconflictq_128_zero(__mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictq<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntq_128_plain(__m128i value) noexcept {
  return native::detail::x86_avx512cd::vplzcntq<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntq_128_merge(__m128i source, __mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntq<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m128i native_vplzcntq_128_zero(__mmask8 mask, __m128i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntq<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictd_256_plain(__m256i value) noexcept {
  return native::detail::x86_avx512cd::vpconflictd<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictd_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictd<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictd_256_zero(__mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictd<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntd_256_plain(__m256i value) noexcept {
  return native::detail::x86_avx512cd::vplzcntd<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntd_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntd<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntd_256_zero(__mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntd<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictq_256_plain(__m256i value) noexcept {
  return native::detail::x86_avx512cd::vpconflictq<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictq_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictq<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vpconflictq_256_zero(__mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictq<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntq_256_plain(__m256i value) noexcept {
  return native::detail::x86_avx512cd::vplzcntq<cdvl>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntq_256_merge(__m256i source, __mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntq<cdvl>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m256i native_vplzcntq_256_zero(__mmask8 mask, __m256i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntq<cdvl>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictd_512_plain(__m512i value) noexcept {
  return native::detail::x86_avx512cd::vpconflictd<cd512>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictd_512_merge(__m512i source, __mmask16 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictd<cd512>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictd_512_zero(__mmask16 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictd<cd512>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntd_512_plain(__m512i value) noexcept {
  return native::detail::x86_avx512cd::vplzcntd<cd512>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntd_512_merge(__m512i source, __mmask16 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntd<cd512>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntd_512_zero(__mmask16 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntd<cd512>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictq_512_plain(__m512i value) noexcept {
  return native::detail::x86_avx512cd::vpconflictq<cd512>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictq_512_merge(__m512i source, __mmask8 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictq<cd512>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vpconflictq_512_zero(__mmask8 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictq<cd512>(mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntq_512_plain(__m512i value) noexcept {
  return native::detail::x86_avx512cd::vplzcntq<cd512>(value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntq_512_merge(__m512i source, __mmask8 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntq<cd512>(source, mask, value);
}

extern "C" native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512cd")
__m512i native_vplzcntq_512_zero(__mmask8 mask, __m512i value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntq<cd512>(mask, value);
}

extern "C" native_noinline unsigned long long
native_avx512cd_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
