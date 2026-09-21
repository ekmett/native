// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>
// Preserve the instruction oracle's register inputs while exercising the public simd API.
namespace fp16fml_api {
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlal<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlal(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlal<native::feature_closure(Arch)>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlal_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlal_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlal_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlal_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlal_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    return vget_low_f32((native::fmlal_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlal<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlal(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlal<native::feature_closure(Arch)>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlal_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlal_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlal_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlal_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlal_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlal_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlal2<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlal2(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlal2<native::feature_closure(Arch)>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlal2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlal2_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlal2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlal2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlal2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    return vget_low_f32((native::fmlal2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlal2<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlal2(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlal2<native::feature_closure(Arch)>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlal2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlal2_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlal2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlal2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlal2_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlal2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlsl<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlsl(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlsl<native::feature_closure(Arch)>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlsl_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlsl_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlsl_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlsl_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlsl_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    return vget_low_f32((native::fmlsl_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlsl<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlsl(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlsl<native::feature_closure(Arch)>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlsl_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlsl_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlsl_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlsl_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlsl_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlsl_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlsl2<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlsl2(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlsl2<native::feature_closure(Arch)>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlsl2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlsl2_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    return vget_low_f32((native::fmlsl2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlsl2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x2_t fmlsl2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    return vget_low_f32((native::fmlsl2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlsl2<native::feature_closure(Arch)>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlsl2(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlsl2<native::feature_closure(Arch)>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fmlsl2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlsl2_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlsl2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fmlsl2_lane<native::feature_closure(Arch), Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("fp16fml")))
  float32x4_t fmlsl2_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fmlsl2_lane<native::feature_closure(Arch), Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa<native::arm> Arch, class... A>
  void fmlal(A...) = delete;
  template<native::isa<native::arm> Arch, unsigned Lane, class... A>
  void fmlal_lane(A...) = delete;
  template<native::isa<native::arm> Arch, class... A>
  void fmlal2(A...) = delete;
  template<native::isa<native::arm> Arch, unsigned Lane, class... A>
  void fmlal2_lane(A...) = delete;
  template<native::isa<native::arm> Arch, class... A>
  void fmlsl(A...) = delete;
  template<native::isa<native::arm> Arch, unsigned Lane, class... A>
  void fmlsl_lane(A...) = delete;
  template<native::isa<native::arm> Arch, class... A>
  void fmlsl2(A...) = delete;
  template<native::isa<native::arm> Arch, unsigned Lane, class... A>
  void fmlsl2_lane(A...) = delete;
}
