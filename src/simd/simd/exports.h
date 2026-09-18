#pragma once
// Shared export declarations, included only below a named-module declaration.
// Primitive and array-kernel definitions are in the GMF; wide is imported.
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

export namespace simd {
  using std::int8_t;
  using std::int16_t;
  using std::int32_t;
  using std::int64_t;
  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;
  using ::simd::simd_traits;
  using ::simd::simd_customization;
  using ::simd::simd_custom_element;
  using ::simd::simd_integer_element;
  using ::simd::imm_t;
  using ::simd::imm;
  using ::simd::simd_access;
  using ::simd::simd_memory;
  using ::simd::load_simd;
  using ::simd::load_simd_partial;
  using ::simd::store_simd;
  using ::simd::store_simd_partial;
  using ::simd::compaction_result;
  using ::simd::compress;
  using ::simd::expand;
  using ::simd::compress_store;
}

export namespace simd {
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator+;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator-;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator*;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator/;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator%;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator&;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator|;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator^;
  using ::simd::operator<<;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator>>;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator!;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator~;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator==;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator!=;
  using ::simd::operator<;
  using ::simd::operator<=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator>;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator>=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator+=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator-=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator*=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator/=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator%=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator&=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator|=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator^=;
  using ::simd::operator<<=;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::operator>>=;

  using ::simd::wide;
  using ::simd::vec;
  using ::simd::scalar;
  using ::simd::avx2;
  using ::simd::avx512;
  using ::simd::avx512_bf16;
  using ::simd::neon;
  using ::simd::architecture;
  using std::int8_t;
  using std::int16_t;
  using std::int32_t;
  using std::int64_t;
  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;
  using ::simd::simd_integer_element;
  using ::simd::imm_t;
  using ::simd::imm;
  using ::simd::simd_access;
  using ::simd::simd_memory;
  using ::simd::mask_lane;
  using ::simd::mask8;
  using ::simd::mask16;
  using ::simd::mask32;
  using ::simd::mask64;
  using ::simd::simd_mask_element;
  using ::simd::flush_to_zero;
  using ::simd::convert;
  using ::simd::reinterpret_bits;
  using ::simd::popcount;
  using ::simd::pairwise_add_widened;
  using ::simd::reduce_add_widened;
  using ::simd::mask_bits;
  using ::simd::mask_cast;
  using ::simd::to_predicate;
  using ::simd::to_vector_mask;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::bit_select;
  using ::simd::select;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::masked_add;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::masked_sub;
  /// Make the corresponding simd overload set visible through this module.
  using ::simd::masked_mul;
  using ::simd::masked_scaleb;
  using ::simd::masked_scaleb_zero;
  using ::simd::scaleb;
  using ::simd::fma;
  using ::simd::broadcast;
  using ::simd::abs;
  using ::simd::sqrt;
  using ::simd::floor;
  using ::simd::ceil;
  using ::simd::trunc;
}

export namespace simd {
  using ::simd::sin;
  using ::simd::cos;
  using ::simd::sincos;
  using ::simd::exp;
  using ::simd::expm1;
  using ::simd::log;
  using ::simd::log1p;
  using ::simd::tanh;
  namespace math {
    using ::simd::abs;
    using ::simd::cos;
    using ::simd::exp;
    using ::simd::expm1;
    using ::simd::fma;
    using ::simd::log;
    using ::simd::log1p;
    using ::simd::sin;
    using ::simd::sincos;
    using ::simd::sqrt;
    using ::simd::floor;
    using ::simd::ceil;
    using ::simd::trunc;
    using ::simd::tanh;
  }
}

export namespace simd {
  using ::simd::deposit_bits;
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON
  using ::simd::narrow_concat;
#endif
}
