// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <simd/targets.h>
#include "simd/kernel_policies.h"

// Clang attributes require literal target strings.
#define SIMD_TARGET_exp_base SIMD_KERNEL_TARGET_2
#define SIMD_TARGET_exp_bw SIMD_KERNEL_TARGET_3
#define SIMD_TARGET_exp_vl SIMD_KERNEL_TARGET_4

namespace simd::detail {
  // Every callee in the binary32 graph uses this same five-way split.
  template<class A> inline constexpr auto exp_target=
    target<A,avx512,kernel_bw,kernel_vl,kernel_base,avx2>;
}

// Ordinal and literal Clang target for each body.
#define SIMD_EXP_TARGETS(X) \
  X(0,avx512) \
  X(1,exp_bw) \
  X(2,exp_vl) \
  X(3,exp_base) \
  X(4,avx2)

#define CHECK_EXP_TARGET(i,name) \
  static_assert(simd::detail::exp_target<SIMD_TARGET_TYPE(name)> == i);
SIMD_EXP_TARGETS(CHECK_EXP_TARGET)
#undef CHECK_EXP_TARGET
