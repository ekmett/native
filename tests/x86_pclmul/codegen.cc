// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/pclmul.h>
#include <native/x86/vpclmul.h>
using native::x86_feature;
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_0(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),0>(a,b);
}
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_1(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),1>(a,b);
}
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_16(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),16>(a,b);
}
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_17(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),17>(a,b);
}
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_238(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),238>(a,b);
}
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_239(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),239>(a,b);
}
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_254(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),254>(a,b);
}
extern "C" native_noinline native_target("pclmul")
__m128i native_pclmul_legacy_255(__m128i a, __m128i b) {
  return native::detail::x86_pclmul::pclmulqdq<native::isa<native::x86>(x86_feature::pclmul),255>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_0(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,0>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_1(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,1>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_16(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,16>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_17(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,17>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_238(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,238>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_239(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,239>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_254(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,254>(a,b);
}
extern "C" native_noinline native_target("avx,pclmul")
__m128i native_pclmul_vex128_255(__m128i a, __m128i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::pclmul & x86_feature::avx,255>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_0(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,0>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_1(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,1>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_16(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,16>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_17(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,17>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_238(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,238>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_239(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,239>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_254(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,254>(a,b);
}
extern "C" native_noinline native_target("avx,vpclmulqdq")
__m256i native_pclmul_vex256_255(__m256i a, __m256i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx,255>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_0(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,0>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_1(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,1>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_16(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,16>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_17(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,17>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_238(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,238>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_239(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,239>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_254(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,254>(a,b);
}
extern "C" native_noinline native_target("avx512f,vpclmulqdq")
__m512i native_pclmul_evex512_255(__m512i a, __m512i b) {
  return native::detail::x86_vpclmul::vpclmulqdq<x86_feature::vpclmulqdq & x86_feature::avx512f,255>(a,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_0(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),0>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_1(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),1>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_16(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),16>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_17(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),17>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_238(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),238>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_239(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),239>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_254(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),254>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m128i native_pclmul_evex128_255(__m128i a, __m128i b) {
  register __m128i high __asm__("xmm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),255>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_0(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),0>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_1(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),1>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_16(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),16>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_17(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),17>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_238(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),238>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_239(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),239>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_254(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),254>(high,b);
}
// A high source register prevents LLVM from compressing EVEX to VEX.
extern "C" native_noinline native_target("avx512f,avx512vl,vpclmulqdq")
__m256i native_pclmul_evex256_255(__m256i a, __m256i b) {
  register __m256i high __asm__("ymm16") = a;
  __asm__("" : "+v"(high));
  return native::detail::x86_vpclmul::vpclmulqdq<native::target_features<native::x86>("avx512f,avx512vl,vpclmulqdq"),255>(high,b);
}
