// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
// This freestanding cross-compile fixture has no target C++ library. Supply only
// the ISA metadata needed to instantiate the actual production instruction header;
// full ISA admission is checked separately by the ordinary module/header fixtures.
namespace native {
  enum class arm_feature { complxnum, neon_fp16, fp16fml };
  template<architecture Family=arm> struct isa {
    unsigned bits;
    constexpr isa(arm_feature f) : bits(1u << unsigned(f)) {}
    constexpr bool has(arm_feature f) const { return bits & (1u << unsigned(f)); }
    constexpr void set(arm_feature f, bool value) {
      if(value) bits |= 1u << unsigned(f);
      else bits &= ~(1u << unsigned(f));
    }
  };
}
