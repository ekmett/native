// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "../../src/native/abi_refinement.h"
namespace refinement_test {
  template<class... Lists> using refinement=native::detail::abi_refinement<Lists...>;
}
#include "../../src/native/exp_policies.h"
#define NATIVE_TARGET_exp_bw_bf16 NATIVE_KERNEL_TARGET_7
#define NATIVE_TARGET_exp_bw_fp16 NATIVE_KERNEL_TARGET_11
#define NATIVE_TARGET_exp_bw_half NATIVE_KERNEL_TARGET_15
#define NATIVE_TARGET_exp_full_half NATIVE_KERNEL_TARGET_17

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
  using exp_policies=native::isa_list<native::avx512,native::detail::kernel_bw,native::detail::kernel_vl,native::detail::kernel_base,native::avx2>;
  using native::detail::exp_target;
  // Only the test entry points are repeated for each caller tag.
#define CALLER_TARGET(i,name,raw) + native::isa_list<native::target_features<native::x86>(NATIVE_TARGET_STRING(name))>{}
  using caller_targets=decltype(native::isa_list<>{} EXP_CALLER_CASES(CALLER_TARGET));
#undef CALLER_TARGET
}

#define CHECK_EXP_CALLER(i,name,raw) \
  static_assert((native::abi_lookup<native::target_features<native::x86>(NATIVE_TARGET_STRING(name)),refinement_test::caller_targets>::index == i)); \
  static_assert((native::detail::exp_target<native::target_features<native::x86>(NATIVE_TARGET_STRING(name))> == raw));
EXP_CALLER_CASES(CHECK_EXP_CALLER)
#undef CHECK_EXP_CALLER
