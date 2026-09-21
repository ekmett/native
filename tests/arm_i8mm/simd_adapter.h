// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>
// Preserve the instruction oracle's register inputs while exercising the public simd API.
namespace i8mm_api {
  template<native::isa<native::arm> Arch> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::int8_t, 16, native::feature_closure(Arch)> a, native::simd<std::int8_t, 16, native::feature_closure(Arch)> b) { native::smmla<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x4_t smmla(int32x4_t acc, int8x16_t a, int8x16_t b) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::smmla<native::feature_closure(Arch)>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<std::uint32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> a, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> b) { native::ummla<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  uint32x4_t ummla(uint32x4_t acc, uint8x16_t a, uint8x16_t b) noexcept {
    return __builtin_bit_cast(uint32x4_t, (native::ummla<native::feature_closure(Arch)>(native::simd<std::uint32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> a, native::simd<std::int8_t, 16, native::feature_closure(Arch)> b) { native::usmmla<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x4_t usmmla(int32x4_t acc, uint8x16_t a, int8x16_t b) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::usmmla<native::feature_closure(Arch)>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 8, native::feature_closure(Arch)> a, native::simd<std::int8_t, 8, native::feature_closure(Arch)> b) { native::usdot<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x2_t usdot(int32x2_t acc, uint8x8_t a, int8x8_t b) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::usdot<native::feature_closure(Arch)>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(acc, vdup_n_s32(0)))), native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 8, native::feature_closure(Arch)>::native_type, b)))).to_native()));
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 8, native::feature_closure(Arch)> a, native::simd<std::int8_t, 8, native::feature_closure(Arch)> b) { native::usdot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x2_t usdot_lane(int32x2_t acc, uint8x8_t a, int8x8_t b) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::usdot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(acc, vdup_n_s32(0)))), native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 8, native::feature_closure(Arch)>::native_type, b)))).to_native()));
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> acc, native::simd<std::int8_t, 8, native::feature_closure(Arch)> a, native::simd<std::uint8_t, 8, native::feature_closure(Arch)> b) { native::sudot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x2_t sudot_lane(int32x2_t acc, int8x8_t a, uint8x8_t b) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sudot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(acc, vdup_n_s32(0)))), native::simd<std::int8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::native_type, b)))).to_native()));
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 8, native::feature_closure(Arch)> a, native::simd<std::int8_t, 16, native::feature_closure(Arch)> b) { native::usdot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x2_t usdot_lane(int32x2_t acc, uint8x8_t a, int8x16_t b) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::usdot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(acc, vdup_n_s32(0)))), native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native()));
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 2, native::feature_closure(Arch)> acc, native::simd<std::int8_t, 8, native::feature_closure(Arch)> a, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> b) { native::sudot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x2_t sudot_lane(int32x2_t acc, int8x8_t a, uint8x16_t b) noexcept {
    return vget_low_s32(__builtin_bit_cast(int32x4_t, (native::sudot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 2, native::feature_closure(Arch)>::native_type, vcombine_s32(acc, vdup_n_s32(0)))), native::simd<std::int8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native()));
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> a, native::simd<std::int8_t, 16, native::feature_closure(Arch)> b) { native::usdot<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x4_t usdot(int32x4_t acc, uint8x16_t a, int8x16_t b) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::usdot<native::feature_closure(Arch)>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> a, native::simd<std::int8_t, 8, native::feature_closure(Arch)> b) { native::usdot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x4_t usdot_lane(int32x4_t acc, uint8x16_t a, int8x8_t b) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::usdot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::int8_t, 16, native::feature_closure(Arch)> a, native::simd<std::uint8_t, 8, native::feature_closure(Arch)> b) { native::sudot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x4_t sudot_lane(int32x4_t acc, int8x16_t a, uint8x8_t b) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sudot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> a, native::simd<std::int8_t, 16, native::feature_closure(Arch)> b) { native::usdot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x4_t usdot_lane(int32x4_t acc, uint8x16_t a, int8x16_t b) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::usdot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<std::int32_t, 4, native::feature_closure(Arch)> acc, native::simd<std::int8_t, 16, native::feature_closure(Arch)> a, native::simd<std::uint8_t, 16, native::feature_closure(Arch)> b) { native::sudot_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("i8mm")))
  int32x4_t sudot_lane(int32x4_t acc, int8x16_t a, uint8x16_t b) noexcept {
    return __builtin_bit_cast(int32x4_t, (native::sudot_lane<native::feature_closure(Arch), Lane>(native::simd<std::int32_t, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<std::int8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, native::feature_closure(Arch)>::native_type, a)), native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, class... A>
  void smmla(A...) = delete;
  template<native::isa<native::arm> Arch, class... A>
  void ummla(A...) = delete;
  template<native::isa<native::arm> Arch, class... A>
  void usmmla(A...) = delete;
  template<native::isa<native::arm> Arch, class... A>
  void usdot(A...) = delete;
  template<native::isa<native::arm> Arch, unsigned Lane, class... A>
  void usdot_lane(A...) = delete;
  template<native::isa<native::arm> Arch, unsigned Lane, class... A>
  void sudot_lane(A...) = delete;
}
