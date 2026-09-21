// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native::detail::x86_pclmul {
// Internal register helpers for the native.x86.pclmul module.


  /// Multiply the selected 64-bit halves into one 128-bit polynomial product.
  /// Requires PCLMUL. An AVX-enabled caller may use the VEX encoding.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::pclmul) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("pclmul")
  __m128i pclmulqdq(__m128i a, __m128i b) noexcept {
    return _mm_clmulepi64_si128(a, b, Imm8);
  }

  /// Reject implicit same-size vector conversions, including when the required
  /// feature or immediate constraint removes the callable overload.
  template<isa<x86> Arch, unsigned Imm8, class A, class B>
  void pclmulqdq(A, B) = delete;


}
#endif
