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
/** \defgroup x86_vpclmul VPCLMULQDQ
 * Exact carry-less 64-by-64 multiplication within each 128-bit lane.
 * Imm8 bit 0 selects a's half and bit 4 selects b's half in every lane.
 * Other bits are ignored; the immediate must be in [0,255]. Each product
 * occupies its original 128-bit lane, with bit 127 zero. There are no
 * cross-lane products, carries, or polynomial reduction.
 * The 128-bit intrinsic needs PCLMUL and AVX, not the VPCLMULQDQ feature.
 * The 256-bit intrinsic needs VPCLMULQDQ and AVX, without AVX2 or AVX512VL.
 * The 512-bit intrinsic also needs AVX512F, without AVX512BW/DQ/VL.
 * Arch records requirements; callers separately enable and admit the target.
 * All forms are pure integer computations with no floating-point effects.
 * \{ */

  /// Multiply selected halves of one 128-bit lane using PCLMUL and AVX.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::pclmul) &&
      Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,pclmul")
  __m128i vpclmulqdq(__m128i a, __m128i b) noexcept {
    return _mm_clmulepi64_si128(a, b, Imm8);
  }

  /// Multiply selected halves independently in two 128-bit lanes; AVX suffices.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::vpclmulqdq) &&
      Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,vpclmulqdq")
  __m256i vpclmulqdq(__m256i a, __m256i b) noexcept {
    return _mm256_clmulepi64_epi128(a, b, Imm8);
  }

  /// Multiply selected halves independently in four 128-bit lanes; needs AVX512F.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::vpclmulqdq) &&
      Arch.has(x86_feature::avx512f) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,vpclmulqdq")
  __m512i vpclmulqdq(__m512i a, __m512i b) noexcept {
    return _mm512_clmulepi64_epi128(a, b, Imm8);
  }

  /// Reject floating registers, mixed widths and unsupported operand shapes.
  template<isa Arch, unsigned Imm8, class A, class B>
  void vpclmulqdq(A, B) = delete;

/// \}
}
#endif
