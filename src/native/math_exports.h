// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
export namespace math {
  using ::math::exp;
  using ::math::sin;
  using ::math::cos;
  using ::math::sincos;
  using ::math::flush_to_zero;
  using ::math::abs;
  using ::math::sqrt;
  using ::math::floor;
  using ::math::ceil;
  using ::math::trunc;
  using ::math::round_even;
}
export namespace wide {
  using ::wide::constant_like;
  using ::wide::add;
  using ::wide::sub;
  using ::wide::negate;
  using ::wide::mul;
  using ::wide::div;
  using ::wide::bit_and;
  using ::wide::bit_or;
  using ::wide::bit_xor;
  using ::wide::bit_not;
  using ::wide::cmp_eq;
  using ::wide::cmp_ne;
  using ::wide::cmp_lt;
  using ::wide::cmp_le;
  using ::wide::cmp_gt;
  using ::wide::cmp_ge;
  using ::wide::mask_not;
  using ::wide::min;
  using ::wide::max;
  using ::wide::abs;
  using ::wide::sqrt;
  using ::wide::floor;
  using ::wide::ceil;
  using ::wide::trunc;
  using ::wide::round_even;
  /// Export pointwise fused multiply-add for arrays of SIMD values.
  using ::wide::fma;
  /// Export pointwise lane selection for arrays and shared SIMD operands.
  using ::wide::select;
  using ::wide::scaleb;
  /// Export pointwise masked power-of-two scaling that preserves inactive lanes.
  using ::wide::masked_scaleb;
  /// Export pointwise masked power-of-two scaling that clears inactive lanes.
  using ::wide::masked_scaleb_zero;
  using ::wide::bits;
  using ::wide::from_bits;
  /// Export pointwise conversion of mask lanes to unsigned zero/all-one words.
  using ::wide::mask_bits;
  /// Export compile-time left shifts of each integer lane in an array.
  using ::wide::left;
  using ::wide::exp;
  using ::wide::sin;
  using ::wide::cos;
  using ::wide::sincos;
  using ::wide::flush_to_zero;
}

export namespace native {
  using ::native::sin;
  using ::native::cos;
  using ::native::sincos;
  using ::native::exp;
  using ::native::expm1;
  using ::native::log;
  using ::native::log1p;
  using ::native::tanh;
  namespace math {
    using ::native::abs;
    using ::native::cos;
    using ::native::exp;
    using ::native::expm1;
    using ::native::fma;
    using ::native::log;
    using ::native::log1p;
    using ::native::sin;
    using ::native::sincos;
    using ::native::sqrt;
    using ::native::floor;
    using ::native::ceil;
    using ::native::trunc;
    using ::native::tanh;
  }
}
