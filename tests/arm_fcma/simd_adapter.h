// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>
// Preserve the instruction oracle's register inputs while exercising the public simd API.
namespace fcma_api {
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<float, 2, native::feature_closure(Arch)> a, native::simd<float, 2, native::feature_closure(Arch)> b) { native::fcadd<native::feature_closure(Arch), Rotation>(a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x2_t fcadd(float32x2_t a, float32x2_t b) noexcept {
    return vget_low_f32((native::fcadd<native::feature_closure(Arch), Rotation>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(a, vdup_n_f32(0.f))), native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(b, vdup_n_f32(0.f))))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<float, 2, native::feature_closure(Arch)> a, native::simd<float, 2, native::feature_closure(Arch)> b) { native::fcmla<native::feature_closure(Arch), Rotation>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x2_t fcmla(float32x2_t acc, float32x2_t a, float32x2_t b) noexcept {
    return vget_low_f32((native::fcmla<native::feature_closure(Arch), Rotation>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(a, vdup_n_f32(0.f))), native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(b, vdup_n_f32(0.f))))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<float, 2, native::feature_closure(Arch)> a, native::simd<float, 2, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x2_t fcmla_lane(float32x2_t acc, float32x2_t a, float32x2_t b) noexcept {
    return vget_low_f32((native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(a, vdup_n_f32(0.f))), native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(b, vdup_n_f32(0.f))))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<float, 2, native::feature_closure(Arch)> acc, native::simd<float, 2, native::feature_closure(Arch)> a, native::simd<float, 4, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x2_t fcmla_lane(float32x2_t acc, float32x2_t a, float32x4_t b) noexcept {
    return vget_low_f32((native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(acc, vdup_n_f32(0.f))), native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(a, vdup_n_f32(0.f))), native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<float, 4, native::feature_closure(Arch)> a, native::simd<float, 4, native::feature_closure(Arch)> b) { native::fcadd<native::feature_closure(Arch), Rotation>(a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x4_t fcadd(float32x4_t a, float32x4_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fcadd<native::feature_closure(Arch), Rotation>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<float, 4, native::feature_closure(Arch)> a, native::simd<float, 4, native::feature_closure(Arch)> b) { native::fcmla<native::feature_closure(Arch), Rotation>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x4_t fcmla(float32x4_t acc, float32x4_t a, float32x4_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fcmla<native::feature_closure(Arch), Rotation>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<float, 4, native::feature_closure(Arch)> a, native::simd<float, 2, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x4_t fcmla_lane(float32x4_t acc, float32x4_t a, float32x2_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<float, 2, native::feature_closure(Arch)>::from_native(vcombine_f32(b, vdup_n_f32(0.f))))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<float, 4, native::feature_closure(Arch)> acc, native::simd<float, 4, native::feature_closure(Arch)> a, native::simd<float, 4, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float32x4_t fcmla_lane(float32x4_t acc, float32x4_t a, float32x4_t b) noexcept {
    return __builtin_bit_cast(float32x4_t, (native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<float, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<float, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<double, 2, native::feature_closure(Arch)> a, native::simd<double, 2, native::feature_closure(Arch)> b) { native::fcadd<native::feature_closure(Arch), Rotation>(a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float64x2_t fcadd(float64x2_t a, float64x2_t b) noexcept {
    return __builtin_bit_cast(float64x2_t, (native::fcadd<native::feature_closure(Arch), Rotation>(native::simd<double, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<double, 2, native::feature_closure(Arch)>::native_type, a)), native::simd<double, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<double, 2, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<double, 2, native::feature_closure(Arch)> acc, native::simd<double, 2, native::feature_closure(Arch)> a, native::simd<double, 2, native::feature_closure(Arch)> b) { native::fcmla<native::feature_closure(Arch), Rotation>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum")))
  float64x2_t fcmla(float64x2_t acc, float64x2_t a, float64x2_t b) noexcept {
    return __builtin_bit_cast(float64x2_t, (native::fcmla<native::feature_closure(Arch), Rotation>(native::simd<double, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<double, 2, native::feature_closure(Arch)>::native_type, acc)), native::simd<double, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<double, 2, native::feature_closure(Arch)>::native_type, a)), native::simd<double, 2, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<double, 2, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fcadd<native::feature_closure(Arch), Rotation>(a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x4_t fcadd(float16x4_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float16x4_t, (native::fcadd<native::feature_closure(Arch), Rotation>(native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<native::fp16, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fcmla<native::feature_closure(Arch), Rotation>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x4_t fcmla(float16x4_t acc, float16x4_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float16x4_t, (native::fcmla<native::feature_closure(Arch), Rotation>(native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<native::fp16, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x4_t fcmla_lane(float16x4_t acc, float16x4_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float16x4_t, (native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<native::fp16, 4, native::feature_closure(Arch)> acc, native::simd<native::fp16, 4, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x4_t fcmla_lane(float16x4_t acc, float16x4_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float16x4_t, (native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fcadd<native::feature_closure(Arch), Rotation>(a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x8_t fcadd(float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float16x8_t, (native::fcadd<native::feature_closure(Arch), Rotation>(native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation> requires requires(native::simd<native::fp16, 8, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fcmla<native::feature_closure(Arch), Rotation>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x8_t fcmla(float16x8_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float16x8_t, (native::fcmla<native::feature_closure(Arch), Rotation>(native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<native::fp16, 8, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 4, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x8_t fcmla_lane(float16x8_t acc, float16x8_t a, float16x4_t b) noexcept {
    return __builtin_bit_cast(float16x8_t, (native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 4, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 4, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, unsigned Lane> requires requires(native::simd<native::fp16, 8, native::feature_closure(Arch)> acc, native::simd<native::fp16, 8, native::feature_closure(Arch)> a, native::simd<native::fp16, 8, native::feature_closure(Arch)> b) { native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(acc, a, b); }
  inline __attribute__((always_inline, target("complxnum,fullfp16")))
  float16x8_t fcmla_lane(float16x8_t acc, float16x8_t a, float16x8_t b) noexcept {
    return __builtin_bit_cast(float16x8_t, (native::fcmla_lane<native::feature_closure(Arch), Rotation, Lane>(native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, acc)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, a)), native::simd<native::fp16, 8, native::feature_closure(Arch)>::from_native(__builtin_bit_cast(typename native::simd<native::fp16, 8, native::feature_closure(Arch)>::native_type, b)))).to_native());
  }
  template<native::isa Arch, unsigned Rotation, class... A>
  void fcadd(A...) = delete;
  template<native::isa Arch, unsigned Rotation, class... A>
  void fcmla(A...) = delete;
  template<native::isa Arch, unsigned Rotation, unsigned Lane, class... A>
  void fcmla_lane(A...) = delete;
}
