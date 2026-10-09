// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/arm/float_control.h"

namespace native::detail {
  // BFDOT scalar encoding semantics with FPCR.EBF=AH=0. Each product, pair
  // sum and accumulator sum is a separate round-to-odd operation. Instruction
  // rules force signed subnormal flushing, positive default NaN and infinity
  // on overflow; the caller's rounding and flush controls do not participate.
  constexpr std::uint32_t arm_bfdot_bits(std::uint32_t accumulator,
      std::uint16_t a0,std::uint16_t a1,std::uint16_t b0,std::uint16_t b1,
      arm_float_control control={}) noexcept {
    namespace cf=constexpr_float;
    cf::policy p{.nan=cf::nan_propagation::default_nan,
      .flush_inputs=true,.flush_outputs=true,.odd_overflow_infinity=true};
    p.default_nan_negative=control.alternative();
    if(control.enhanced()) {
      p=control.policy<cf::binary32>();p.nan=cf::nan_propagation::default_nan;
      auto r=control.rounding_mode();
      auto pair=cf::dot2_bits<cf::binary32>(std::uint32_t(a0)<<16,std::uint32_t(a1)<<16,
        std::uint32_t(b0)<<16,std::uint32_t(b1)<<16,r,p);
      return cf::add_bits<cf::binary32>(accumulator,pair,r,p);
    }
    constexpr auto r=cf::rounding::to_odd;
    auto x=cf::mul_bits<cf::binary32>(std::uint32_t(a0)<<16,std::uint32_t(b0)<<16,r,p);
    auto y=cf::mul_bits<cf::binary32>(std::uint32_t(a1)<<16,std::uint32_t(b1)<<16,r,p);
    return cf::add_bits<cf::binary32>(accumulator,cf::add_bits<cf::binary32>(x,y,r,p),r,p);
  }
}
