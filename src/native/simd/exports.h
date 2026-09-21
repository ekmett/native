#pragma once
// Shared export declarations, included only below a named-module declaration.
// Primitive and array-kernel definitions are in the GMF; wide is imported.
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

export namespace native {
  using std::int8_t;
  using std::int16_t;
  using std::int32_t;
  using std::int64_t;
  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;
  using ::native::simd_traits;
  using ::native::simd_customization;
  using ::native::simd_custom_element;
  using ::native::simd_integer_element;
  using ::native::imm_t;
  using ::native::imm;
  using ::native::simd_access;
  using ::native::simd_memory;
  using ::native::load_simd;
  using ::native::load_simd_partial;
  using ::native::store_simd;
  using ::native::store_simd_partial;
  using ::native::compaction_result;
  using ::native::compress;
  using ::native::expand;
  using ::native::compress_store;
}

export namespace native {
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator+;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator-;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator*;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator/;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator%;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator&;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator|;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator^;
  using ::native::operator<<;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator>>;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator!;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator~;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator==;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator!=;
  using ::native::operator<;
  using ::native::operator<=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator>;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator>=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator+=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator-=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator*=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator/=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator%=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator&=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator|=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator^=;
  using ::native::operator<<=;
  /// Make the corresponding native overload set visible through this module.
  using ::native::operator>>=;

  using ::native::wide;
  using ::native::simd;
  using ::native::predicate;
  using ::native::scalar;
  using ::native::avx2;
  using ::native::avx512;
  using ::native::avx512_bf16;
  using ::native::avx512_fp16;
  using ::native::neon;
  using ::native::neon_fp16;
  using ::native::neon_bf16;
  using ::native::isa;
  using ::native::arch;
  using std::int8_t;
  using std::int16_t;
  using std::int32_t;
  using std::int64_t;
  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;
  using ::native::simd_integer_element;
  using ::native::imm_t;
  using ::native::imm;
  using ::native::simd_access;
  using ::native::simd_memory;
  using ::native::mask_lane;
  using ::native::mask8;
  using ::native::mask16;
  using ::native::mask32;
  using ::native::mask64;
  using ::native::simd_mask_element;
  using ::native::flush_to_zero;
  using ::native::convert;
  using ::native::reinterpret_bits;
  using ::native::popcount;
  using ::native::pairwise_add_widened;
  using ::native::reduce_add_widened;
  using ::native::mask_bits;
  using ::native::mask_cast;
  using ::native::to_bool;
  using ::native::to_predicate;
  using ::native::to_vector_mask;
  /// Make the corresponding native overload set visible through this module.
  using ::native::bit_select;
  using ::native::select;
  /// Make the corresponding native overload set visible through this module.
  using ::native::masked_add;
  using ::native::masked_add_zero;
  /// Make the corresponding native overload set visible through this module.
  using ::native::masked_sub;
  using ::native::masked_sub_zero;
  /// Make the corresponding native overload set visible through this module.
  using ::native::masked_mul;
  using ::native::masked_mul_zero;
  using ::native::masked_scaleb;
  using ::native::masked_scaleb_zero;
  using ::native::scaleb;
  using ::native::fma;
  using ::native::broadcast;
  using ::native::abs;
  using ::native::sqrt;
  using ::native::floor;
  using ::native::ceil;
  using ::native::trunc;
}


export namespace native {
#if (NATIVE_HOST_X86 || NATIVE_HOST_NEON || NATIVE_HOST_WASM) && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
  using ::native::narrow_concat;
#endif
}

#if NATIVE_HOST_WASM && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
export namespace native {
  using ::native::shuffle;
  using ::native::swizzle;
  using ::native::round_even;
  using ::native::add_sat;
  using ::native::sub_sat;
  using ::native::average_round;
  using ::native::min;
  using ::native::max;
  using ::native::pmin;
  using ::native::pmax;
  using ::native::extend_low;
  using ::native::extend_high;
  using ::native::multiply_widened_low;
  using ::native::multiply_widened_high;
  using ::native::bitmask;
  using ::native::any;
  using ::native::all;
  using ::native::narrow_sat;
  using ::native::q15mulr_sat;
  using ::native::dot;
  using ::native::trunc_sat;
  using ::native::load_splat;
  using ::native::load_zero;
  using ::native::load_lane;
  using ::native::store_lane;
  using ::native::load_widened;
}
#endif
