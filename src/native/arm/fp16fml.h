// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include "native/arm/detail/register_order.h"
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
  /// \defgroup arm_fp16fml FP16 widening fused multiply-add
  /// Requires arm_feature::fp16fml (FEAT_FHM), independently of FCMA/BF16.
  /// Binary16 inputs accumulate directly into binary32, with one rounding.
  /// FPCR controls and FPSR effects are those of the instruction; neither is reset.
  /// Volatile assembly retains status effects even when the result is unused.
  /// \{

  /// Add products from the low 2 half lanes of a and b.
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

  /// FMLAL with b[Lane] broadcast; selects the low 2 lanes of a.
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

  /// FMLAL with b[Lane] broadcast; selects the low 2 lanes of a.
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

  /// Add products from the low 4 half lanes of a and b.
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

  /// FMLAL with b[Lane] broadcast; selects the low 4 lanes of a.
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

  /// FMLAL with b[Lane] broadcast; selects the low 4 lanes of a.
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

  /// Add products from the high 2 half lanes of a and b.
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

  /// FMLAL2 with b[Lane] broadcast; selects the high 2 lanes of a.
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

  /// FMLAL2 with b[Lane] broadcast; selects the high 2 lanes of a.
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

  /// Add products from the high 4 half lanes of a and b.
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

  /// FMLAL2 with b[Lane] broadcast; selects the high 4 lanes of a.
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

  /// FMLAL2 with b[Lane] broadcast; selects the high 4 lanes of a.
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

  /// Subtract products from the low 2 half lanes of a and b.
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

  /// FMLSL with b[Lane] broadcast; selects the low 2 lanes of a.
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

  /// FMLSL with b[Lane] broadcast; selects the low 2 lanes of a.
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

  /// Subtract products from the low 4 half lanes of a and b.
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

  /// FMLSL with b[Lane] broadcast; selects the low 4 lanes of a.
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

  /// FMLSL with b[Lane] broadcast; selects the low 4 lanes of a.
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

  /// Subtract products from the high 2 half lanes of a and b.
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

  /// FMLSL2 with b[Lane] broadcast; selects the high 2 lanes of a.
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

  /// FMLSL2 with b[Lane] broadcast; selects the high 2 lanes of a.
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

  /// Subtract products from the high 4 half lanes of a and b.
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

  /// FMLSL2 with b[Lane] broadcast; selects the high 4 lanes of a.
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

  /// FMLSL2 with b[Lane] broadcast; selects the high 4 lanes of a.
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

  /// \}
}
#endif
