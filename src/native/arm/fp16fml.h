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







  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlal(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlal %0.2s, %1.2h, %2.2h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlal_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlal %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlal_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlal %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlal(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlal %0.4s, %1.4h, %2.4h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlal_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlal %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlal_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlal %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlal2(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlal2 %0.2s, %1.2h, %2.2h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlal2_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlal2 %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlal2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlal2 %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlal2(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlal2 %0.4s, %1.4h, %2.4h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlal2_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlal2 %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlal2_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlal2 %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlsl(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlsl %0.2s, %1.2h, %2.2h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlsl_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlsl %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlsl_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlsl %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlsl(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlsl %0.4s, %1.4h, %2.4h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlsl_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlsl %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlsl_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlsl %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlsl2(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlsl2 %0.2s, %1.2h, %2.2h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlsl2_lane(float32x2_t acc, float16x4_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlsl2 %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x2_t fmlsl2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlsl2 %0.2s, %1.2h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlsl2(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    b = detail::arm_register_order(b);
    asm volatile("fmlsl2 %0.4s, %1.4h, %2.4h"
      : "+w"(acc) : "w"(a), "w"(b) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlsl2_lane(float32x4_t acc, float16x8_t a, float16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_f16(b, b));
    asm volatile("fmlsl2 %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  template<isa Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  float32x4_t fmlsl2_lane(float32x4_t acc, float16x8_t a, float16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("fmlsl2 %0.4s, %1.4h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

}
#endif
/// \endcond
