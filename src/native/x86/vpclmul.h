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
namespace native::detail::x86_vpclmul {
// Internal register helpers for the native.x86.vpclmul module.


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


}
#endif
