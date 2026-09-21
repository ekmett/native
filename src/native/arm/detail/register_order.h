// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
  namespace detail {
    // At a Clang inline-assembly boundary on big-endian AArch64,
    // 128-bit vector-to-byte asm coercion requires reversing all bytes,
    // including each element's bytes. Its 64-bit coercion already has native
    // register order. Apply the same involution on inputs and outputs.
    template<class T>
    native_inline T arm_register_order(T x) noexcept {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      if constexpr(sizeof(T) == 8) {
        return x;
      } else {
        auto bytes = __builtin_bit_cast(uint8x16_t, x);
        return __builtin_bit_cast(T, __builtin_shufflevector(bytes, bytes,
          15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0));
      }
#else
      return x;
#endif
    }
  }
}
#endif
