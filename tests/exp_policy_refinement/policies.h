// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "refinement.h"
#include "../../src/simd/exp_policies.h"
#define SIMD_TARGET_exp_bw_bf16 SIMD_KERNEL_TARGET_7
#define SIMD_TARGET_exp_bw_fp16 SIMD_KERNEL_TARGET_11
#define SIMD_TARGET_exp_bw_half SIMD_KERNEL_TARGET_15
#define SIMD_TARGET_exp_full_half SIMD_KERNEL_TARGET_17

// Test inputs: case number, caller tag, expected one of five FP32 exp cells.
// Extra half features must leave that selected FP32 implementation unchanged.
#define EXP_CALLER_CASES(X) \
  X(0,exp_full_half,0) \
  X(1,avx512_bf16,0) \
  X(2,avx512_fp16,0) \
  X(3,avx512,0) \
  X(4,exp_bw_half,1) \
  X(5,exp_bw_bf16,1) \
  X(6,exp_bw_fp16,1) \
  X(7,exp_bw,1) \
  X(8,exp_vl,2) \
  X(9,exp_base,3) \
  X(10,avx2,4)

namespace refinement_test {
  using exp_policies=simd::detail::x86_kernel_policies;
  using simd::detail::exp_target;
  // Only the test entry points are repeated for each caller tag.
#define CALLER_TARGET(i,name,raw) + simd::isa_list<SIMD_TARGET_TYPE(name)>{}
  using caller_targets=decltype(simd::isa_list<>{} EXP_CALLER_CASES(CALLER_TARGET));
#undef CALLER_TARGET
}

#define CHECK_EXP_CALLER(i,name,raw) \
  static_assert((simd::target<SIMD_TARGET_TYPE(name),refinement_test::caller_targets> == i)); \
  static_assert((simd::detail::exp_target<SIMD_TARGET_TYPE(name)> == raw));
EXP_CALLER_CASES(CHECK_EXP_CALLER)
#undef CHECK_EXP_CALLER
