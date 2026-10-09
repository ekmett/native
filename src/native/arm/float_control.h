// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/detail/constexpr_float.h"

namespace native::detail {
  // A value snapshot: constant evaluation uses the established default controls.
  // Runtime reads are ordered with surrounding explicit environment operations.
  struct arm_float_control {
    std::uint64_t bits{};
    static constexpr arm_float_control current() noexcept {
      if consteval { return {}; } else {
        arm_float_control result;
#if NATIVE_HOST_NEON
        asm volatile("mrs %0, fpcr" : "=r"(result.bits) :: "memory");
#endif
        return result;
      }
    }
    constexpr bool alternative() const noexcept { return bits&2; }
    constexpr bool enhanced() const noexcept { return bits&(std::uint64_t{1}<<13); }
    constexpr bool flush_half_inputs() const noexcept { return bits&(std::uint64_t{1}<<19); }
    constexpr constexpr_float::rounding rounding_mode() const noexcept {
      using enum constexpr_float::rounding;
      switch((bits>>22)&3) {
      case 1:return upward;
      case 2:return downward;
      case 3:return toward_zero;
      default:return nearest_even;
      }
    }
    template<class F> constexpr constexpr_float::policy policy() const noexcept {
      namespace cf=constexpr_float;
      bool ah=alternative(),half=std::same_as<F,cf::binary16>;
      bool flush=half?flush_half_inputs():bool(bits&(std::uint64_t{1}<<24));
      cf::policy result;
      result.nan=(bits&(std::uint64_t{1}<<25))?cf::nan_propagation::default_nan:
        (ah?cf::nan_propagation::first:cf::nan_propagation::signaling_first);
      result.fma_order=ah?cf::fma_nan_order::multiplicands_first:cf::fma_nan_order::addend_first;
      result.flush_inputs=half?flush:bool((bits&1) || (!ah && flush));
      result.flush_outputs=flush && !ah;
      result.flush_outputs_after_rounding=flush && ah;
      result.default_nan_negative=ah;
      result.invalid_product_overrides_quiet_addend=!ah;
      return result;
    }
    template<class F> constexpr typename F::bits_type negate(typename F::bits_type value) const noexcept {
      return alternative() && constexpr_float::is_nan<F>(value)?value:
        typename F::bits_type(value^F::sign_mask);
    }
  };
}
