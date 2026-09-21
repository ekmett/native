// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include <cstdint>
#include <utility>
#if NATIVE_HOST_NEON
#include <arm_neon.h>

namespace native::detail::arm_neon {
  template<class V, std::size_t... I>
  native_inline native_target("neon") V register_order(V value,
    std::index_sequence<I...>) noexcept {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    // Clang maps a 128-bit inline-asm vector operand as bytes on big endian.
    // Cancel that whole-register byte permutation, independently of lane width.
    // Its 64-bit asm operands already use the ACLE register representation.
    if constexpr (sizeof(V) == 16) {
      auto raw = __builtin_bit_cast(uint8x16_t, value);
      return __builtin_bit_cast(
        V, __builtin_shufflevector(raw, raw, (15 - I)...));
    }
#endif
    return value;
  }

  template<class V> native_inline native_target("neon") V register_order(V value) noexcept {
    return register_order(value, std::make_index_sequence<16>{});
  }

  // The narrowing-high instruction overwrites the upper 64 register bits.
  // Duplicate the low vector on BE so widening needs no constant-table shuffle.
  template<class V, std::size_t... I>
  native_inline native_target("neon") auto low_register(V low, std::index_sequence<I...>) noexcept {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    using result_type = decltype(__builtin_shufflevector(low, low, I...));
    result_type result;
    asm("dup %0.2d, %1.d[0]" : "=w"(result) : "w"(low));
    return result;
#else
    return register_order(__builtin_shufflevector(low, V{}, I...));
#endif
  }

  template<class V> native_inline native_target("neon") auto low_register(V low) noexcept {
    return low_register(low, std::make_index_sequence<2 * sizeof(V) / sizeof(low[0])>{});
  }

  template<class R, class V> native_inline native_target("neon") R to_register(V value) noexcept {
    if constexpr (sizeof(R) == sizeof(typename V::native_type))
      return __builtin_bit_cast(R, value.to_native());
    else {
      auto bytes = __builtin_bit_cast(uint8x16_t, value.to_native());
      return __builtin_bit_cast(R, __builtin_shufflevector(bytes, bytes, 0, 1, 2, 3, 4, 5, 6, 7));
    }
  }

  template<class V, class R>
  native_inline native_target("neon") V from_register(R value) noexcept {
    if constexpr (sizeof(R) == sizeof(typename V::native_type))
      return V::from_native(__builtin_bit_cast(typename V::native_type, value));
    else {
      auto bytes = __builtin_bit_cast(uint8x8_t, value);
      return V::from_native(__builtin_bit_cast(
        typename V::native_type, __builtin_shufflevector(bytes, uint8x8_t{}, 0, 1, 2, 3, 4, 5, 6,
          7, 8, 9, 10, 11, 12, 13, 14, 15)));
    }
  }

