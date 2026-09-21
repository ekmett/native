// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/pclmul.h>
#include <native/x86/vpclmul.h>

using native::x86_feature;
constexpr native::isa legacy{x86_feature::pclmul};
constexpr auto vex128=legacy & x86_feature::avx;
constexpr auto vex256=x86_feature::vpclmulqdq & x86_feature::avx;
constexpr auto evex512=x86_feature::vpclmulqdq & x86_feature::avx512f;
#if NATIVE_PCLMUL_REJECT == 1
native_target("pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<native::scalar,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 2
native_target("avx,pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<native::isa(x86_feature::avx),0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 3
native_target("avx,pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 4
native_target("vpclmulqdq") auto rejected(__m256i a, __m256i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex128,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 5
native_target("vpclmulqdq") auto rejected(__m256i a, __m256i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<native::isa(x86_feature::vpclmulqdq),0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 6
native_target("avx512f,vpclmulqdq") auto rejected(__m512i a, __m512i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<native::isa(x86_feature::avx512f),0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 7
native_target("avx512f,vpclmulqdq") auto rejected(__m512i a, __m512i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 8
auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 9
native_target("pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex128,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 10
native_target("avx,pclmul") auto rejected(__m256i a, __m256i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 11
native_target("pclmul") auto rejected(__m256i a, __m256i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 12
native_target("vpclmulqdq") auto rejected(__m512i a, __m512i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<evex512,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 13
native_target("avx512f,pclmul") auto rejected(__m512i a, __m512i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<evex512,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 14
native_target("pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<legacy,256>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 15
native_target("pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<legacy,0x100000000ULL>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 16
native_target("pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<legacy,-1>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 17
native_target("pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<legacy,immediate>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 18
native_target("pclmul") auto rejected(__m128 a, __m128 b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 19
native_target("vpclmulqdq") auto rejected(__m256 a, __m256 b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 20
native_target("avx512f,vpclmulqdq") auto rejected(__m512 a, __m512 b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<evex512,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 21
native_target("vpclmulqdq") auto rejected(__m256i a, __m128i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 22
native_target("pclmul") auto rejected(unsigned long long a, unsigned long long b, unsigned immediate) {
  return native::detail::x86_pclmul::pclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 23
native_target("avx,pclmul") auto rejected(__m128i a, __m128i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex128,256>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 24
native_target("vpclmulqdq") auto rejected(__m256i a, __m256i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<vex256,256>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 25
native_target("avx512f,vpclmulqdq") auto rejected(__m512i a, __m512i b, unsigned immediate) {
  return native::detail::x86_vpclmul::vpclmulqdq<evex512,256>(a,b);
}
#else
#error Select a carry-less multiplication negative control
#endif
