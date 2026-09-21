// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
#include <utility>
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif
#if NATIVE_TEST_IMPORT
import native;
#else
#include <native/integer.h>
#include <native/packing.h>
#if defined(__x86_64__) || defined(_M_X64)
import native.x86.features;
#elif defined(__aarch64__) || defined(_M_ARM64)
import native.arm.features;
#endif
#endif

namespace integer_test {
  template<class T,std::size_t N,native::isa A> concept has_shape=requires { typename native::simd<T,N,A>::native_type; };
  template<class To,class From,std::size_t N,native::isa A> concept can_pack=requires(native::simd<From,N,A> a) {
    native::narrow_concat<To>(a,a);
  };
  template<class To,class From,std::size_t N,native::isa A> concept can_reinterpret=requires(native::simd<From,N,A> a) {
    native::reinterpret_bits<To>(a);
  };
  template<class T,std::size_t N,native::isa A> concept can_popcount=requires(native::simd<T,N,A> a) {
    native::popcount(a);
  };
  template<class T,std::size_t N,native::isa A> concept can_pairwise=requires(native::simd<T,N,A> a) {
    native::pairwise_add_widened(a);
  };
  template<class T,std::size_t N,native::isa A> concept can_reduce=requires(native::simd<T,N,A> a) {
    native::reduce_add_widened(a);
  };
  static_assert(!has_shape<std::uint32_t,0,native::scalar>);
}
#define INTEGER_CASE_NAME scalar
NATIVE_TARGET_PUSH(scalar)
#include "case.h"
NATIVE_TARGET_POP()
#undef INTEGER_CASE_NAME

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_TARGET_fdq "avx2,fma,avx512f,avx512dq"
#define NATIVE_TARGET_fdq_vl "avx2,fma,avx512f,avx512dq,avx512vl"
#define NATIVE_TARGET_fdq_bw "avx2,fma,avx512f,avx512dq,avx512bw"
#define INTEGER_CASE_NAME avx2
NATIVE_TARGET_PUSH(avx2)
#include "case.h"
NATIVE_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME fdq
NATIVE_TARGET_PUSH(fdq)
#include "case.h"
NATIVE_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME fdq_vl
NATIVE_TARGET_PUSH(fdq_vl)
#include "case.h"
NATIVE_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME fdq_bw
NATIVE_TARGET_PUSH(fdq_bw)
#include "case.h"
NATIVE_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME avx512
NATIVE_TARGET_PUSH(avx512)
#include "case.h"
NATIVE_TARGET_POP()
#undef INTEGER_CASE_NAME

static_assert(integer_test::can_pack<std::uint32_t,std::uint64_t,8,NATIVE_TARGET_ISA(fdq)>);
#if NATIVE_TEST_IMPORT
// Module instruction storage supports these bit views and the F/DQ dword pack.
static_assert(integer_test::has_shape<std::uint8_t,64,NATIVE_TARGET_ISA(fdq)>);
static_assert(integer_test::has_shape<std::uint16_t,32,NATIVE_TARGET_ISA(fdq)>);
static_assert(integer_test::can_pack<std::uint16_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq)>);
static_assert(integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq)>);
static_assert(integer_test::can_reinterpret<std::uint16_t,std::uint64_t,8,NATIVE_TARGET_ISA(fdq_vl)>);
#else
static_assert(!integer_test::can_pack<std::uint16_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq)>);
static_assert(!integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq)>);
static_assert(!integer_test::can_reinterpret<std::uint16_t,std::uint64_t,8,NATIVE_TARGET_ISA(fdq_vl)>);
#endif
// A storage-only byte or word register does not acquire integer arithmetic.
static_assert(!integer_test::can_popcount<std::uint8_t,64,NATIVE_TARGET_ISA(fdq)>);
static_assert(!integer_test::can_popcount<std::uint16_t,32,NATIVE_TARGET_ISA(fdq_vl)>);
static_assert(!integer_test::can_pairwise<std::uint8_t,64,NATIVE_TARGET_ISA(fdq)>);
static_assert(!integer_test::can_pairwise<std::uint16_t,32,NATIVE_TARGET_ISA(fdq_vl)>);
static_assert(!integer_test::can_reduce<std::uint8_t,64,NATIVE_TARGET_ISA(fdq)>);
static_assert(!integer_test::can_reduce<std::uint16_t,32,NATIVE_TARGET_ISA(fdq_vl)>);
static_assert(!integer_test::can_pack<std::uint8_t,std::uint16_t,32,NATIVE_TARGET_ISA(fdq)>);
// Without DQ, dispatch selects the 256-bit backend even when 512-bit storage exists.
constexpr auto f_without_dq=native::target_features("avx2,fma,avx512f");
static_assert(!integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,f_without_dq>);
static_assert(!integer_test::can_popcount<std::uint32_t,16,f_without_dq>);
static_assert(!integer_test::can_pairwise<std::uint32_t,16,f_without_dq>);
static_assert(!integer_test::can_reduce<std::uint32_t,16,f_without_dq>);
static_assert(!integer_test::can_pack<std::uint16_t,std::uint32_t,16,f_without_dq>);
static_assert(!integer_test::can_pack<std::uint32_t,std::uint64_t,8,f_without_dq>);
static_assert(integer_test::can_popcount<std::uint8_t,64,NATIVE_TARGET_ISA(fdq_bw)>);
static_assert(integer_test::can_pairwise<std::uint8_t,64,NATIVE_TARGET_ISA(fdq_bw)>);
static_assert(integer_test::can_reduce<std::uint8_t,64,NATIVE_TARGET_ISA(fdq_bw)>);
static_assert(integer_test::can_pack<std::uint8_t,std::uint16_t,32,NATIVE_TARGET_ISA(fdq_bw)>);
static_assert(integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq_bw)>);
static_assert(!integer_test::can_reinterpret<std::uint64_t,std::uint32_t,2,native::avx2>);
static_assert(!integer_test::can_reinterpret<std::uint32_t,std::uint64_t,1,native::avx2>);
static_assert(integer_test::can_reinterpret<std::int32_t,std::uint32_t,2,native::avx2>);

