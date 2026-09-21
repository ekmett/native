// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/gfni.h>

constexpr native::isa gfni128{native::x86_feature::gfni};
constexpr auto gfni256 = gfni128 & native::x86_feature::avx;
constexpr auto gfni512 = gfni128 & native::x86_feature::avx512f;
constexpr auto gfni_mask512 = gfni512 & native::x86_feature::avx512bw;
constexpr auto gfni_maskvl = gfni_mask512 & native::x86_feature::avx512vl;

#if NATIVE_GFNI_REJECT == 1
native_target("gfni") __m128i rejected(__m128i a, __m128i b) {
  return native::detail::x86_gfni::gf2p8mulb<native::isa{}>(a, b);
}
#elif NATIVE_GFNI_REJECT == 2
native_target("avx,gfni") __m256i rejected(__m256i a, __m256i b) {
  return native::detail::x86_gfni::gf2p8mulb<gfni128>(a, b);
}
#elif NATIVE_GFNI_REJECT == 3
native_target("avx512f,gfni") __m512i rejected(__m512i a, __m512i b) {
  return native::detail::x86_gfni::gf2p8mulb<gfni256>(a, b);
}
#elif NATIVE_GFNI_REJECT == 4
native_target("avx512bw,avx512vl,gfni") __m128i rejected(__m128i a, __m128i b, __mmask16 k) {
  return native::detail::x86_gfni::gf2p8mulb_maskz<gfni_mask512>(k, a, b);
}
#elif NATIVE_GFNI_REJECT == 5
native_target("avx512bw,avx512vl,gfni") __m256i rejected(__m256i a, __m256i b, __mmask32 k) {
  return native::detail::x86_gfni::gf2p8mulb_maskz<gfni512 & native::x86_feature::avx512vl>(k, a, b);
}
#elif NATIVE_GFNI_REJECT == 6
native_target("avx512bw,gfni") __m512i rejected(__m512i a, __m512i b, __mmask64 k) {
  return native::detail::x86_gfni::gf2p8mulb_maskz<gfni512>(k, a, b);
}
#elif NATIVE_GFNI_REJECT == 7
__m128i rejected(__m128i a, __m128i b) {
  return native::detail::x86_gfni::gf2p8mulb<gfni128>(a, b);
}
#elif NATIVE_GFNI_REJECT == 8
native_target("gfni,no-avx") __m256i rejected(__m256i a, __m256i b) {
  return native::detail::x86_gfni::gf2p8mulb<gfni256>(a, b);
}
#elif NATIVE_GFNI_REJECT == 9
native_target("avx,gfni") __m512i rejected(__m512i a, __m512i b) {
  return native::detail::x86_gfni::gf2p8mulb<gfni512>(a, b);
}
#elif NATIVE_GFNI_REJECT == 10
native_target("avx512bw,gfni") __m128i rejected(__m128i a, __m128i b, __mmask16 k) {
  return native::detail::x86_gfni::gf2p8mulb_maskz<gfni_maskvl>(k, a, b);
}
#elif NATIVE_GFNI_REJECT == 11
native_target("avx512f,gfni") __m512i rejected(__m512i a, __m512i b, __mmask64 k) {
  return native::detail::x86_gfni::gf2p8mulb_maskz<gfni_mask512>(k, a, b);
}
#elif NATIVE_GFNI_REJECT == 12
native_target("gfni") __m128i rejected(__m128i a, __m128i b) {
  return native::detail::x86_gfni::gf2p8affineqb<gfni128, 256>(a, b);
}
#elif NATIVE_GFNI_REJECT == 13
native_target("gfni") __m128i rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_gfni::gf2p8affineinvqb<gfni128, immediate>(a, b);
}
#else
#error Select a GFNI negative control
#endif
