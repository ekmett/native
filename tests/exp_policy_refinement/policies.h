// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <simd/targets.h>
#include "refinement.h"

// Source names retain literal compiler targets. The C++ refinement below checks
// every emitted record against its computed union and callee-choice signature.
#define SIMD_TARGET_exp_base "avx2,fma,bmi2,avx512f,avx512dq"
#define SIMD_TARGET_exp_bw "avx2,fma,bmi2,avx512f,avx512dq,avx512bw"
#define SIMD_TARGET_exp_vl "avx2,fma,bmi2,avx512f,avx512dq,avx512vl"
#define SIMD_TARGET_exp_bw_bf16 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16"
#define SIMD_TARGET_exp_bw_fp16 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512fp16"
#define SIMD_TARGET_exp_bw_half "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16,avx512fp16"
#define SIMD_TARGET_exp_full_half "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16,avx512fp16"

namespace refinement_test {
  using raw_exp_policies=simd::isa_list<simd::avx512,SIMD_TARGET_TYPE(exp_bw),
    SIMD_TARGET_TYPE(exp_vl),SIMD_TARGET_TYPE(exp_base),simd::avx2>;
  using result_policies=simd::isa_list<SIMD_TARGET_TYPE(exp_full_half),SIMD_TARGET_TYPE(exp_bw_half),
    simd::avx512_bf16,SIMD_TARGET_TYPE(exp_bw_bf16),simd::avx512_fp16,SIMD_TARGET_TYPE(exp_bw_fp16),
    simd::avx512,SIMD_TARGET_TYPE(exp_bw),SIMD_TARGET_TYPE(exp_vl),SIMD_TARGET_TYPE(exp_base),simd::avx2>;
  using exp_refinement=refinement<raw_exp_policies,result_policies>;
  using exp_policies=exp_refinement::policies;
  static_assert(exp_refinement::capacity==55);
  static_assert(exp_refinement::cells.size==11);
}

// index, source target spelling, raw-exp choice, wide-constructor choice
#define EXP_POLICY_CELLS(X) \
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

#define CHECK_EXP_CELL(i,name,raw,result) \
  static_assert(SIMD_TARGET_TYPE(name)::features==refinement_test::exp_refinement::cells.records[i].requirements); \
  static_assert(refinement_test::exp_refinement::cells.records[i].choices==std::array<std::size_t,2>{raw,result}); \
  static_assert(simd::requires_abi<SIMD_TARGET_TYPE(name),refinement_test::exp_policies,i>);
EXP_POLICY_CELLS(CHECK_EXP_CELL)
#undef CHECK_EXP_CELL