  native_inline native_target("neon") int8x8_t sqadd(int8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x8_t result;
    asm volatile("sqadd %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x16_t sqadd(int8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x16_t result;
    asm volatile("sqadd %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sqadd(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("sqadd %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sqadd(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("sqadd %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sqadd(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("sqadd %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sqadd(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("sqadd %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x1_t sqadd(int64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x1_t result;
    asm volatile("sqadd %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x2_t sqadd(int64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x2_t result;
    asm volatile("sqadd %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t uqadd(uint8x8_t a, uint8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x8_t result;
    asm volatile("uqadd %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t uqadd(uint8x16_t a, uint8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x16_t result;
    asm volatile("uqadd %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t uqadd(uint16x4_t a, uint16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x4_t result;
    asm volatile("uqadd %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t uqadd(uint16x8_t a, uint16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x8_t result;
    asm volatile("uqadd %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t uqadd(uint32x2_t a, uint32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x2_t result;
    asm volatile("uqadd %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t uqadd(uint32x4_t a, uint32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x4_t result;
    asm volatile("uqadd %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x1_t uqadd(uint64x1_t a, uint64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x1_t result;
    asm volatile("uqadd %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x2_t uqadd(uint64x2_t a, uint64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x2_t result;
    asm volatile("uqadd %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x8_t sqsub(int8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x8_t result;
    asm volatile("sqsub %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x16_t sqsub(int8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x16_t result;
    asm volatile("sqsub %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sqsub(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("sqsub %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sqsub(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("sqsub %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sqsub(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("sqsub %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sqsub(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("sqsub %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x1_t sqsub(int64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x1_t result;
    asm volatile("sqsub %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x2_t sqsub(int64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x2_t result;
    asm volatile("sqsub %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t uqsub(uint8x8_t a, uint8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x8_t result;
    asm volatile("uqsub %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t uqsub(uint8x16_t a, uint8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x16_t result;
    asm volatile("uqsub %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t uqsub(uint16x4_t a, uint16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x4_t result;
    asm volatile("uqsub %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t uqsub(uint16x8_t a, uint16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x8_t result;
    asm volatile("uqsub %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t uqsub(uint32x2_t a, uint32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x2_t result;
    asm volatile("uqsub %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t uqsub(uint32x4_t a, uint32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x4_t result;
    asm volatile("uqsub %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x1_t uqsub(uint64x1_t a, uint64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x1_t result;
    asm volatile("uqsub %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x2_t uqsub(uint64x2_t a, uint64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x2_t result;
    asm volatile("uqsub %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sqdmulh(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("sqdmulh %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sqdmulh(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("sqdmulh %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sqdmulh(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("sqdmulh %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sqdmulh(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("sqdmulh %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sqrdmulh(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("sqrdmulh %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sqrdmulh(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("sqrdmulh %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sqrdmulh(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("sqrdmulh %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sqrdmulh(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("sqrdmulh %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x8_t sshl(int8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x8_t result;
    asm volatile("sshl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x16_t sshl(int8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x16_t result;
    asm volatile("sshl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sshl(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("sshl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sshl(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("sshl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sshl(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("sshl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sshl(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("sshl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x1_t sshl(int64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x1_t result;
    asm volatile("sshl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x2_t sshl(int64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x2_t result;
    asm volatile("sshl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x8_t srshl(int8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x8_t result;
    asm volatile("srshl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x16_t srshl(int8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x16_t result;
    asm volatile("srshl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t srshl(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("srshl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t srshl(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("srshl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t srshl(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("srshl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t srshl(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("srshl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x1_t srshl(int64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x1_t result;
    asm volatile("srshl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x2_t srshl(int64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x2_t result;
    asm volatile("srshl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x8_t sqshl(int8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x8_t result;
    asm volatile("sqshl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x16_t sqshl(int8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x16_t result;
    asm volatile("sqshl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sqshl(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("sqshl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sqshl(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("sqshl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sqshl(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("sqshl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sqshl(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("sqshl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x1_t sqshl(int64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x1_t result;
    asm volatile("sqshl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x2_t sqshl(int64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x2_t result;
    asm volatile("sqshl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x8_t sqrshl(int8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x8_t result;
    asm volatile("sqrshl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x16_t sqrshl(int8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int8x16_t result;
    asm volatile("sqrshl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sqrshl(int16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x4_t result;
    asm volatile("sqrshl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sqrshl(int16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int16x8_t result;
    asm volatile("sqrshl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sqrshl(int32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x2_t result;
    asm volatile("sqrshl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sqrshl(int32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int32x4_t result;
    asm volatile("sqrshl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x1_t sqrshl(int64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x1_t result;
    asm volatile("sqrshl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int64x2_t sqrshl(int64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    int64x2_t result;
    asm volatile("sqrshl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t ushl(uint8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x8_t result;
    asm volatile("ushl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t ushl(uint8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x16_t result;
    asm volatile("ushl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t ushl(uint16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x4_t result;
    asm volatile("ushl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t ushl(uint16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x8_t result;
    asm volatile("ushl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t ushl(uint32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x2_t result;
    asm volatile("ushl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t ushl(uint32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x4_t result;
    asm volatile("ushl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x1_t ushl(uint64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x1_t result;
    asm volatile("ushl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x2_t ushl(uint64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x2_t result;
    asm volatile("ushl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t urshl(uint8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x8_t result;
    asm volatile("urshl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t urshl(uint8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x16_t result;
    asm volatile("urshl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t urshl(uint16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x4_t result;
    asm volatile("urshl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t urshl(uint16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x8_t result;
    asm volatile("urshl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t urshl(uint32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x2_t result;
    asm volatile("urshl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t urshl(uint32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x4_t result;
    asm volatile("urshl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x1_t urshl(uint64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x1_t result;
    asm volatile("urshl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x2_t urshl(uint64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x2_t result;
    asm volatile("urshl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t uqshl(uint8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x8_t result;
    asm volatile("uqshl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t uqshl(uint8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x16_t result;
    asm volatile("uqshl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t uqshl(uint16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x4_t result;
    asm volatile("uqshl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t uqshl(uint16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x8_t result;
    asm volatile("uqshl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t uqshl(uint32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x2_t result;
    asm volatile("uqshl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t uqshl(uint32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x4_t result;
    asm volatile("uqshl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x1_t uqshl(uint64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x1_t result;
    asm volatile("uqshl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x2_t uqshl(uint64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x2_t result;
    asm volatile("uqshl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t uqrshl(uint8x8_t a, int8x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x8_t result;
    asm volatile("uqrshl %0.8b, %1.8b, %2.8b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t uqrshl(uint8x16_t a, int8x16_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint8x16_t result;
    asm volatile("uqrshl %0.16b, %1.16b, %2.16b" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t uqrshl(uint16x4_t a, int16x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x4_t result;
    asm volatile("uqrshl %0.4h, %1.4h, %2.4h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t uqrshl(uint16x8_t a, int16x8_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint16x8_t result;
    asm volatile("uqrshl %0.8h, %1.8h, %2.8h" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t uqrshl(uint32x2_t a, int32x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x2_t result;
    asm volatile("uqrshl %0.2s, %1.2s, %2.2s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t uqrshl(uint32x4_t a, int32x4_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint32x4_t result;
    asm volatile("uqrshl %0.4s, %1.4s, %2.4s" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x1_t uqrshl(uint64x1_t a, int64x1_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x1_t result;
    asm volatile("uqrshl %d0, %d1, %d2" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") uint64x2_t uqrshl(uint64x2_t a, int64x2_t b) noexcept {
    auto left = register_order(a);
    auto right = register_order(b);
    uint64x2_t result;
    asm volatile("uqrshl %0.2d, %1.2d, %2.2d" : "=w"(result) : "w"(left), "w"(right));
    return register_order(result);
  }

  native_inline native_target("neon") int8x8_t sqxtn(int16x8_t a) noexcept {
    auto source = register_order(a);
    int8x8_t result;
    asm volatile("sqxtn %0.8b, %1.8h" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") int8x16_t sqxtn_high(int8x8_t low, int16x8_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("sqxtn2 %0.16b, %1.8h" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") int16x4_t sqxtn(int32x4_t a) noexcept {
    auto source = register_order(a);
    int16x4_t result;
    asm volatile("sqxtn %0.4h, %1.4s" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") int16x8_t sqxtn_high(int16x4_t low, int32x4_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("sqxtn2 %0.8h, %1.4s" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") int32x2_t sqxtn(int64x2_t a) noexcept {
    auto source = register_order(a);
    int32x2_t result;
    asm volatile("sqxtn %0.2s, %1.2d" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") int32x4_t sqxtn_high(int32x2_t low, int64x2_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("sqxtn2 %0.4s, %1.2d" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t uqxtn(uint16x8_t a) noexcept {
    auto source = register_order(a);
    uint8x8_t result;
    asm volatile("uqxtn %0.8b, %1.8h" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t uqxtn_high(uint8x8_t low, uint16x8_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("uqxtn2 %0.16b, %1.8h" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t uqxtn(uint32x4_t a) noexcept {
    auto source = register_order(a);
    uint16x4_t result;
    asm volatile("uqxtn %0.4h, %1.4s" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t uqxtn_high(uint16x4_t low, uint32x4_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("uqxtn2 %0.8h, %1.4s" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t uqxtn(uint64x2_t a) noexcept {
    auto source = register_order(a);
    uint32x2_t result;
    asm volatile("uqxtn %0.2s, %1.2d" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t uqxtn_high(uint32x2_t low, uint64x2_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("uqxtn2 %0.4s, %1.2d" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x8_t sqxtun(int16x8_t a) noexcept {
    auto source = register_order(a);
    uint8x8_t result;
    asm volatile("sqxtun %0.8b, %1.8h" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint8x16_t sqxtun_high(uint8x8_t low, int16x8_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("sqxtun2 %0.16b, %1.8h" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x4_t sqxtun(int32x4_t a) noexcept {
    auto source = register_order(a);
    uint16x4_t result;
    asm volatile("sqxtun %0.4h, %1.4s" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint16x8_t sqxtun_high(uint16x4_t low, int32x4_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("sqxtun2 %0.8h, %1.4s" : "+w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x2_t sqxtun(int64x2_t a) noexcept {
    auto source = register_order(a);
    uint32x2_t result;
    asm volatile("sqxtun %0.2s, %1.2d" : "=w"(result) : "w"(source));
    return register_order(result);
  }

  native_inline native_target("neon") uint32x4_t sqxtun_high(uint32x2_t low, int64x2_t a) noexcept {
    auto result = low_register(low);
    auto source = register_order(a);
    asm volatile("sqxtun2 %0.4s, %1.2d" : "+w"(result) : "w"(source));
    return register_order(result);
  }
} // namespace native::detail::arm_neon
#endif
