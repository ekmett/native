// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/targets.h>
#include <array>
#include <bit>
#include <cstdio>
#include <utility>
#if SIMD_TEST_IMPORT
import simd;
#else
#include <simd/vec.h>
#include <simd/wide.h>
#include <simd/simd/math/exp.h>
#if defined(__x86_64__) || defined(_M_X64)
import simd.cpuid;
#elif defined(__aarch64__) || defined(_M_ARM64)
import simd.arm;
#endif
#endif
#include "traits.h"

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_TARGET_empty_base "avx2,fma,bmi2,avx512f,avx512dq"
#define SIMD_TARGET_empty_bw "avx2,fma,bmi2,avx512f,avx512dq,avx512bw"
#define SIMD_TARGET_empty_vl "avx2,fma,bmi2,avx512f,avx512dq,avx512vl"
#define SIMD_TARGET_empty_bw_bf16 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16"
#define SIMD_TARGET_empty_bw_fp16 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512fp16"
#define SIMD_TARGET_empty_bw_half "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16,avx512fp16"
#define SIMD_TARGET_empty_half "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16,avx512fp16"
#define NATIVE_CASES(X) X(avx2,8) X(empty_base,16) X(empty_bw,16) X(empty_vl,16) X(avx512,16) \
  X(empty_bw_bf16,16) X(avx512_bf16,16) X(empty_bw_fp16,16) X(avx512_fp16,16) \
  X(empty_bw_half,16) X(empty_half,16)
#elif defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_TARGET_empty_half "neon,fullfp16,bf16"
#define NATIVE_CASES(X) X(neon,4) X(neon_bf16,4) X(neon_fp16,4) X(empty_half,4)
#else
#define NATIVE_CASES(X)
#endif

#define DEFINE_CASE(name,width) \
  SIMD_TARGET_PUSH(name) \
  __attribute__((noinline)) bool construct_##name() { \
    constexpr auto A = SIMD_TARGET_ISA(name); \
    auto empty=[]<class T>() { \
      static_assert(same_traits<T,0>() && same_traits<T,1>() && same_traits<T,3>()); \
      simd::wide<T,0> defaulted; \
      simd::wide<T,0> valued{}; \
      auto copied=defaulted; \
      auto moved=std::move(valued); \
      return copied.registers.empty() && moved.registers.empty(); \
    }; \
    using V=simd::vec<float,1,A>; \
    auto positive=[]<std::size_t N>() { \
      simd::wide<V,N> defaulted; \
      simd::wide<V,N> valued{}; \
      V original_default; \
      V original_value{}; \
      for(std::size_t i=0;i<N;++i) \
        if(std::bit_cast<unsigned>(defaulted.registers[i].value)!=std::bit_cast<unsigned>(original_default.value) || \
           std::bit_cast<unsigned>(valued.registers[i].value)!=std::bit_cast<unsigned>(original_value.value)) return false; \
      return true; \
    }; \
    simd::wide<V,0> input; \
    auto result=simd::exp(input); \
    return positive.template operator()<1>() && positive.template operator()<3>() && \
      result.registers.empty() && empty.template operator()<V>() && \
      empty.template operator()<simd::vec<float,2,A>>() && \
      empty.template operator()<simd::vec<float,3,A>>() && \
      empty.template operator()<simd::vec<float,4,A>>() && \
      empty.template operator()<simd::vec<float,width,A>>() && \
      empty.template operator()<simd::vec<simd::uint32_t,1,A>>() && \
      empty.template operator()<typename V::mask>() && \
      empty.template operator()<simd::wide<V,0>>(); \
  } \
  SIMD_TARGET_POP()
NATIVE_CASES(DEFINE_CASE)
#undef DEFINE_CASE

int native_construction() {
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=simd::observe_x86_capabilities();
#elif defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=simd::observe_arm_capabilities();
#endif
  unsigned executed=0,skipped=0;
#define RUN_CASE(name,width) \
  if(simd::classify_isa(cpu,SIMD_TARGET_ISA(name),SIMD_TARGET_MINIMUM).admitted()) { \
    if(!construct_##name()) return 3; \
    ++executed; \
  } else ++skipped;
  NATIVE_CASES(RUN_CASE)
#undef RUN_CASE
  std::printf("native empty construction: %u cases executed, %u skipped\n",executed,skipped);
  return 0;
}
