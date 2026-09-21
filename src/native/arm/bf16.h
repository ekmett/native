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
  /// \defgroup arm_bf16 BF16 dot products, matrices and widening multiply-add
  /// Requires arm_feature::neon_bf16 and a compatible bf16 compiler target.
  /// BFDOT/BFMMLA use round-to-odd steps by default, or fused pairs followed by
  /// separate accumulation when FEAT_EBF16 and FPCR.EBF enable enhanced behavior.
  /// They return default NaNs and leave FPSR unchanged in either mode.
  /// BFMLALB/T instead perform single fused binary32 multiply-adds. With AH=0
  /// they use ordinary FP32 controls and accumulate exception flags. With AFP
  /// and AH=1 they force RNE and input/output flushing, suppressing exceptions.
  /// All operations read the caller's FPCR without changing it.
  /// \{

  // Clang 23 CSEs BF16 intrinsics across FPCR writes, even with FENV_ACCESS,
  // and discards unused BFMLAL results and their FPSR effects. Volatile assembly
  // preserves both contracts; the memory clobber orders environment accesses
  // without adding a CPU memory fence.

  /// Accumulate each adjacent pair of BF16 products into the corresponding FP32 lane.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x2_t bfdot(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfdot %0.2s, %1.4h, %2.4h"
      : "+w"(acc) : "w"(a), "w"(source) : "memory");
    return detail::arm_register_order(acc);
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 2)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x2_t bfdot_lane(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_bf16(b, b));
    asm volatile("bfdot %0.2s, %1.4h, %2.2h[%3]"
      : "+w"(acc) : "w"(a), "w"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x2_t bfdot_lane(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfdot %0.2s, %1.4h, %2.2h[%3]"
      : "+w"(acc) : "w"(a), "w"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Accumulate each adjacent pair of BF16 products into the corresponding FP32 lane.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfdot(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfdot %0.4s, %1.8h, %2.8h"
      : "+w"(acc) : "w"(a), "w"(source) : "memory");
    return detail::arm_register_order(acc);
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 2)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfdot_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_bf16(b, b));
    asm volatile("bfdot %0.4s, %1.8h, %2.2h[%3]"
      : "+w"(acc) : "w"(a), "w"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  /// BFDOT with the BF16 pair b[2*Lane], b[2*Lane+1] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfdot_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfdot %0.4s, %1.8h, %2.2h[%3]"
      : "+w"(acc) : "w"(a), "w"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Accumulate a row-major 2x4 matrix times a column-major 4x2 matrix, two BFDOT steps per result.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmmla(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfmmla %0.4s, %1.8h, %2.8h"
      : "+w"(acc) : "w"(a), "w"(source) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Fused multiply-add of the even BF16 lanes into the corresponding FP32 lanes.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalb(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfmlalb %0.4s, %1.8h, %2.8h"
      : "+w"(acc) : "w"(a), "w"(source) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Fused multiply-add of the even lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalb_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_bf16(b, b));
    asm volatile("bfmlalb %0.4s, %1.8h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Fused multiply-add of the even lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalb_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfmlalb %0.4s, %1.8h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Fused multiply-add of the odd BF16 lanes into the corresponding FP32 lanes.
  template<isa Arch>
    requires(Arch.has(arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalt(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfmlalt %0.4s, %1.8h, %2.8h"
      : "+w"(acc) : "w"(a), "w"(source) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Fused multiply-add of the odd lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalt_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(vcombine_bf16(b, b));
    asm volatile("bfmlalt %0.4s, %1.8h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  /// Fused multiply-add of the odd lanes of a with b[Lane] shared by all output lanes.
  template<isa Arch, unsigned Lane>
    requires(Arch.has(arm_feature::neon_bf16) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalt_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    acc = detail::arm_register_order(acc);
    a = detail::arm_register_order(a);
    auto source = detail::arm_register_order(b);
    asm volatile("bfmlalt %0.4s, %1.8h, %2.h[%3]"
      : "+w"(acc) : "w"(a), "x"(source), "i"(Lane) : "memory");
    return detail::arm_register_order(acc);
  }

  // Exact deduction rejects unrelated vectors and invalid immediates before
  // Clang's lax vector conversions can select an overload for another shape.
  /// \cond
  template<isa Arch, class R, class A, class B> void bfdot(R, A, B) = delete;
  template<isa Arch, class R, class A, class B> void bfmmla(R, A, B) = delete;
  template<isa Arch, class R, class A, class B> void bfmlalb(R, A, B) = delete;
  template<isa Arch, class R, class A, class B> void bfmlalt(R, A, B) = delete;
  template<isa Arch, unsigned Lane, class R, class A, class B> void bfdot_lane(R, A, B) = delete;
  template<isa Arch, unsigned Lane, class R, class A, class B> void bfmlalb_lane(R, A, B) = delete;
  template<isa Arch, unsigned Lane, class R, class A, class B> void bfmlalt_lane(R, A, B) = delete;

  /// \endcond
  /// \}
}
#endif
