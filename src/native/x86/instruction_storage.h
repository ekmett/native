// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include <hint.h>
#include <bit>
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native::detail {
  // Instruction boundaries use complete logical register shapes. Permitted
  // storage can be scalar or split while an operation's hardware is available.
  // These casts preserve bytes without adding a second public vector API.
  template<class V> requires(sizeof(typename V::native_type) == 16)
  [[nodiscard]] hint_inline hint_target("sse2")
  __m128i x86_integer_register(V value) noexcept {
    return __builtin_bit_cast(__m128i, value.to_native());
  }

  template<class V, class R> requires(sizeof(R) == 16)
  [[nodiscard]] hint_inline hint_target("sse2")
  V x86_instruction_result(R value) noexcept {
    static_assert(sizeof(typename V::native_type) == sizeof(R));
    return V::from_native(__builtin_bit_cast(typename V::native_type, value));
  }

  template<class V> requires(sizeof(typename V::native_type) == 32)
  [[nodiscard]] hint_inline hint_target("avx")
  __m256i x86_integer_register(V value) noexcept {
    return __builtin_bit_cast(__m256i, value.to_native());
  }

  template<class V, class R> requires(sizeof(R) == 32)
  [[nodiscard]] hint_inline hint_target("avx")
  V x86_instruction_result(R value) noexcept {
    static_assert(sizeof(typename V::native_type) == sizeof(R));
    return V::from_native(__builtin_bit_cast(typename V::native_type, value));
  }

  template<class V> requires(sizeof(typename V::native_type) == 64)
  [[nodiscard]] hint_inline hint_target("avx512f")
  __m512i x86_integer_register(V value) noexcept {
    return __builtin_bit_cast(__m512i, value.to_native());
  }

  template<class V, class R> requires(sizeof(R) == 64)
  [[nodiscard]] hint_inline hint_target("avx512f")
  V x86_instruction_result(R value) noexcept {
    static_assert(sizeof(typename V::native_type) == sizeof(R));
    return V::from_native(__builtin_bit_cast(typename V::native_type, value));
  }

  template<class V> requires(sizeof(typename V::native_type) == 16)
  [[nodiscard]] hint_inline hint_target("sse2")
  __m128 x86_float_register(V value) noexcept {
    return __builtin_bit_cast(__m128, value.to_native());
  }
  template<class V> requires(sizeof(typename V::native_type) == 32)
  [[nodiscard]] hint_inline hint_target("avx")
  __m256 x86_float_register(V value) noexcept {
    return __builtin_bit_cast(__m256, value.to_native());
  }
  // Four logical half lanes can occupy eight scalar bytes or a native register.
  // Copy only their representation; unused native register words are zero.
  template<class V> requires(sizeof(typename V::native_type) == 8 ||
                            sizeof(typename V::native_type) == 16)
  [[nodiscard]] hint_inline hint_target("sse2")
  __m128i x86_half_register(V value) noexcept {
    auto source = value.to_native();
    __m128i result{};
    __builtin_memcpy(&result, &source, sizeof(source));
    return result;
  }
  template<class V, class R> requires(sizeof(R) == 16 && (sizeof(V) == 8 || sizeof(V) == 16))
  [[nodiscard]] hint_inline hint_target("sse2")
  V x86_half_result(R value) noexcept {
    if constexpr (sizeof(V) == 16) return __builtin_bit_cast(V, value);
    else {
      std::uint64_t bits;
      __builtin_memcpy(&bits, &value, sizeof(bits));
      return __builtin_bit_cast(V, bits);
    }
  }

}
#endif
