// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/targets.h>
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
#include <utility>
#if SIMD_TEST_IMPORT
import simd;
#else
#include <simd/integer.h>
#include <simd/packing.h>
#if defined(__x86_64__) || defined(_M_X64)
import simd.cpuid;
#elif defined(__aarch64__) || defined(_M_ARM64)
import simd.arm;
#endif
#endif

namespace integer_test {
  template<class T,std::size_t N,class A> concept has_shape=requires { typename simd::vec<T,N,A>::native_type; };
  template<class To,class From,std::size_t N,class A> concept can_pack=requires(simd::vec<From,N,A> a) {
    simd::narrow_concat<To>(a,a);
  };
  template<class To,class From,std::size_t N,class A> concept can_reinterpret=requires(simd::vec<From,N,A> a) {
    simd::reinterpret_bits<To>(a);
  };
  static_assert(!has_shape<std::uint32_t,0,simd::scalar>);
}
#define INTEGER_CASE_NAME scalar
SIMD_TARGET_PUSH(scalar)
#include "case.h"
SIMD_TARGET_POP()
#undef INTEGER_CASE_NAME

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_TARGET_fdq "avx2,fma,bmi2,avx512f,avx512dq"
#define SIMD_TARGET_fdq_vl "avx2,fma,bmi2,avx512f,avx512dq,avx512vl"
#define SIMD_TARGET_fdq_bw "avx2,fma,bmi2,avx512f,avx512dq,avx512bw"
#define INTEGER_CASE_NAME avx2
SIMD_TARGET_PUSH(avx2)
#include "case.h"
SIMD_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME fdq
SIMD_TARGET_PUSH(fdq)
#include "case.h"
SIMD_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME fdq_vl
SIMD_TARGET_PUSH(fdq_vl)
#include "case.h"
SIMD_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME fdq_bw
SIMD_TARGET_PUSH(fdq_bw)
#include "case.h"
SIMD_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_CASE_NAME avx512
SIMD_TARGET_PUSH(avx512)
#include "case.h"
SIMD_TARGET_POP()
#undef INTEGER_CASE_NAME

static_assert(integer_test::can_pack<std::uint32_t,std::uint64_t,8,SIMD_TARGET_TYPE(fdq)>);
static_assert(!integer_test::can_pack<std::uint16_t,std::uint32_t,16,SIMD_TARGET_TYPE(fdq)>);
static_assert(!integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,SIMD_TARGET_TYPE(fdq)>);
static_assert(!integer_test::can_reinterpret<std::uint16_t,std::uint64_t,8,SIMD_TARGET_TYPE(fdq_vl)>);
static_assert(integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,SIMD_TARGET_TYPE(fdq_bw)>);
static_assert(!integer_test::can_reinterpret<std::uint64_t,std::uint32_t,2,simd::avx2>);
static_assert(!integer_test::can_reinterpret<std::uint32_t,std::uint64_t,1,simd::avx2>);
static_assert(integer_test::can_reinterpret<std::int32_t,std::uint32_t,2,simd::avx2>);

#define INTEGER_CODEGEN(name) \
  SIMD_TARGET_PUSH(name) \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_u32(std::uint32_t const * p,std::uint32_t * q) { \
    simd::popcount(simd::vec<std::uint32_t,16,SIMD_TARGET_TYPE(name)>::loadu(p)).storeu(q); \
  } \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_u64(std::uint64_t const * p,std::uint64_t * q) { \
    simd::popcount(simd::vec<std::uint64_t,8,SIMD_TARGET_TYPE(name)>::loadu(p)).storeu(q); \
  } \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_pack(std::uint64_t const * a,std::uint64_t const * b,std::uint32_t * q) { \
    using V=simd::vec<std::uint64_t,8,SIMD_TARGET_TYPE(name)>; \
    simd::narrow_concat<std::uint32_t>(V::loadu(a),V::loadu(b)).storeu(q); \
  } \
  SIMD_TARGET_POP()
INTEGER_CODEGEN(fdq)
INTEGER_CODEGEN(fdq_vl)
INTEGER_CODEGEN(fdq_bw)
INTEGER_CODEGEN(avx512)
#undef INTEGER_CODEGEN
#define INTEGER_NATIVE_CASES(X) X(avx2) X(fdq) X(fdq_vl) X(fdq_bw) X(avx512)
#elif defined(__aarch64__) || defined(_M_ARM64)
#define INTEGER_CASE_NAME neon
SIMD_TARGET_PUSH(neon)
#include "case.h"
SIMD_TARGET_POP()
#undef INTEGER_CASE_NAME
#define INTEGER_NATIVE_CASES(X) X(neon)
#else
#define INTEGER_NATIVE_CASES(X)
#endif

int main() {
  if(!integer_test::scalar::run()) return 1;
  std::puts("integer/packing scalar: executed");
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=simd::observe_x86_capabilities();
#elif defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=simd::observe_arm_capabilities();
#endif
  unsigned executed=1,skipped=0;
#define RUN_CASE(name) \
  if(simd::classify_isa(cpu,SIMD_TARGET_TYPE(name){},SIMD_TARGET_MINIMUM).admitted()) { \
    if(!integer_test::name::run()) { std::puts("integer/packing " #name ": FAILED"); return 2; } \
    ++executed;std::puts("integer/packing " #name ": executed"); \
  } else { ++skipped;std::puts("integer/packing " #name ": skipped (not admitted)"); }
  INTEGER_NATIVE_CASES(RUN_CASE)
#undef RUN_CASE
  std::printf("%u integer/packing cases executed, %u skipped\n",executed,skipped);
}