#define INTEGER_CODEGEN(name) \
  NATIVE_TARGET_PUSH(name) \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_u32(std::uint32_t const * p,std::uint32_t * q) { \
    native::popcount(native::simd<std::uint32_t,16,NATIVE_TARGET_ISA(name)>::loadu(p)).storeu(q); \
  } \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_u64(std::uint64_t const * p,std::uint64_t * q) { \
    native::popcount(native::simd<std::uint64_t,8,NATIVE_TARGET_ISA(name)>::loadu(p)).storeu(q); \
  } \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_pack(std::uint64_t const * a,std::uint64_t const * b,std::uint32_t * q) { \
    using V=native::simd<std::uint64_t,8,NATIVE_TARGET_ISA(name)>; \
    native::narrow_concat<std::uint32_t>(V::loadu(a),V::loadu(b)).storeu(q); \
  } \
  NATIVE_TARGET_POP()
INTEGER_CODEGEN(fdq)
INTEGER_CODEGEN(fdq_vl)
INTEGER_CODEGEN(fdq_bw)
INTEGER_CODEGEN(avx512)
#undef INTEGER_CODEGEN
#if NATIVE_TEST_IMPORT
#define INTEGER_STORAGE_CODEGEN(name) \
  NATIVE_TARGET_PUSH(name) \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_pack_words(std::uint32_t const * a,std::uint32_t const * b,std::uint16_t * q) { \
    using V=native::simd<std::uint32_t,16,NATIVE_TARGET_ISA(name)>; \
    native::narrow_concat<std::uint16_t>(V::loadu(a),V::loadu(b)).storeu(q); \
  } \
  extern "C" __attribute__((noinline)) __m512i integer_codegen_##name##_identity(__m512i value) { return value; } \
  extern "C" __attribute__((noinline)) __m512i integer_codegen_##name##_reinterpret(__m512i value) { \
    using V=native::simd<std::uint32_t,16,NATIVE_TARGET_ISA(name)>; \
    return native::reinterpret_bits<std::uint8_t>(V::from_native(value)).to_native(); \
  } \
  NATIVE_TARGET_POP()
INTEGER_STORAGE_CODEGEN(fdq)
INTEGER_STORAGE_CODEGEN(fdq_vl)
#undef INTEGER_STORAGE_CODEGEN
#endif
#define INTEGER_NATIVE_CASES(X) X(avx2) X(fdq) X(fdq_vl) X(fdq_bw) X(avx512)
#elif defined(__aarch64__) || defined(_M_ARM64)
#define INTEGER_CASE_NAME neon
NATIVE_TARGET_PUSH(neon)
#include "case.h"
NATIVE_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_NATIVE_CASES(X) X(neon)
#else
#define INTEGER_NATIVE_CASES(X)
#endif

int main() {
  if(!integer_test::scalar::run()) return 1;
  std::puts("integer/packing scalar: executed");
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=native::observe_x86_capabilities();
#elif defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=native::observe_arm_capabilities();
#endif
  unsigned executed=1,skipped=0;
#define RUN_CASE(name) \
  if(native::classify_isa(cpu,NATIVE_TARGET_ISA(name),NATIVE_TARGET_MINIMUM).admitted()) { \
    if(!integer_test::name::run()) { std::puts("integer/packing " #name ": FAILED"); return 2; } \
    ++executed;std::puts("integer/packing " #name ": executed"); \
  } else { ++skipped;std::puts("integer/packing " #name ": skipped (not admitted)"); }
  INTEGER_NATIVE_CASES(RUN_CASE)
#undef RUN_CASE
  std::printf("%u integer/packing cases executed, %u skipped\n",executed,skipped);
}
