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
namespace native {
/** \defgroup x86_pclmul PCLMULQDQ
 * Exact multiplication of two binary polynomials of degree at most 63.
 * Bit i of an input is the coefficient of x^i. Addition is XOR, with no
 * carries or reduction; result bit 127 is always zero.
 * Imm8 bit 0 selects the high (1) or low (0) half of a, and bit 4 selects
 * the half of b. Other immediate bits are ignored. Imm8 must be in [0,255].
 * Arch records requirements; the caller must separately enable and admit its
 * target. These pure integer operations do not affect floating-point status.
 * \{ */

  /// Multiply the selected 64-bit halves into one 128-bit polynomial product.
  /// Requires PCLMUL. An AVX-enabled caller may use the VEX encoding.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::pclmul) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("pclmul")
  __m128i pclmulqdq(__m128i a, __m128i b) noexcept {
    return _mm_clmulepi64_si128(a, b, Imm8);
  }

  /// Reject implicit same-size vector conversions, including when the required
  /// feature or immediate constraint removes the callable overload.
  template<isa Arch, unsigned Imm8, class A, class B>
  void pclmulqdq(A, B) = delete;

/// \}
}
#endif
