#if defined(NATIVE_PCLMUL_MODULE_NEGATIVE)
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.pclmul;
import native.x86.vpclmul;

using native::x86_feature;
constexpr auto legacy = native::feature_closure(native::isa{x86_feature::pclmul});
constexpr auto vex128=native::feature_closure(legacy & x86_feature::avx);
constexpr auto vex256=native::feature_closure(x86_feature::vpclmulqdq & x86_feature::avx);
constexpr auto evex512=native::feature_closure(x86_feature::vpclmulqdq & x86_feature::avx512f);
#if NATIVE_PCLMUL_REJECT == 1
native_target("pclmul") auto rejected(native::simd<std::uint64_t,2,native::feature_closure(native::isa(native::x86_feature::avx))> a, native::simd<std::uint64_t,2,native::feature_closure(native::isa(native::x86_feature::avx))> b, unsigned immediate) {
  return native::pclmulqdq<native::feature_closure(native::isa(native::x86_feature::avx)),0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 2
native_target("avx,pclmul") auto rejected(native::simd<std::uint64_t,2,native::isa(x86_feature::avx)> a, native::simd<std::uint64_t,2,native::isa(x86_feature::avx)> b, unsigned immediate) {
  return native::vpclmulqdq<native::isa(x86_feature::avx),0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 3
native_target("avx,pclmul") auto rejected(native::simd<std::uint64_t,2,legacy> a, native::simd<std::uint64_t,2,legacy> b, unsigned immediate) {
  return native::vpclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 4
native_target("vpclmulqdq") auto rejected(native::simd<std::uint64_t,4,vex128> a, native::simd<std::uint64_t,4,vex128> b, unsigned immediate) {
  return native::vpclmulqdq<vex128,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 5
native_target("vpclmulqdq") auto rejected(native::simd<std::uint64_t,4,native::isa(x86_feature::vpclmulqdq)> a, native::simd<std::uint64_t,4,native::isa(x86_feature::vpclmulqdq)> b, unsigned immediate) {
  return native::vpclmulqdq<native::isa(x86_feature::vpclmulqdq),0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 6
native_target("avx512f,vpclmulqdq") auto rejected(native::simd<std::uint64_t,8,native::isa(x86_feature::avx512f)> a, native::simd<std::uint64_t,8,native::isa(x86_feature::avx512f)> b, unsigned immediate) {
  return native::vpclmulqdq<native::isa(x86_feature::avx512f),0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 7
native_target("avx512f,vpclmulqdq") auto rejected(native::simd<std::uint64_t,8,vex256> a, native::simd<std::uint64_t,8,vex256> b, unsigned immediate) {
  return native::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 8
auto rejected(native::simd<std::uint64_t,2,legacy> a, native::simd<std::uint64_t,2,legacy> b, unsigned immediate) {
  return native::pclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 9
native_target("pclmul") auto rejected(native::simd<std::uint64_t,2,vex128> a, native::simd<std::uint64_t,2,vex128> b, unsigned immediate) {
  return native::vpclmulqdq<vex128,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 10
native_target("avx,pclmul") auto rejected(native::simd<std::uint64_t,4,vex256> a, native::simd<std::uint64_t,4,vex256> b, unsigned immediate) {
  return native::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 11
native_target("pclmul") auto rejected(native::simd<std::uint64_t,4,vex256> a, native::simd<std::uint64_t,4,vex256> b, unsigned immediate) {
  return native::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 12
native_target("vpclmulqdq") auto rejected(native::simd<std::uint64_t,8,evex512> a, native::simd<std::uint64_t,8,evex512> b, unsigned immediate) {
  return native::vpclmulqdq<evex512,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 13
native_target("avx512f,pclmul") auto rejected(native::simd<std::uint64_t,8,evex512> a, native::simd<std::uint64_t,8,evex512> b, unsigned immediate) {
  return native::vpclmulqdq<evex512,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 14
native_target("pclmul") auto rejected(native::simd<std::uint64_t,2,legacy> a, native::simd<std::uint64_t,2,legacy> b, unsigned immediate) {
  return native::pclmulqdq<legacy,256>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 15
native_target("pclmul") auto rejected(native::simd<std::uint64_t,2,legacy> a, native::simd<std::uint64_t,2,legacy> b, unsigned immediate) {
  return native::pclmulqdq<legacy,0x100000000ULL>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 16
native_target("pclmul") auto rejected(native::simd<std::uint64_t,2,legacy> a, native::simd<std::uint64_t,2,legacy> b, unsigned immediate) {
  return native::pclmulqdq<legacy,-1>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 17
native_target("pclmul") auto rejected(native::simd<std::uint64_t,2,legacy> a, native::simd<std::uint64_t,2,legacy> b, unsigned immediate) {
  return native::pclmulqdq<legacy,immediate>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 18
native_target("pclmul") auto rejected(native::simd<float,4,legacy> a, native::simd<float,4,legacy> b, unsigned immediate) {
  return native::pclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 19
native_target("vpclmulqdq") auto rejected(native::simd<float,8,vex256> a, native::simd<float,8,vex256> b, unsigned immediate) {
  return native::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 20
native_target("avx512f,vpclmulqdq") auto rejected(native::simd<float,16,evex512> a, native::simd<float,16,evex512> b, unsigned immediate) {
  return native::vpclmulqdq<evex512,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 21
native_target("vpclmulqdq") auto rejected(native::simd<std::uint64_t,4,vex256> a, native::simd<std::uint64_t,2,vex256> b, unsigned immediate) {
  return native::vpclmulqdq<vex256,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 22
native_target("pclmul") auto rejected(unsigned long long a, unsigned long long b, unsigned immediate) {
  return native::pclmulqdq<legacy,0>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 23
native_target("avx,pclmul") auto rejected(native::simd<std::uint64_t,2,vex128> a, native::simd<std::uint64_t,2,vex128> b, unsigned immediate) {
  return native::vpclmulqdq<vex128,256>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 24
native_target("vpclmulqdq") auto rejected(native::simd<std::uint64_t,4,vex256> a, native::simd<std::uint64_t,4,vex256> b, unsigned immediate) {
  return native::vpclmulqdq<vex256,256>(a,b);
}
#elif NATIVE_PCLMUL_REJECT == 25
native_target("avx512f,vpclmulqdq") auto rejected(native::simd<std::uint64_t,8,evex512> a, native::simd<std::uint64_t,8,evex512> b, unsigned immediate) {
  return native::vpclmulqdq<evex512,256>(a,b);
}
#else
#error Select a carry-less multiplication negative control
#endif

#else
#include "negative_raw.cc"
#endif
