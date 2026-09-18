// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "refinement.h"
#include "../../src/simd/exp_policies.h"
namespace refinement_test {
  using raw_exp_policies=simd::detail::raw_exp_policies;
  using result_policies=simd::detail::exp_result_policies;
  using exp_refinement=simd::detail::exp_refinement;
  using exp_policies=simd::detail::exp_policies;
  // Keep all eleven historical caller tags independently of production's five
  // binary32 cells, including the extra half features that must not retarget it.
  using caller_refinement=refinement<raw_exp_policies,simd::detail::x86_storage_policies>;
  using caller_policies=caller_refinement::policies;
  static_assert(caller_refinement::cells.size==11);
}
#define SIMD_TARGET_exp_bw_bf16 SIMD_KERNEL_TARGET_7
#define SIMD_TARGET_exp_bw_fp16 SIMD_KERNEL_TARGET_11
#define SIMD_TARGET_exp_bw_half SIMD_KERNEL_TARGET_15
#define SIMD_TARGET_exp_full_half SIMD_KERNEL_TARGET_17

// index, caller target, selected raw-exp cell, historical storage ordinal
#define EXP_CALLER_CELLS(X) \
  X(0,exp_full_half,0,0) \
  X(1,avx512_bf16,0,2) \
  X(2,avx512_fp16,0,4) \
  X(3,avx512,0,6) \
  X(4,exp_bw_half,1,1) \
  X(5,exp_bw_bf16,1,3) \
  X(6,exp_bw_fp16,1,5) \
  X(7,exp_bw,1,7) \
  X(8,exp_vl,2,8) \
  X(9,exp_base,3,9) \
  X(10,avx2,4,10)

#define CHECK_EXP_CALLER(i,name,raw,result) \
  static_assert(SIMD_TARGET_TYPE(name)::features==refinement_test::caller_refinement::cells.records[i].requirements); \
  static_assert(refinement_test::caller_refinement::cells.records[i].choices==std::array<std::size_t,2>{raw,result}); \
  static_assert(simd::requires_abi<SIMD_TARGET_TYPE(name),refinement_test::caller_policies,i>); \
  static_assert(simd::requires_abi<SIMD_TARGET_TYPE(name),refinement_test::exp_policies,raw>);
EXP_CALLER_CELLS(CHECK_EXP_CALLER)
#undef CHECK_EXP_CALLER
