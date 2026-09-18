// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <simd/targets.h>
#include "simd/kernel_policies.h"

// Source names retain literal compiler targets. The C++ refinement below checks
// every emitted record against its computed union and callee-choice signature.
#define SIMD_TARGET_exp_base SIMD_KERNEL_TARGET_2
#define SIMD_TARGET_exp_bw SIMD_KERNEL_TARGET_3
#define SIMD_TARGET_exp_vl SIMD_KERNEL_TARGET_4

namespace simd::detail {
  using raw_exp_policies=x86_kernel_policies;
  // A binary32 result uses raw float constructors regardless of extra half
  // features in its retained Arch. Native half element policies are separate.
  using exp_result_policies=x86_kernel_policies;
  // Named transitive summaries: all raw operations currently inherit the same
  // five backend declaration scopes. VL is a genuine mask/scaling boundary;
  // BW remains a backend attribute requirement for this binary32 graph.
  using exp_compare_select_policies=raw_exp_policies;
  using exp_multiply_fma_policies=raw_exp_policies;
  using exp_round_policies=raw_exp_policies;
  using exp_scaling_policies=raw_exp_policies; // includes fallback integer/memory work
  static_assert(std::same_as<exp_compare_select_policies,exp_multiply_fma_policies> &&
    std::same_as<exp_compare_select_policies,exp_round_policies> &&
    std::same_as<exp_compare_select_policies,exp_scaling_policies>);
  // Identical partitions are composed once, not once per polynomial operation.
  using exp_refinement=abi_refinement<exp_compare_select_policies,exp_result_policies>;
  using exp_policies=exp_refinement::policies;
  static_assert(exp_refinement::capacity==25);
  static_assert(exp_refinement::cells.size==5);
}

// index, source target spelling, raw-exp choice, binary32-result choice
#define SIMD_EXP_POLICY_CELLS(X) \
  X(0,avx512,0,0) \
  X(1,exp_bw,1,1) \
  X(2,exp_vl,2,2) \
  X(3,exp_base,3,3) \
  X(4,avx2,4,4)

#define CHECK_EXP_CELL(i,name,raw,result) \
  static_assert(SIMD_TARGET_TYPE(name)::features==simd::detail::exp_refinement::cells.records[i].requirements); \
  static_assert(simd::detail::exp_refinement::cells.records[i].choices==std::array<std::size_t,2>{raw,result}); \
  static_assert(simd::requires_abi<SIMD_TARGET_TYPE(name),simd::detail::exp_policies,i>);
SIMD_EXP_POLICY_CELLS(CHECK_EXP_CELL)
#undef CHECK_EXP_CELL
