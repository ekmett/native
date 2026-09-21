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
  template<class T,std::size_t N,native::isa A> concept has_shape=requires { typename native::vec<T,N,A>::native_type; };
  template<class To,class From,std::size_t N,native::isa A> concept can_pack=requires(native::vec<From,N,A> a) {
    native::narrow_concat<To>(a,a);
  };
  template<class To,class From,std::size_t N,native::isa A> concept can_reinterpret=requires(native::vec<From,N,A> a) {
    native::reinterpret_bits<To>(a);
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
static_assert(!integer_test::can_pack<std::uint16_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq)>);
static_assert(!integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq)>);
static_assert(!integer_test::can_reinterpret<std::uint16_t,std::uint64_t,8,NATIVE_TARGET_ISA(fdq_vl)>);
static_assert(integer_test::can_reinterpret<std::uint8_t,std::uint32_t,16,NATIVE_TARGET_ISA(fdq_bw)>);
static_assert(!integer_test::can_reinterpret<std::uint64_t,std::uint32_t,2,native::avx2>);
static_assert(!integer_test::can_reinterpret<std::uint32_t,std::uint64_t,1,native::avx2>);
static_assert(integer_test::can_reinterpret<std::int32_t,std::uint32_t,2,native::avx2>);

#define INTEGER_CODEGEN(name) \
  NATIVE_TARGET_PUSH(name) \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_u32(std::uint32_t const * p,std::uint32_t * q) { \
    native::popcount(native::vec<std::uint32_t,16,NATIVE_TARGET_ISA(name)>::loadu(p)).storeu(q); \
  } \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_u64(std::uint64_t const * p,std::uint64_t * q) { \
    native::popcount(native::vec<std::uint64_t,8,NATIVE_TARGET_ISA(name)>::loadu(p)).storeu(q); \
  } \
  extern "C" __attribute__((noinline)) void integer_codegen_##name##_pack(std::uint64_t const * a,std::uint64_t const * b,std::uint32_t * q) { \
    using V=native::vec<std::uint64_t,8,NATIVE_TARGET_ISA(name)>; \
    native::narrow_concat<std::uint32_t>(V::loadu(a),V::loadu(b)).storeu(q); \
  } \
  NATIVE_TARGET_POP()
INTEGER_CODEGEN(fdq)
INTEGER_CODEGEN(fdq_vl)
INTEGER_CODEGEN(fdq_bw)
INTEGER_CODEGEN(avx512)
#undef INTEGER_CODEGEN
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
