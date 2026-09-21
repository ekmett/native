// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
/// \cond
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include "native/arm/detail/register_order.h"
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native::detail {









  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x2_t fcadd(float32x2_t a, float32x2_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    float32x2_t result;
    asm volatile("fcadd %0.2s, %1.2s, %2.2s, #%3"
      : "=w"(result) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(result);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x2_t fcmla(float32x2_t acc, float32x2_t a, float32x2_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    acc = detail::arm_register_order(acc);
    asm volatile("fcmla %0.2s, %1.2s, %2.2s, #%3"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 1)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x2_t fcmla_lane(float32x2_t acc, float32x2_t a, float32x2_t b) noexcept {
    return fcmla<Arch, Rotation>(acc, a, b);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x2_t fcmla_lane(float32x2_t acc, float32x2_t a, float32x4_t b) noexcept {
    if constexpr(Lane == 0)
      return fcmla<Arch, Rotation>(acc, a, vget_low_f32(b));
    else
      return fcmla<Arch, Rotation>(acc, a, vget_high_f32(b));
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x4_t fcadd(float32x4_t a, float32x4_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    float32x4_t result;
    asm volatile("fcadd %0.4s, %1.4s, %2.4s, #%3"
      : "=w"(result) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(result);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x4_t fcmla(float32x4_t acc, float32x4_t a, float32x4_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    acc = detail::arm_register_order(acc);
    asm volatile("fcmla %0.4s, %1.4s, %2.4s, #%3"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 1)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x4_t fcmla_lane(float32x4_t acc, float32x4_t a, float32x2_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fcmla %0.4s, %1.4s, %2.s[%3], #%4"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Lane), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float32x4_t fcmla_lane(float32x4_t acc, float32x4_t a, float32x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fcmla %0.4s, %1.4s, %2.s[%3], #%4"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Lane), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float64x2_t fcadd(float64x2_t a, float64x2_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    float64x2_t result;
    asm volatile("fcadd %0.2d, %1.2d, %2.2d, #%3"
      : "=w"(result) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(result);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum")))
  float64x2_t fcmla(float64x2_t acc, float64x2_t a, float64x2_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    acc = detail::arm_register_order(acc);
    asm volatile("fcmla %0.2d, %1.2d, %2.2d, #%3"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x4_t fcadd(float16x4_t a, float16x4_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    float16x4_t result;
    asm volatile("fcadd %0.4h, %1.4h, %2.4h, #%3"
      : "=w"(result) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(result);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x4_t fcmla(float16x4_t acc, float16x4_t a, float16x4_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    acc = detail::arm_register_order(acc);
    asm volatile("fcmla %0.4h, %1.4h, %2.4h, #%3"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x4_t fcmla_lane(float16x4_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fcmla %0.4h, %1.4h, %2.h[%3], #%4"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Lane), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 4)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x4_t fcmla_lane(float16x4_t acc, float16x4_t a, float16x8_t b) noexcept {
    // The 64-bit half form encodes only the low two complex pairs.
    if constexpr(Lane >= 2)
      return fcmla_lane<Arch, Rotation, Lane - 2>(acc, a, vget_high_f16(b));
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fcmla %0.4h, %1.4h, %2.h[%3], #%4"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Lane % 2), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 90 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x8_t fcadd(float16x8_t a, float16x8_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    float16x8_t result;
    asm volatile("fcadd %0.8h, %1.8h, %2.8h, #%3"
      : "=w"(result) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(result);
  }

  template<isa Arch, unsigned Rotation> requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
    && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270))
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x8_t fcmla(float16x8_t acc, float16x8_t a, float16x8_t b) noexcept {
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    acc = detail::arm_register_order(acc);
    asm volatile("fcmla %0.8h, %1.8h, %2.8h, #%3"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x8_t fcmla_lane(float16x8_t acc, float16x8_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fcmla %0.8h, %1.8h, %2.h[%3], #%4"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Lane), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 4)
  native_nodiscard native_inline __attribute__((target("complxnum,fullfp16")))
  float16x8_t fcmla_lane(float16x8_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fcmla %0.8h, %1.8h, %2.h[%3], #%4"
      : "+w"(acc) : "w"(a), "w"(b), "i"(Lane), "i"(Rotation) : "memory");
    return detail::arm_register_order(acc);
  }

  // Clang permits implicit same-size NEON vector conversions. Exact deleted
  // overloads keep missing features and invalid immediates from selecting
  // an overload of another element format through those conversions.
  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 90 || Rotation == 270)))
  float32x2_t fcadd(float32x2_t, float32x2_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)))
  float32x2_t fcmla(float32x2_t, float32x2_t, float32x2_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 1))
  float32x2_t fcmla_lane(float32x2_t, float32x2_t, float32x2_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2))
  float32x2_t fcmla_lane(float32x2_t, float32x2_t, float32x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 90 || Rotation == 270)))
  float32x4_t fcadd(float32x4_t, float32x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)))
  float32x4_t fcmla(float32x4_t, float32x4_t, float32x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 1))
  float32x4_t fcmla_lane(float32x4_t, float32x4_t, float32x2_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2))
  float32x4_t fcmla_lane(float32x4_t, float32x4_t, float32x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 90 || Rotation == 270)))
  float64x2_t fcadd(float64x2_t, float64x2_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)))
  float64x2_t fcmla(float64x2_t, float64x2_t, float64x2_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 90 || Rotation == 270)))
  float16x4_t fcadd(float16x4_t, float16x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)))
  float16x4_t fcmla(float16x4_t, float16x4_t, float16x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2))
  float16x4_t fcmla_lane(float16x4_t, float16x4_t, float16x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 4))
  float16x4_t fcmla_lane(float16x4_t, float16x4_t, float16x8_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 90 || Rotation == 270)))
  float16x8_t fcadd(float16x8_t, float16x8_t) noexcept = delete;

  template<isa Arch, unsigned Rotation>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)))
  float16x8_t fcmla(float16x8_t, float16x8_t, float16x8_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 2))
  float16x8_t fcmla_lane(float16x8_t, float16x8_t, float16x4_t) noexcept = delete;

  template<isa Arch, unsigned Rotation, unsigned Lane>
    requires(!(Arch.has(arm_feature::complxnum) && Arch.has(arm_feature::neon_fp16)
      && (Rotation == 0 || Rotation == 90 || Rotation == 180 || Rotation == 270)
      && Lane < 4))
  float16x8_t fcmla_lane(float16x8_t, float16x8_t, float16x8_t) noexcept = delete;

}
#endif
/// \endcond
