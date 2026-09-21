// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <native/targets.h>

// Include only in native.simd's global module fragment. The default belongs to
// this module provider, not to each translation unit including the raw headers.
namespace native {
  /// Omitted architecture arguments use the native.simd provider's baseline.
  template<class T, std::size_t N, isa Arch = NATIVE_BASELINE> struct simd;
}
