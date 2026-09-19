// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Include after a named-module declaration; isa.h definitions belong to the GMF.
#include "simd/isa_capability_exports.h"
export namespace simd {
  using ::simd::arch;
  using ::simd::abi_lookup;
  using ::simd::target;
}
// Source-target macro validation also runs in importing translation units.
export namespace simd::detail {
  using ::simd::detail::source_isa;
  using ::simd::detail::known_features;
  using ::simd::detail::x86_features;
  using ::simd::detail::arm_features;
}
