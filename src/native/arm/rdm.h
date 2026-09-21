// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
/// \cond
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)

namespace native::detail {
  // Clang lowers 128-bit inline-asm operands through byte vectors. On a
  // big-endian target that changes byte order within 16/32-bit lanes.
  // Its 64-bit/scalar register operands already preserve the required bits.
  template<class V> native_inline V rdm_register_order(V value) noexcept {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    if constexpr (requires { value[0]; }) {
      if constexpr (sizeof(V) == 16) {
        auto bytes = __builtin_bit_cast(int8x16_t, value);
        if constexpr (sizeof(value[0]) == 2)
          return __builtin_bit_cast(V, __builtin_shufflevector(bytes, bytes,
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14));
        else
          return __builtin_bit_cast(V, __builtin_shufflevector(bytes, bytes,
            3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12));
      }
    }
#endif
    return value;
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlah(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlah %h0, %h1, %h2"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    // The restricted V0-V15 constraint accepts a full 128-bit register.
    auto source = detail::rdm_register_order(
      __builtin_shufflevector(right, right, 0, 1, 2, 3, -1, -1, -1, -1));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %h0, %h1, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(source), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlah_lane(int16_t accumulator, int16_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 7 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %h0, %h1, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x4_t sqrdmlah(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlah %0.4h, %1.4h, %2.4h"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x4_t sqrdmlah_lane(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    // The restricted V0-V15 constraint accepts a full 128-bit register.
    auto source = detail::rdm_register_order(
      __builtin_shufflevector(right, right, 0, 1, 2, 3, -1, -1, -1, -1));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %0.4h, %1.4h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(source), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x4_t sqrdmlah_lane(int16x4_t accumulator, int16x4_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 7 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %0.4h, %1.4h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x8_t sqrdmlah(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlah %0.8h, %1.8h, %2.8h"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x8_t sqrdmlah_lane(int16x8_t accumulator, int16x8_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    // The restricted V0-V15 constraint accepts a full 128-bit register.
    auto source = detail::rdm_register_order(
      __builtin_shufflevector(right, right, 0, 1, 2, 3, -1, -1, -1, -1));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %0.8h, %1.8h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(source), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x8_t sqrdmlah_lane(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 7 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %0.8h, %1.8h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlah(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlah %s0, %s1, %s2"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    constexpr int index = Lane;
    asm volatile("sqrdmlah %s0, %s1, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlah_lane(int32_t accumulator, int32_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %s0, %s1, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x2_t sqrdmlah(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlah %0.2s, %1.2s, %2.2s"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x2_t sqrdmlah_lane(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    constexpr int index = Lane;
    asm volatile("sqrdmlah %0.2s, %1.2s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x2_t sqrdmlah_lane(int32x2_t accumulator, int32x2_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %0.2s, %1.2s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x4_t sqrdmlah(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlah %0.4s, %1.4s, %2.4s"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x4_t sqrdmlah_lane(int32x4_t accumulator, int32x4_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    constexpr int index = Lane;
    asm volatile("sqrdmlah %0.4s, %1.4s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x4_t sqrdmlah_lane(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlah %0.4s, %1.4s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlsh(int16_t accumulator, int16_t lhs, int16_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlsh %h0, %h1, %h2"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    // The restricted V0-V15 constraint accepts a full 128-bit register.
    auto source = detail::rdm_register_order(
      __builtin_shufflevector(right, right, 0, 1, 2, 3, -1, -1, -1, -1));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %h0, %h1, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(source), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16_t sqrdmlsh_lane(int16_t accumulator, int16_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 7 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %h0, %h1, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x4_t sqrdmlsh(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlsh %0.4h, %1.4h, %2.4h"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x4_t sqrdmlsh_lane(int16x4_t accumulator, int16x4_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    // The restricted V0-V15 constraint accepts a full 128-bit register.
    auto source = detail::rdm_register_order(
      __builtin_shufflevector(right, right, 0, 1, 2, 3, -1, -1, -1, -1));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %0.4h, %1.4h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(source), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x4_t sqrdmlsh_lane(int16x4_t accumulator, int16x4_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 7 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %0.4h, %1.4h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x8_t sqrdmlsh(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlsh %0.8h, %1.8h, %2.8h"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x8_t sqrdmlsh_lane(int16x8_t accumulator, int16x8_t lhs, int16x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    // The restricted V0-V15 constraint accepts a full 128-bit register.
    auto source = detail::rdm_register_order(
      __builtin_shufflevector(right, right, 0, 1, 2, 3, -1, -1, -1, -1));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %0.8h, %1.8h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(source), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 8)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int16x8_t sqrdmlsh_lane(int16x8_t accumulator, int16x8_t lhs, int16x8_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 7 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %0.8h, %1.8h, %2.h[%c3]"
      : "+w"(result) : "w"(left), "x"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlsh(int32_t accumulator, int32_t lhs, int32_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlsh %s0, %s1, %s2"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    constexpr int index = Lane;
    asm volatile("sqrdmlsh %s0, %s1, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32_t sqrdmlsh_lane(int32_t accumulator, int32_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %s0, %s1, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x2_t sqrdmlsh(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlsh %0.2s, %1.2s, %2.2s"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x2_t sqrdmlsh_lane(int32x2_t accumulator, int32x2_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    constexpr int index = Lane;
    asm volatile("sqrdmlsh %0.2s, %1.2s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x2_t sqrdmlsh_lane(int32x2_t accumulator, int32x2_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %0.2s, %1.2s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch> requires(Arch.has(arm_feature::rdm))
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x4_t sqrdmlsh(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    asm volatile("sqrdmlsh %0.4s, %1.4s, %2.4s"
      : "+w"(result) : "w"(left), "w"(right));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 2)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x4_t sqrdmlsh_lane(int32x4_t accumulator, int32x4_t lhs, int32x2_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
    constexpr int index = Lane;
    asm volatile("sqrdmlsh %0.4s, %1.4s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  template<isa<arm> Arch, int Lane> requires(Arch.has(arm_feature::rdm) && Lane >= 0 && Lane < 4)
  native_nodiscard native_inline __attribute__((target("rdm")))
  int32x4_t sqrdmlsh_lane(int32x4_t accumulator, int32x4_t lhs, int32x4_t rhs) noexcept {
    auto result = detail::rdm_register_order(accumulator);
    auto left = detail::rdm_register_order(lhs);
    auto right = detail::rdm_register_order(rhs);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr int index = 3 - Lane;
#else
    constexpr int index = Lane;
#endif
    asm volatile("sqrdmlsh %0.4s, %1.4s, %2.s[%c3]"
      : "+w"(result) : "w"(left), "w"(right), "i"(index));
    return detail::rdm_register_order(result);
  }

  // Reject Clang's lax vector conversions and scalar narrowing when an exact
  // instruction shape or immediate lane is unavailable.

  template<isa<arm> Arch, class A, class B, class C>
  void sqrdmlah(A, B, C) = delete;
  template<isa<arm> Arch, int Lane, class A, class B, class C>
  void sqrdmlah_lane(A, B, C) = delete;
  template<isa<arm> Arch, class A, class B, class C>
  void sqrdmlsh(A, B, C) = delete;
  template<isa<arm> Arch, int Lane, class A, class B, class C>
  void sqrdmlsh_lane(A, B, C) = delete;


}
#endif
/// \endcond
