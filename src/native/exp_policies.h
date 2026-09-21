// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <native/targets.h>
#include "native/kernel_policies.h"

// Clang attributes require literal target strings.
#define NATIVE_TARGET_exp_base NATIVE_KERNEL_TARGET_2
#define NATIVE_TARGET_exp_bw NATIVE_KERNEL_TARGET_3
#define NATIVE_TARGET_exp_vl NATIVE_KERNEL_TARGET_4

namespace native::detail {
  // Every callee in the binary32 graph uses this same five-way split.
  template<isa<x86> A> inline constexpr auto exp_target=
    target<A,avx512,kernel_bw,kernel_vl,kernel_base,avx2>;
}

// Ordinal and literal Clang target for each body.
#define NATIVE_EXP_TARGETS(X) \
  X(0,avx512) \
  X(1,exp_bw) \
  X(2,exp_vl) \
  X(3,exp_base) \
  X(4,avx2)

#define CHECK_EXP_TARGET(i,name) \
  static_assert(::native::detail::exp_target<::native::target_features<::native::x86>(NATIVE_TARGET_STRING(name))> == i);
NATIVE_EXP_TARGETS(CHECK_EXP_TARGET)
#undef CHECK_EXP_TARGET
