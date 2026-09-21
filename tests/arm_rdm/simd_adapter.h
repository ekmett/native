// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>
// Preserve the instruction oracle's register inputs while exercising the public simd API.
namespace rdm_api {
  template<native::isa Arch> requires requires(int16_t accumulator, int16_t lhs, int16_t rhs) { native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16_t sqrdmlah(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    return native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs);
  }
  template<native::isa Arch, int Lane> requires requires(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, int16x4_t rhs) noexcept {
    return native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)));
  }
  template<native::isa Arch, int Lane> requires requires(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, int16x8_t rhs) noexcept {
    return native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)));
  }
  template<native::isa Arch> requires requires(native::simd<std::int16_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x4_t sqrdmlah(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    return __builtin_bit_cast(int16x4_t, (native::sqrdmlah<native::feature_closure(Arch)>(native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x4_t sqrdmlah_lane(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    return __builtin_bit_cast(int16x4_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x4_t sqrdmlah_lane(int16x4_t accumulator, int16x4_t lhs, int16x8_t rhs) noexcept {
    return __builtin_bit_cast(int16x4_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch> requires requires(native::simd<std::int16_t, 8, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 8, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x8_t sqrdmlah(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    return __builtin_bit_cast(int16x8_t, (native::sqrdmlah<native::feature_closure(Arch)>(native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 8, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 8, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x8_t sqrdmlah_lane(int16x8_t accumulator, int16x8_t lhs, int16x4_t rhs) noexcept {
    return __builtin_bit_cast(int16x8_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 8, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 8, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x8_t sqrdmlah_lane(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    return __builtin_bit_cast(int16x8_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch> requires requires(int32_t accumulator, int32_t lhs, int32_t rhs) { native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32_t sqrdmlah(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    return native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs);
  }
  template<native::isa Arch, int Lane> requires requires(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, int32x2_t rhs) noexcept {
    return native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))));
  }
  template<native::isa Arch, int Lane> requires requires(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, int32x4_t rhs) noexcept {
    return native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)));
  }
  template<native::isa Arch> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 2, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x2_t sqrdmlah(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sqrdmlah<native::feature_closure(Arch)>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(accumulator, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(lhs, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))))).to_native()));
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 2, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x2_t sqrdmlah_lane(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(accumulator, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(lhs, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))))).to_native()));
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 2, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x2_t sqrdmlah_lane(int32x2_t accumulator, int32x2_t lhs, int32x4_t rhs) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(accumulator, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(lhs, vdup_n_s32(0)))), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native()));
  }
  template<native::isa Arch> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x4_t sqrdmlah(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sqrdmlah<native::feature_closure(Arch)>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x4_t sqrdmlah_lane(int32x4_t accumulator, int32x4_t lhs, int32x2_t rhs) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x4_t sqrdmlah_lane(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sqrdmlah_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch> requires requires(int16_t accumulator, int16_t lhs, int16_t rhs) { native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16_t sqrdmlsh(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    return native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs);
  }
  template<native::isa Arch, int Lane> requires requires(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, int16x4_t rhs) noexcept {
    return native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)));
  }
  template<native::isa Arch, int Lane> requires requires(int16_t accumulator, int16_t lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, int16x8_t rhs) noexcept {
    return native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)));
  }
  template<native::isa Arch> requires requires(native::simd<std::int16_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x4_t sqrdmlsh(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    return __builtin_bit_cast(int16x4_t, (native::sqrdmlsh<native::feature_closure(Arch)>(native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x4_t sqrdmlsh_lane(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    return __builtin_bit_cast(int16x4_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x4_t sqrdmlsh_lane(int16x4_t accumulator, int16x4_t lhs, int16x8_t rhs) noexcept {
    return __builtin_bit_cast(int16x4_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch> requires requires(native::simd<std::int16_t, 8, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 8, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x8_t sqrdmlsh(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    return __builtin_bit_cast(int16x8_t, (native::sqrdmlsh<native::feature_closure(Arch)>(native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 8, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 8, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x8_t sqrdmlsh_lane(int16x8_t accumulator, int16x8_t lhs, int16x4_t rhs) noexcept {
    return __builtin_bit_cast(int16x8_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int16_t, 8, native::feature_closure(Arch)> accumulator, native::simd<std::int16_t, 8, native::feature_closure(Arch)> lhs, native::simd<std::int16_t, 8, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int16x8_t sqrdmlsh_lane(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    return __builtin_bit_cast(int16x8_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int16_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch> requires requires(int32_t accumulator, int32_t lhs, int32_t rhs) { native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32_t sqrdmlsh(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    return native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs);
  }
  template<native::isa Arch, int Lane> requires requires(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, int32x2_t rhs) noexcept {
    return native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))));
  }
  template<native::isa Arch, int Lane> requires requires(int32_t accumulator, int32_t lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, int32x4_t rhs) noexcept {
    return native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)));
  }
  template<native::isa Arch> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 2, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x2_t sqrdmlsh(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sqrdmlsh<native::feature_closure(Arch)>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(accumulator, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(lhs, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))))).to_native()));
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 2, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x2_t sqrdmlsh_lane(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(accumulator, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(lhs, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))))).to_native()));
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 2, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x2_t sqrdmlsh_lane(int32x2_t accumulator, int32x2_t lhs, int32x4_t rhs) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(accumulator, vdup_n_s32(0)))), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(lhs, vdup_n_s32(0)))), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native()));
  }
  template<native::isa Arch> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh<native::feature_closure(Arch)>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x4_t sqrdmlsh(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sqrdmlsh<native::feature_closure(Arch)>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 2, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x4_t sqrdmlsh_lane(int32x4_t accumulator, int32x4_t lhs, int32x2_t rhs) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(rhs, vdup_n_s32(0)))))).to_native());
  }
  template<native::isa Arch, int Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> accumulator, native::simd<std::int32_t, 4, native::feature_closure(Arch)> lhs, native::simd<std::int32_t, 4, native::feature_closure(Arch)> rhs) { native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(accumulator, lhs, rhs); }
  inline __attribute__((always_inline, target("rdm")))
  int32x4_t sqrdmlsh_lane(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sqrdmlsh_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, accumulator)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, lhs)), native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, rhs)))).to_native());
  }
  template<native::isa Arch, class... A>
  void sqrdmlah(A...) = delete;
  template<native::isa Arch, int Lane, class... A>
  void sqrdmlah_lane(A...) = delete;
  template<native::isa Arch, class... A>
  void sqrdmlsh(A...) = delete;
  template<native::isa Arch, int Lane, class... A>
  void sqrdmlsh_lane(A...) = delete;
}
