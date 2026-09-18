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
}
#define EXP_POLICY_CELLS(X) SIMD_EXP_POLICY_CELLS(X)
